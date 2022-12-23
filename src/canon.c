// Canon and EOS Specific operation implementations
// Copyright 2022 by Daniel C (https://github.com/petabyt/camlib)

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <ptp.h>
#include <backend.h>
#include <camlib.h>

int ptp_eos_remote_release_on(struct PtpRuntime *r, int mode) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_RemoteReleaseOn;
	cmd.param_length = 2;
	cmd.params[0] = mode;
	cmd.params[1] = 0;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_remote_release_off(struct PtpRuntime *r, int mode) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_RemoteReleaseOff;
	cmd.param_length = 1;
	cmd.params[0] = mode;
	return ptp_generic_send(r, &cmd);
}

// Add 0x8000 to param1 change direction
int ptp_eos_drive_lens(struct PtpRuntime *r, int steps) {
	if (steps < 0) {
		steps = 0x8000 + (steps * -1);
	}
	
	printf("%X\n", steps);

	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_DriveLens;
	cmd.param_length = 1;
	cmd.params[0] = steps;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_set_event_mode(struct PtpRuntime *r, int mode) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_SetEventMode;
	cmd.param_length = 1;
	cmd.params[0] = mode;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_set_remote_mode(struct PtpRuntime *r, int mode) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_SetRemoteMode;
	cmd.param_length = 1;
	cmd.params[0] = mode;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_get_viewfinder_data(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_GetViewFinderData;
	cmd.param_length = 1;

	cmd.params[0] = 0x200000;
	cmd.params[1] = 0x0;
	cmd.params[2] = 0x0;

	return ptp_generic_send(r, &cmd);
}

int ptp_eos_get_prop_value(struct PtpRuntime *r, int code) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_GetDevicePropValue;
	cmd.param_length = 1;
	cmd.params[0] = code;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_set_prop_value(struct PtpRuntime *r, int code, int value) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_SetDevicePropValueEx;
	cmd.param_length = 3;
	
	cmd.params[0] = 0xc;
	cmd.params[1] = code;
	cmd.params[2] = value;

	// cmd and data packets should be the same

	int length = ptp_new_cmd_packet(r, &cmd);
	if (ptp_send_bulk_packets(r, length) != length) return PTP_IO_ERR;

	length = ptp_new_data_packet(r, &cmd);

	if (ptp_send_bulk_packets(r, length) != length) return PTP_IO_ERR;

	if (ptp_recieve_bulk_packets(r) < 0) return PTP_IO_ERR;
	return 0;
}

// This is the same operation as ptp_eos_set_prop_value, but is more spiffy
int ptp_eos_set_prop_data(struct PtpRuntime *r, int code, void *data, int dlength) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_SetDevicePropValueEx;
	cmd.param_length = 3;
	
	cmd.params[0] = 0x10 + dlength;
	cmd.params[1] = code;
	cmd.params[2] = ((uint32_t *)data)[0];

	// CMD packet should only be 12 bytes long (?)

	int length = ptp_new_cmd_packet(r, &cmd);
	if (ptp_send_bulk_packets(r, length) != length) return PTP_IO_ERR;

	length = ptp_new_data_packet(r, &cmd);
	length += dlength - 4;
	ptp_update_data_length(r, length);
	memcpy(ptp_get_payload(r) + 8, data, dlength);

	if (ptp_send_bulk_packets(r, length) != length) return PTP_IO_ERR;

	if (ptp_recieve_bulk_packets(r) < 0) return PTP_IO_ERR;
	return 0;
}


int ptp_eos_get_event(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_GetEvent;
	cmd.param_length = 0;
	return ptp_generic_send(r, &cmd);	
}

int ptp_eos_ping(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_KeepDeviceOn;
	cmd.param_length = 0;
	return ptp_generic_send(r, &cmd);	
}

// The HDD capacity is pushed/popped (for lack of a better term)
// Not sure why, but his is how EOS Utility does it.
int ptp_eos_hdd_capacity_push(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_PCHDDCapacity;
	cmd.param_length = 3;
	cmd.params[0] = 0xfffffff7;
	cmd.params[1] = 0x1000;
	cmd.params[2] = 0x0;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_hdd_capacity_pop(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_PCHDDCapacity;
	cmd.param_length = 3;
	cmd.params[0] = 0x332d2d;
	cmd.params[1] = 0x1000;
	cmd.params[2] = 0x1;
	return ptp_generic_send(r, &cmd);
}

int ptp_eos_bulb_start(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_BulbStart;
	cmd.param_length = 0;
	return ptp_generic_send(r, &cmd);	
}

int ptp_eos_bulb_stop(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_BulbEnd;
	cmd.param_length = 0;
	return ptp_generic_send(r, &cmd);	
}

int ptp_eos_set_ui_lock(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_SetUILock;
	cmd.param_length = 1;
	cmd.params[0] = 0;
	return ptp_generic_send(r, &cmd);	
}

int ptp_eos_reset_ui_lock(struct PtpRuntime *r) {
	struct PtpCommand cmd;
	cmd.code = PTP_OC_EOS_ResetUILock;
	cmd.param_length = 0;
	return ptp_generic_send(r, &cmd);	
}
