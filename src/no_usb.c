// Include this if you wish to never use USB
#include <libpict.h>
#include <stdlib.h>
#include <ptp.h>

int ptp_device_connect(struct PtpRuntime *r) {
    return -1;
}

int ptp_device_close(struct PtpRuntime *r) {
    return -1;
}

int ptp_device_reset(struct PtpRuntime *r) {
    return -1;
}

int ptp_cmd_write(struct PtpRuntime *r, void *to, unsigned int length) {
    return -1;
}

int ptp_cmd_read(struct PtpRuntime *r, void *to, unsigned int length) {
    return -1;
}

int ptp_read_int(struct PtpRuntime *r, void *to, unsigned int length) {
    return -1;
}

int reset_int(void) {
    return -1;
}

void ptp_comm_deinit(struct PtpRuntime *r) {}

int ptpusb_get_status(struct PtpRuntime *r) {
    return -1;
}

// shouldn't be called in normal circumstances
int ptp_device_connect_from_libusb_handle(struct PtpRuntime *r, void *dev) { abort(); }