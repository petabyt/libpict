#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include <libpict.h>

static const uint8_t *fuzz_input = NULL;
static unsigned int fuzz_input_length = 0;
static unsigned int fuzz_input_of = 0;

static void fuzz_set_input(const uint8_t *data, unsigned int length) {
	fuzz_input = data;
	fuzz_input_length = length;
	fuzz_input_of = 0;
}

static int fuzz_read(void *data, unsigned int size) {
	if (fuzz_input_of >= fuzz_input_length) return -1;
	unsigned int n = size;
	if (n > fuzz_input_length - fuzz_input_of) n = fuzz_input_length - fuzz_input_of;
	memcpy(data, fuzz_input + fuzz_input_of, n);
	fuzz_input_of += n;
	return (int)n;
}

int ptpip_cmd_read(struct PtpRuntime *r, void *data, unsigned int size) { (void)r; return fuzz_read(data, size); }
int ptp_cmd_read(struct PtpRuntime *r, void *to, unsigned int length) { (void)r; return fuzz_read(to, length); }

int ptpip_cmd_write(struct PtpRuntime *r, void *data, unsigned int size) { (void)r; (void)data; return (int)size; }
int ptp_cmd_write(struct PtpRuntime *r, void *to, unsigned int length) { (void)r; (void)to; return (int)length; }

int ptpip_new_timeout_socket(char *addr, int port) { (void)addr; (void)port; return -1; }
int ptpip_connect(struct PtpRuntime *r, const char *addr, int port, int extra_tmout) { (void)r; (void)addr; (void)port; (void)extra_tmout; return -1; }
int ptpip_connect_events(struct PtpRuntime *r, const char *addr, int port) { (void)r; (void)addr; (void)port; return -1; }
int ptpip_device_close(struct PtpRuntime *r) { (void)r; return -1; }
int ptpip_event_send(struct PtpRuntime *r, void *data, unsigned int size) { (void)r; (void)data; (void)size; return -1; }
int ptpip_event_read(struct PtpRuntime *r, void *data, unsigned int size) { (void)r; (void)data; (void)size; return -1; }
int ptpip_video_read(struct PtpRuntime *r, void *data, unsigned int size) { (void)r; (void)data; (void)size; return -1; }
int ptp_device_connect(struct PtpRuntime *r) { (void)r; return -1; }
int ptp_device_close(struct PtpRuntime *r) { (void)r; return -1; }
int ptp_device_reset(struct PtpRuntime *r) { (void)r; return -1; }
int ptp_read_int(struct PtpRuntime *r, void *to, unsigned int length) { (void)r; (void)to; (void)length; return -1; }
int reset_int(void) { return -1; }
int ptpusb_get_status(struct PtpRuntime *r) { (void)r; return -1; }
int ptp_comm_init(struct PtpRuntime *r) { (void)r; return 0; }
void ptp_comm_deinit(struct PtpRuntime *r) { (void)r; }
void ptpip_comm_deinit(struct PtpRuntime *r) { (void)r; }
void ptpusb_free_device_list_entry(void *ptr) { (void)ptr; }
struct PtpDeviceEntry *ptpusb_device_list(struct PtpRuntime *r) { (void)r; return NULL; }
int ptp_device_open(struct PtpRuntime *r, struct PtpDeviceEntry *entry) { (void)r; (void)entry; return -1; }
int ptp_device_connect_from_libusb_handle(struct PtpRuntime *r, void *dev) { (void)r; (void)dev; abort(); }

void ptp_verbose_log(struct PtpRuntime *r, const char *fmt, ...) { (void)r; (void)fmt; }
void ptp_error_log(struct PtpRuntime *r, const char *fmt, ...) { (void)r; (void)fmt; }

static jmp_buf fuzz_panic_env;
static int fuzz_panic_armed = 0;

void ptp_panic(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "PTP abort: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	if (fuzz_panic_armed) longjmp(fuzz_panic_env, 1);
	abort();
}

static unsigned int fuzzed_runs = 0;

static void fuzz_free_runtime(struct PtpRuntime *r) {
	struct PtpPropAvail *n = r->avail;
	while (n != NULL) {
		struct PtpPropAvail *next = n->next;
		free(n->data);
		free(n);
		n = next;
	}

	#ifndef PTP_DONT_USE_MUTEX
	free(r->mutex);
	#endif

	ptp_close(r);
}

static void fuzz_one(const uint8_t *data, unsigned int length, int connection_type) {
	struct PtpRuntime *r = ptp_new(connection_type);

	r->response_wait_default = 0;
	r->wait_for_response = 0;

	if (length >= 1) {
		r->max_packet_size = 1 + (((unsigned int)data[length - 1] * 8) % 4096);
	} else {
		r->max_packet_size = 512;
	}

	fuzz_set_input(data, length);

	int rc = ptp_receive_all_packets(r);

	if (rc < PTP_COMMAND_IGNORED) {
		fprintf(stderr, "fuzz: unexpected return code %d\n", rc);
		abort();
	}
	if (r->data_filled_length > r->data_length) {
		fprintf(stderr, "fuzz: data_filled_length %u > data_length %u\n",
			r->data_filled_length, r->data_length);
		abort();
	}

	fuzz_free_runtime(r);
	fuzzed_runs++;
}

static void fuzz_seed(const uint8_t *data, unsigned int length) {
	fuzz_one(data, length, PTP_IP);
	fuzz_one(data, length, PTP_USB);
}

static void fuzz_file(const char *path) {
	FILE *fp = fopen(path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "fuzz: cannot open %s\n", path);
		exit(1);
	}

	static uint8_t buffer[65536];
	unsigned int length = (unsigned int)fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);

	printf("fuzz: %s (%u bytes)\n", path, length);
	fuzz_seed(buffer, length);
}

static struct PtpRuntime *fuzz_parser_setup(const uint8_t *payload, unsigned int length) {
	struct PtpRuntime *r = ptp_new(PTP_USB);
	memset(r->data, 0, r->data_length);

	ptp_write_u32(r->data, 12 + length);
	ptp_write_u16(r->data + 4, PTP_PACKET_TYPE_DATA);
	ptp_write_u16(r->data + 6, 0x0000);
	ptp_write_u32(r->data + 8, 1);
	memcpy(r->data + 12, payload, length);
	r->data_filled_length = 12 + length;

	return r;
}

static void fuzz_parser_run(int (*parser)(struct PtpRuntime *, void *), struct PtpRuntime *r, void *out) {
	fuzz_panic_armed = 1;
	if (setjmp(fuzz_panic_env) == 0) {
		parser(r, out);
	}
	fuzz_panic_armed = 0;
}

int main(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		fuzz_file(argv[i]);
	}

	{
		uint8_t ones[64];
		memset(ones, 0xFF, sizeof(ones));
		fuzz_seed(ones, sizeof(ones));

		uint8_t zeros[64];
		memset(zeros, 0x00, sizeof(zeros));
		fuzz_seed(zeros, sizeof(zeros));

		uint8_t alt[64];
		for (unsigned int i = 0; i < sizeof(alt); i++) {
			alt[i] = (i & 1) ? 0xFF : 0x00;
		}
		fuzz_seed(alt, sizeof(alt));

		uint8_t one[1] = {0xFF};
		fuzz_seed(one, sizeof(one));

		fuzz_seed((const uint8_t *)"", 0);
	}

	{
		static const uint8_t ip_response[] = {
			0x0E, 0x00, 0x00, 0x00,
			0x07, 0x00, 0x00, 0x00,
			0x01, 0x00,
			0x01, 0x00, 0x00, 0x00,
		};
		fuzz_seed(ip_response, sizeof(ip_response));
	}

	{
		static const uint8_t ip_data[] = {
			0x0C, 0x00, 0x00, 0x00,
			0x09, 0x00, 0x00, 0x00,
			0x01, 0x00, 0x00, 0x00,
			0x0C, 0x00, 0x00, 0x00,
			0x0C, 0x00, 0x00, 0x00,
			0x01, 0x00, 0x00, 0x00,
			0x0E, 0x00, 0x00, 0x00,
			0x07, 0x00, 0x00, 0x00,
			0x01, 0x00,
			0x01, 0x00, 0x00, 0x00,
		};
		fuzz_seed(ip_data, sizeof(ip_data));
	}

	{
		static const uint8_t usb_data[] = {
			0x0C, 0x00, 0x00, 0x00,
			0x02, 0x00,
			0x01, 0x00,
			0x01, 0x00, 0x00, 0x00,
			0x0C, 0x00, 0x00, 0x00,
			0x03, 0x00,
			0x01, 0x00,
			0x01, 0x00, 0x00, 0x00,
		};
		fuzz_seed(usb_data, sizeof(usb_data));
	}

	{
		static const uint8_t usb_response[] = {
			0x0C, 0x00, 0x00, 0x00,
			0x03, 0x00,
			0x01, 0x00,
			0x01, 0x00, 0x00, 0x00,
		};
		fuzz_seed(usb_response, sizeof(usb_response));
	}

	{
		static const uint8_t object_info[] = {
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		};
		struct PtpRuntime *r = fuzz_parser_setup(object_info, sizeof(object_info));
		struct PtpObjectInfo oi;
		memset(&oi, 0, sizeof(oi));
		fuzz_parser_run((int (*)(struct PtpRuntime *, void *))ptp_parse_object_info, r, &oi);
		fuzz_free_runtime(r);
		fuzzed_runs++;
	}

	{
		static const uint8_t prop_desc[] = {
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		};
		struct PtpRuntime *r = fuzz_parser_setup(prop_desc, sizeof(prop_desc));
		struct PtpPropDesc pd;
		memset(&pd, 0, sizeof(pd));
		fuzz_parser_run((int (*)(struct PtpRuntime *, void *))ptp_parse_prop_desc, r, &pd);
		free(pd.enum_form);
		free(pd.default_value);
		free(pd.current_value);
		fuzz_free_runtime(r);
		fuzzed_runs++;
	}

	srand((unsigned int)time(NULL));

	static uint8_t buffer[2048];
	for (int i = 0; i < 10000; i++) {
		unsigned int length = (unsigned int)rand() % sizeof(buffer);
		for (unsigned int x = 0; x < length; x++) {
			buffer[x] = (uint8_t)rand();
		}

		fuzz_one(buffer, length, PTP_IP);
		fuzz_one(buffer, length, PTP_USB);
	}

	printf("fuzz: %u runs, no issues found\n", fuzzed_runs);
	return 0;
}
