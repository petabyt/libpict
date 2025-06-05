// Test EOS events
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libpict.h>

int main(void) {
	struct PtpRuntime *r = ptp_new(PTP_USB);

	if (ptp_device_init(r)) {
		puts("Device connection error");
		return 0;
	}

	ptp_open_session(r);

	struct PtpDeviceInfo di;

	ptp_get_device_info(r, &di);
	char temp[4096];
	ptp_device_info_json(&di, temp, sizeof(temp));
	printf("%s\n", temp);

	int length = 0;
	struct PtpGenericEvent *s = NULL;
	if (ptp_device_type(r) == PTP_DEV_EOS) {
		ptp_eos_set_remote_mode(r, 1);
		ptp_eos_set_event_mode(r, 1);

		int rc = ptp_eos_get_event(r);
		if (rc) return rc;

		struct PtpEventReader reader;
		ptp_eos_events_open(r, &reader);

		struct PtpGenericEvent event;
		int i = 0;
		while (ptp_eos_events_next(r, &reader, &event) == 0) {
			printf("%X = %X\n", event.code, event.value);
		}
	}

	ptp_device_close(r);
	ptp_close(r);
	return 0;
}

