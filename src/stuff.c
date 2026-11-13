// Bunch of handy util functions for CLI
#include <stdio.h>
#include <stdlib.h>
#include <libpict.h>

int ptp_list_devices(void) {
	struct PtpRuntime *r = ptp_new(PTP_USB);

	struct PtpDeviceList *list = ptpusb_device_list(r);
	if (list == NULL || list->n_devices == 0) {
		printf("No PTP/USB devices found.\n");
		return 0;
	}

	for (unsigned int i = 0; i < list->n_devices; i++) {
		printf("Device #%d:\n", i);
		printf("  product id: %04x\n", list->devices[i].product_id);
		printf("  vendor id: %04x\n", list->devices[i].vendor_id);
		printf("  Vendor friendly name: '%s'\n", list->devices[i].manufacturer);
		printf("  Model friendly name: '%s'\n", list->devices[i].name);
	}

	ptp_close(r);

	return 0;
}

struct PtpRuntime *ptp_connect_from_id(int id) {
	if (id == -1) ptp_panic("%s Bug: -1", __func__);
	struct PtpRuntime *r = ptp_new(PTP_USB);
	struct PtpDeviceList *list = ptpusb_device_list(r);
	if (list == NULL || list->n_devices == 0) {
		printf("No PTP/USB devices found.\n");
		return 0;
	}

	for (unsigned int i = 0; i < list->n_devices; i++) {
		if (i == id) {
			int rc = ptp_device_open(r, &list->devices[i]);
			if (rc) return NULL;
			return r;
		}
	}

	ptp_close(r);
	return NULL;
}