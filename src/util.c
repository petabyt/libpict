// Helper/convenient functions
// Copyright 2022 by Daniel C (https://github.com/petabyt/camlib)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <camlib.h>
#include <ptp.h>

void ptp_generic_reset(struct PtpRuntime *r) {
	r->active_connection = 0;
	r->transaction = 0;
	r->session = 0;	
	r->max_packet_size = 512;
	r->data_phase_length = 0;
	r->di = NULL;
	r->connection_type = PTP_USB;
	r->caller_unlocks_mutex = 0;
	r->mutex = NULL;
}

void ptp_generic_init(struct PtpRuntime *r) {
	ptp_generic_reset(r);
	r->data = malloc(CAMLIB_DEFAULT_SIZE);
	r->data_length = CAMLIB_DEFAULT_SIZE;

	#ifndef CAMLIB_DONT_USE_MUTEX
	r->mutex = malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(r->mutex, NULL)) {
		ptp_verbose_log("Failed to init mutex\n");
		free(r->mutex);
		r->mutex = NULL;
	}
	#endif
}

void ptp_mutex_lock(struct PtpRuntime *r) {
	if (r->mutex == NULL) return;
	pthread_mutex_lock(r->mutex);
}

void ptp_mutex_keep_locked(struct PtpRuntime *r) {
	r->caller_unlocks_mutex = 1;
}

void ptp_mutex_unlock(struct PtpRuntime *r) {
	if (r->mutex == NULL) return;
	pthread_mutex_unlock(r->mutex);
	r->caller_unlocks_mutex = 0;
}

void ptp_generic_close(struct PtpRuntime *r) {
	free(r->data);
}

// Perform a "generic" command type transaction. Could be a macro, but macros suck
int ptp_generic_send(struct PtpRuntime *r, struct PtpCommand *cmd) {
	ptp_mutex_lock(r);

	r->data_phase_length = 0;

	int length = ptp_new_cmd_packet(r, cmd);
	if (ptp_send_bulk_packets(r, length) != length) {
		ptp_mutex_unlock(r);
		return PTP_IO_ERR;
	}

	if (ptp_receive_bulk_packets(r) < 0) {
		ptp_mutex_unlock(r);
		return PTP_IO_ERR;
	}

	r->transaction++;

	if (ptp_get_return_code(r) == PTP_RC_OK) {
		if (!r->caller_unlocks_mutex) ptp_mutex_unlock(r);
		return 0;
	} else {
		printf("Invalid return code: %X\n", ptp_get_return_code(r));
		if (!r->caller_unlocks_mutex) ptp_mutex_unlock(r);
		return 0;
		return PTP_CHECK_CODE;
	}
}

// Send a cmd packet, then data packet
// Perform a generic operation with a data phase to the camera
int ptp_generic_send_data(struct PtpRuntime *r, struct PtpCommand *cmd, void *data, int length) {
	ptp_mutex_lock(r);

	r->data_phase_length = length;

	// Send operation request packet data phase 2
	int plength = ptp_new_cmd_packet(r, cmd);
	if (ptp_send_bulk_packets(r, plength) != plength) {
		ptp_mutex_unlock(r);
		return PTP_IO_ERR;
	}

	// new start data packet
	plength = ptp_new_data_packet(r, cmd);

	if (plength + length > r->data_length) {
		ptp_verbose_log("ptp_generic_send_data: Not enough memory\n");
		ptp_mutex_unlock(r);
		return PTP_OUT_OF_MEM;
	}

	memcpy(ptp_get_payload(r), data, length);
	ptp_update_data_length(r, plength + length);

	if (ptp_send_bulk_packets(r, plength + length) != plength + length) {
		ptp_mutex_unlock(r);
		return PTP_IO_ERR;
	}

	if (ptp_receive_bulk_packets(r) < 0) {
		ptp_mutex_unlock(r);
		return PTP_IO_ERR;
	}

	r->transaction++;

	if (ptp_get_return_code(r) == PTP_RC_OK) {
		if (!r->caller_unlocks_mutex) ptp_mutex_unlock(r);
		return 0;
	} else {
		if (!r->caller_unlocks_mutex) ptp_mutex_unlock(r);
		return PTP_CHECK_CODE;
	}
}

struct UintArray *ptp_dup_uint_array(struct UintArray *arr) {
	struct UintArray *arr2 = malloc(4 + arr->length * 4);
	if (arr2 == NULL) return NULL;
	memcpy(arr2, arr, 4 + arr->length * 4);
	return arr2;
}

// May be slightly inneficient for every frame/action
// TODO: maybe 'cache' dev type for speed
int ptp_device_type(struct PtpRuntime *r) {
	struct PtpDeviceInfo *di = r->di;
	if (di == NULL) return PTP_DEV_EMPTY;
	if (!strcmp(di->manufacturer, "Canon Inc.")) {
		if (ptp_check_opcode(r, PTP_OC_EOS_GetStorageIDs)) {
			return PTP_DEV_EOS;
		}

		return PTP_DEV_CANON;
	} else if (!strcmp(di->manufacturer, "FUJIFILM")) {
		return PTP_DEV_FUJI;
	} else if (!strcmp(di->manufacturer, "Sony Corporation")) {
		return PTP_DEV_SONY;
	} else if (!strcmp(di->manufacturer, "Nikon Corporation")) {
		return PTP_DEV_NIKON;
	}

	return PTP_DEV_EMPTY;
}

int ptp_check_opcode(struct PtpRuntime *r, int op) {
	if (r->di == NULL) return 0;
	for (int i = 0; i < r->di->ops_supported_length; i++) {
		if (r->di->ops_supported[i] == op) {
			return 1;
		}
	}

	return 0;
}

int ptp_check_prop(struct PtpRuntime *r, int code) {
	if (r->di == NULL) return 0;
	for (int i = 0; i < r->di->props_supported_length; i++) {
		if (r->di->props_supported[i] == code) {
			return 1;
		}
	}

	return 0;
}

int ptp_dump(struct PtpRuntime *r) {
	FILE *f = fopen("DUMP", "w");
	fwrite(r->data, r->data_length, 1, f);
	fclose(f);
	return 0;
}
