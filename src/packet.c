// Packet generation, parsing, and manipulation routines
// Copyright 2022 by Daniel C (https://github.com/petabyt/camlib)

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <ptp.h>
#include <camlib.h>

uint8_t ptp_read_uint8(void **dat) {
	uint8_t **p = (uint8_t **)dat;
	uint8_t x = (**p);
	(*p)++;
	return x;
}

uint16_t ptp_read_uint16(void **dat) {
	uint16_t **p = (uint16_t **)dat;
	uint16_t x = (**p);
	(*p)++;
	return x;
}

uint32_t ptp_read_uint32(void **dat) {
	uint32_t **p = (uint32_t **)dat;
	uint32_t x = (**p);
	(*p)++;
	return x;
}

// Read UTF16 string
void ptp_read_string(void **dat, char *string, int max) {
	uint8_t **p = (uint8_t **)dat;
	int length = (int)ptp_read_uint8((void **)p);

	int y = 0;
	while (y < length) {
		string[y] = (char)(**p);
		(*p)++;
		//printf("'%c'\n", (char)(*p));
		//assert((char)(**dat) == '\0');
		(*p)++;
		y++;
		if (y >= max) { break; }
	}

	string[y] = '\0';
}

int ptp_read_uint16_array(void **dat, uint16_t *buf, int max) {
	int n = ptp_read_uint32((void **)dat);

	for (int i = 0; i < n; i++) {
		if (i >= max) {
			(void)ptp_read_uint16((void **)dat);
		} else {
			buf[i] = ptp_read_uint16((void **)dat);
		}
	}

	return n;
}

void ptp_write_uint8(void **dat, uint8_t b) {
	uint8_t **ptr = (uint8_t **)dat;
	(**ptr) = b;
	(*ptr)++;
}

int ptp_write_string(void **dat, char *string) {
	int length = strlen(string);
	ptp_write_uint8(dat, length);

	for (int i = 0; i < length; i++) {
		ptp_write_uint8(dat, string[i]);
		ptp_write_uint8(dat, '\0');
	}

	ptp_write_uint8(dat, '\0');

	return (length * 2) + 2;
}

// Write non-PTP standard unicode string
int ptp_write_unicode_string(char *dat, char *string) {
	int i;
	for (i = 0; string[i] != '\0'; i++) {
		dat[i * 2] = string[i];
		dat[i * 2 + 1] = '\0';
	}
	dat[i * 2 + 1] = '\0';
	return i;
}

int ptp_read_unicode_string(char *buffer, char *dat, int max) {
	int i;
	for (i = 0; dat[i] != '\0'; i += 2) {
		buffer[i / 2] = dat[i];
		if (i >= max) {
			buffer[(i / 2) + 1] = '\0';
			return i;
		}
	}

	buffer[(i / 2)] = '\0';
	return i / 2;
}

// PTP/IP-specific packet
int ptpip_bulk_packet(struct PtpRuntime *r, struct PtpCommand *cmd, int type) {
	struct PtpIpBulkContainer bulk;
	int size = 18 + (sizeof(uint32_t) * cmd->param_length);
	bulk.length = size;
	bulk.type = type;
	bulk.length += cmd->data_length;
	bulk.code = cmd->code;
	bulk.transaction = r->transaction;

	if (r->data_phase_length == 0) {
		bulk.data_phase = 1;
	} else {
		bulk.data_phase = 2;
	}

	for (int i = 0; i < 5; i++) {
		bulk.params[i] = cmd->params[i];
	}

	memcpy(r->data, &bulk, size);

	return size;
}

int ptpip_data_start_packet(struct PtpRuntime *r, int data_length) {
	struct PtpIpStartDataPacket *pkt = (struct PtpIpStartDataPacket *)(r->data);
	pkt->length = 0x20;
	pkt->type = PTPIP_DATA_PACKET_START;
	pkt->transaction = r->transaction;
	pkt->data_phase_length = (uint64_t)data_length;	

	return pkt->length;
}

int ptpip_data_end_packet(struct PtpRuntime *r, void *data, int data_length) {
	struct PtpIpEndDataPacket *pkt = (struct PtpIpEndDataPacket *)(r->data);
	pkt->length = 12 + data_length;
	pkt->type = PTPIP_DATA_PACKET_END;
	pkt->transaction = r->transaction;

	memcpy(((uint8_t *)r->data) + 12, data, data_length);

	return pkt->length;
}

// Generate a USB-only BulkContainer packet
int ptpusb_bulk_packet(struct PtpRuntime *r, struct PtpCommand *cmd, int type) {
	struct PtpBulkContainer bulk;
	int size = 12 + (sizeof(uint32_t) * cmd->param_length);

	bulk.length = size;
	bulk.type = type;
	bulk.length += cmd->data_length;
	bulk.code = cmd->code;
	bulk.transaction = r->transaction;

	for (int i = 0; i < 5; i++) {
		bulk.params[i] = cmd->params[i];
	}

	memcpy(r->data, &bulk, size);

	return size;
}

// Only for PTP_USB or PTP_USB_IP
int ptp_new_data_packet(struct PtpRuntime *r, struct PtpCommand *cmd, void *data, int data_length) {
	cmd->param_length = 0;

	int length = ptpusb_bulk_packet(r, cmd, PTP_PACKET_TYPE_DATA);
	memcpy(ptp_get_payload(r), data, data_length);
	ptp_update_data_length(r, length + data_length);	
	return length;
}

// Generate a IP or USB style command packet (both are pretty similar)
int ptp_new_cmd_packet(struct PtpRuntime *r, struct PtpCommand *cmd) {
	cmd->data_length = 0;
	if (r->connection_type == PTP_IP) {
		int length = ptpip_bulk_packet(r, cmd, PTPIP_COMMAND_REQUEST);
		return length;
	} else {
		int length = ptpusb_bulk_packet(r, cmd, PTP_PACKET_TYPE_COMMAND);
		return length;
	}
}

// Update the length of the data packet after creation
// TODO: This is a bad function, should be deleted
void ptp_update_data_length(struct PtpRuntime *r, int length) {
	if (r->connection_type == PTP_IP) {
		// Should only be one packet here (?)
		// struct PtpIpStartDataPacket *ds = (struct PtpIpStartDataPacket*)(r->data);
		// if (ds->type != PTPIP_DATA_PACKET_START) {
			// exit(1);
		// }
// 
		// ds->data_phase_length = length;
		
		struct PtpIpHeader *de = (struct PtpIpHeader*)(r->data);
		if (ds->type != PTPIP_DATA_PACKET_END) {
			// TODO: camlib_fatal
			exit(1);
		}

		// Update the packet length for the end packet
		de->length = 12 + length;
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);
		bulk->length = length;
	}
}

// Get data start packet, then data end packet, then response packet
static struct PtpIpResponseContainer *ptpip_get_response_packet(struct PtpRuntime *r) {
	struct PtpIpStartDataPacket *ds = (struct PtpIpStartDataPacket*)(r->data);
	if (ds->type == PTPIP_COMMAND_RESPONSE) {
		// If there is no data phase, return first packet
		return (struct PtpIpResponseContainer *)(r->data);
	}

	if (ds->type != PTPIP_DATA_PACKET_START) {
		exit(1);
	}

	struct PtpIpEndDataPacket *de = (struct PtpIpEndDataPacket *)(r->data + ds->length);
	if (de->type != PTPIP_DATA_PACKET_END) {
		exit(1);
	}

	struct PtpIpResponseContainer *resp = (struct PtpIpResponseContainer *)(r->data + ds->length + de->length);
	if (resp->type != PTPIP_COMMAND_RESPONSE) {
		exit(1);
	}

	return resp;
}

// Update transid for current request packet
void ptp_update_transaction(struct PtpRuntime *r, int t) {
	if (r->connection_type == PTP_IP) {
		struct PtpIpBulkContainer *bulk = (struct PtpIpBulkContainer *)(r->data);
		bulk->transaction = t;
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer *)(r->data);
		bulk->transaction = t;
	}
}

// Get rccode from response packet
int ptp_get_return_code(struct PtpRuntime *r) {
	if (r->connection_type == PTP_IP) {
		struct PtpIpResponseContainer *resp = ptpip_get_response_packet(r);
		return resp->code;
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);
		if (bulk->type == PTP_PACKET_TYPE_DATA) {
			bulk = (struct PtpBulkContainer*)(r->data + bulk->length);
			return bulk->code;
		} else {
			return bulk->code;
		}
	}
}

// Get ptr to payload
uint8_t *ptp_get_payload(struct PtpRuntime *r) {
	if (r->connection_type == PTP_IP) {
		// For IP, payload is in the DATA_END packet
		struct PtpIpStartDataPacket *ds = (struct PtpIpStartDataPacket*)(r->data);
		if (ds->type != PTPIP_DATA_PACKET_START) {
			//ptp_verbose_log("Fatal: non data start packet\n");
			exit(1);
		}

		struct PtpIpHeader *de = (struct PtpIpHeader*)(r->data + ds->length);
		if (de->type != PTPIP_DATA_PACKET_END) {
			//ptp_verbose_log("Fatal: non data end packet, got %X\n", de->type);
			exit(1);
		}

		return r->data + ds->length + 12;
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);
		if (bulk->type == PTP_PACKET_TYPE_RESPONSE) {
			return r->data + 12;
		} else if (bulk->type == PTP_PACKET_TYPE_COMMAND) {
			return r->data + 28;
		} else {
			// TODO: Fatal
			return r->data + 12;
		}
	}
}

int ptp_get_payload_length(struct PtpRuntime *r) {
	if (r->connection_type == PTP_IP) {
		struct PtpIpStartDataPacket *ds = (struct PtpIpStartDataPacket*)(r->data);
		if (ds->type != PTPIP_DATA_PACKET_START) {
			exit(1);
		}

		return (int)ds->data_phase_length;
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);
		return bulk->length - 12;
	}
}

int ptp_get_param_length(struct PtpRuntime *r) {
	if (r->connection_type == PTP_IP) {
		struct PtpIpResponseContainer *resp = ptpip_get_response_packet(r);	
		return (resp->length - 14) / 4;	
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);

		// Get response packet, which backend stores after data packet
		if (bulk->type == PTP_PACKET_TYPE_DATA) {
			bulk = (struct PtpBulkContainer*)(r->data + bulk->length);
		}

		return (bulk->length - 12) / 4;
	}
}

uint32_t ptp_get_param(struct PtpRuntime *r, int index) {
	if (r->connection_type == PTP_IP) {
		struct PtpIpResponseContainer *resp = ptpip_get_response_packet(r);	
		return resp->params[index];		
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);

		// Get response packet, which backend stores after data packet
		if (bulk->type == PTP_PACKET_TYPE_DATA) {
			bulk = (struct PtpBulkContainer*)(r->data + bulk->length);
			return bulk->code;
		}

		return bulk->params[index];
	}
}

int ptp_get_last_transaction(struct PtpRuntime *r) {
	if (r->connection_type == PTP_IP) {
		struct PtpIpResponseContainer *resp = ptpip_get_response_packet(r);	
		return resp->transaction;		
	} else {
		struct PtpBulkContainer *bulk = (struct PtpBulkContainer*)(r->data);
		if (bulk->type == PTP_PACKET_TYPE_DATA) {
			bulk = (struct PtpBulkContainer*)(r->data + bulk->length);
			return bulk->transaction;
		} else {
			return bulk->transaction;
		}
	}
}
