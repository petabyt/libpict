#include <stdio.h>
#include <libpict.h>

/* Some snippets, if you dare to run them:
Show green on display:
ptp_eos_evproc_run(r, "UndisplayPhysicalScreen");
ptp_eos_evproc_run(r, "CreateColor %d %d %d", 0, 70, 0);

Dump gangfile, all 0xFF:
ptp_eos_evproc_run(r, "gang 'B:/ASD.BIN'");

Write to RAM (or ROM, if you're not careful):
ptp_eos_evproc_run(r, "writeaddr %d %d", 0x3780, 0x12345678);
*/

int main(void) {
	int rc;
	struct PtpDeviceInfo di;
	struct PtpRuntime *r = ptp_new(PTP_USB);

	if (ptp_device_connect(r)) {
		puts("Device connection error");
		return 0;
	}

	rc = ptp_open_session(r);
	if (rc) return rc;

	char buffer[4096];
	rc = ptp_get_device_info(r, &di);
	if (rc) return rc;
	r->di = &di;
	ptp_device_info_json(&di, buffer, sizeof(buffer));
	printf("%s\n", (char*)buffer);

	if (ptp_device_type(r) == PTP_DEV_EOS) {
		ptp_eos_set_remote_mode(r, 1);
		ptp_eos_set_event_mode(r, 1);

		// TODO: Do EOS stuff
	} else {
		printf("Is not an EOS device.\n");
	}

	rc = ptp_close_session(r);
	if (rc) return rc;

	ptp_device_close(r);
	ptp_close(r);
	return 0;
}

