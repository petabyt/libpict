// Scan filesystem
#include <libpict.h>

int main() {
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
	ptp_device_info_json(&di, buffer, sizeof(buffer));
	printf("%s\n", (char*)buffer);

	rc = ptp_close_session(r);
	if (rc) return rc;
	ptp_device_close(r);
	ptp_close(r);

#ifdef WIN32
	printf("Press any key to close\n");
	getchar();
#endif
	return 0;
}

