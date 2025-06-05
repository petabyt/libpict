// Test basic opcode, get device properties
#include <stdio.h>
#include <libpict.h>

int main(void) {
	struct PtpRuntime *r = ptp_new(PTP_USB);

	struct PtpDeviceInfo di;

	if (ptp_device_init(r)) {
		puts("Device connection error");
		return 0;
	}

	ptp_open_session(r);

	ptp_close_session(r);
	ptp_device_close(r);
	ptp_close(r);
	return 0;
}

