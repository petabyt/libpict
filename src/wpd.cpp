// Windows Portable Devices PTP backend
// WPD HRESULT Error codes: https://learn.microsoft.com/en-us/windows/win32/wpd_sdk/error-constants
// Copyright 2023 Daniel Cook
#include <windows.h>
#include <conio.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <objbase.h>
#include <initguid.h>
#include <portabledeviceapi.h>
#include <portabledevice.h>
#include <cstdint>
#include "libpict.h"
#include "ptp.h"

#define CLIENT_MAJOR_VER	1
#define CLIENT_MINOR_VER	0
#define CLIENT_REVISION		0

// Polyfill for MinGW
#ifndef WPD_COMMAND_MTP_EXT_EXECUTE_COMMAND_WITHOUT_DATA_PHASE
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_GET_SUPPORTED_VENDOR_OPCODES, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 11);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_EXECUTE_COMMAND_WITHOUT_DATA_PHASE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 12);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_EXECUTE_COMMAND_WITH_DATA_TO_READ, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 13);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_EXECUTE_COMMAND_WITH_DATA_TO_WRITE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 14);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_END_DATA_TRANSFER, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 15);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_GET_VENDOR_EXTENSION_DESCRIPTION, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 16);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_READ_DATA, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 17);
DEFINE_PROPERTYKEY(WPD_COMMAND_MTP_EXT_WRITE_DATA, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 18);

DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_OPERATION_CODE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1001);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_OPERATION_PARAMS, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1002);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_RESPONSE_CODE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1003);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_RESPONSE_PARAMS, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1004);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_TOTAL_DATA_SIZE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1005);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_OPTIMAL_TRANSFER_BUFFER_SIZE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1006);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_CONTEXT, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1007);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_TO_READ, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1008);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_READ, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1009);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_TO_WRITE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1010);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_WRITTEN, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1011);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_TRANSFER_DATA, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1012);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_VENDOR_OPCODE, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1013);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_VENDOR_OPERATION_CODES, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1014);
DEFINE_PROPERTYKEY(WPD_PROPERTY_MTP_EXT_VENDOR_EXTENSION_DESCRIPTION, 0x4d545058, 0x1a2e, 0x4106, 0xa3, 0x57, 0x77, 0x1e, 0x8, 0x19, 0xfc, 0x56, 1015);
#endif

struct PtpCommPriv {
	IPortableDevice *pDevice;
	IPortableDeviceValues *spResults;
	IPortableDeviceValues *pDevValues;
	IPortableDeviceValues *pClientInformation;
	wchar_t *device_path;
};

struct LibWPDDescriptor {
	int product_id;
	int vendor_id;
};

static int wpd_parse_io_path(const wchar_t *path, struct LibWPDDescriptor *d) {
	int len = wcslen(path);
	if (len < 30) {
		return -1;
	}
	
	if (wcsncmp(L"\\\\?\\usb", path, 7)) {
		return -1;
	}

	int of = 7;

	if (path[of] != L'#') {
		return -2;
	}
	of++;

	for (int i = 0; i < 2; i++) {
		if (path[of] == L'&') of++;
	
		if (i == 0) {
			if (wcsncmp(L"vid_", &path[of], 4)) {
				return -1;
			}
		} else if (i == 1) {
			if (wcsncmp(L"pid_", &path[of], 4)) {
				return -1;
			}
		}
		of += 4;

		uint32_t id = 0;

		while (1) {
			char c = (char)path[of];
			if (c >= '0' && c <= '9') {
				id *= 16;
				id += c - '0';
			} else if (c >= 'a' && c <= 'z') {
				id *= 16;
				id += c - 'a' + 10;
			} else {
				break;
			}
			of++;
		}

		if (i == 0) {
			d->vendor_id = id;
		} else if (i == 1) {
			d->product_id = id;
		}
		if (path[of] == L'#') {
			break;
		}
	}

	return 0;
}

int ptp_comm_init(struct PtpRuntime *r) {
	// We are not using low-level I/O operations, so this is never used
	r->max_packet_size = 512;

	if (r->comm_priv != NULL) return 0;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		ptp_error_log(r, "Couldn't coinitialize hr=%d err=%d\n", hr, GetLastError());
		return 1;
	}

	r->comm_priv = (struct PtpCommPriv *)calloc(1, sizeof(struct PtpCommPriv));
	
	return 0;
}

void ptp_comm_deinit(struct PtpRuntime *r) {
	// ...
}

int ptp_device_close(struct PtpRuntime *r) {
	struct PtpCommPriv *priv = r->comm_priv;
	HRESULT hr = 0;
	if (priv->pDevice != NULL) hr = priv->pDevice->Close();
	if (priv->pDevValues != NULL) hr |= priv->pDevValues->Clear();
	if (priv->spResults != NULL) hr |= priv->spResults->Clear();

	if (FAILED(hr)) {
		ptp_error_log(r, "Failed to close the device\n");
	}

	return FAILED(hr);
}

int ptpusb_get_status(struct PtpRuntime *r) {
	struct PtpCommPriv *priv = r->comm_priv;
	if (priv == NULL) return PTP_IO_ERR;
	HANDLE h = CreateFileW(priv->device_path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (h == INVALID_HANDLE_VALUE) {
		return -1;
	}
	return 0;
}

int ptp_device_reset(struct PtpRuntime *r) {
	return 0;
}

struct PtpDeviceEntry *ptpusb_device_list(struct PtpRuntime *r) {
	ptp_comm_init(r);
	//struct PtpCommPriv *priv = (struct PtpCommPriv *)r->comm_priv;
	ptp_mutex_lock(r);

	IPortableDeviceManager *pPortableDeviceManager = nullptr;

	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pPortableDeviceManager));
	if (FAILED(hr)) {
		ptp_error_log(r, "CoCreateInstance Error %X\n", HRESULT_FROM_WIN32(GetLastError()));
		ptp_mutex_unlock(r);
		return NULL;
	}

	DWORD mNumDevices;
	hr = pPortableDeviceManager->GetDevices(NULL, &mNumDevices);
	if (FAILED(hr)) {
		ptp_error_log(r, "Error getting devices length\n");
		ptp_mutex_unlock(r);
		return NULL;
	}

	ptp_error_log(r, "Detected %d devices.\n", mNumDevices);

	wchar_t **deviceIDs = (wchar_t **)malloc(sizeof(wchar_t *) * mNumDevices);

	hr = pPortableDeviceManager->GetDevices(deviceIDs, &mNumDevices);
	if (FAILED(hr)) {
		ptp_error_log(r, "Error getting devices\n");
		ptp_mutex_unlock(r);
		return NULL;
	}

	struct PtpDeviceEntry *curr_ent = (struct PtpDeviceEntry *)calloc(1, sizeof(struct PtpDeviceEntry));
	struct PtpDeviceEntry *new_list = curr_ent;
	for (unsigned int i = 0; i < mNumDevices; i++) {
		if (i != 0) {
			struct PtpDeviceEntry *next_ent = (struct PtpDeviceEntry *)calloc(1, sizeof(struct PtpDeviceEntry));
			next_ent->prev = curr_ent;
			next_ent->next = NULL;
			curr_ent->next = next_ent;
			curr_ent = next_ent;
		} else {
			curr_ent->prev = NULL;
		}

		struct LibWPDDescriptor d;
		int rc = wpd_parse_io_path(deviceIDs[i], &d);
		if (rc) ptp_panic("wpd_parse_io_path");

		curr_ent->product_id = d.product_id;
		curr_ent->vendor_id = d.vendor_id;
		curr_ent->device_handle_ptr = deviceIDs[i];
	}

	for (unsigned int i = 0; i < mNumDevices; i++) {
		CoTaskMemFree(deviceIDs[i]);
	}
	free(deviceIDs);

	ptp_mutex_unlock(r);
	return new_list;
}

int ptp_device_open(struct PtpRuntime *r, struct PtpDeviceEntry *entry) {
	ptp_mutex_lock(r);
	struct PtpCommPriv *priv = r->comm_priv;

	wchar_t *deviceId = (wchar_t *)entry->device_handle_ptr;

	if (deviceId == NULL) return PTP_NO_DEVICE;

	priv->device_path = _wcsdup(deviceId);

	LPCWSTR wszPnPDeviceID = deviceId;

	HRESULT hr = S_OK;
	IPortableDeviceValues *pClientInformation = NULL;

	hr = CoCreateInstance(
		CLSID_PortableDeviceValues,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IPortableDeviceValues,
		(VOID**)&pClientInformation
	);
	if (hr) {
		ptp_error_log(r, "Failed to create portable devices");
		return hr;
	}

	if (FAILED(pClientInformation->SetStringValue(WPD_CLIENT_NAME, L"libpict"))) {
		ptp_error_log(r, "Failed to set WPD_CLIENT_NAME: %d\n", GetLastError());
		return 1;
	}

	if (FAILED(pClientInformation->SetUnsignedIntegerValue(WPD_CLIENT_MAJOR_VERSION, CLIENT_MAJOR_VER))) {
		ptp_error_log(r, "Failed to set WPD_CLIENT_MAJOR_VERSION: %d\n", GetLastError());
		return 1;
	}

	if (FAILED(pClientInformation->SetUnsignedIntegerValue(WPD_CLIENT_MINOR_VERSION, CLIENT_MINOR_VER))) {
		ptp_error_log(r, "Failed to set WPD_CLIENT_MINOR_VERSION: %d\n", GetLastError());
		return 1;
	}

	if (FAILED(pClientInformation->SetUnsignedIntegerValue(WPD_CLIENT_REVISION, CLIENT_REVISION))) {
		ptp_error_log(r, "Failed to set WPD_CLIENT_REVISION: %d\n", GetLastError());
		return 1;
	}

	if (FAILED(pClientInformation->SetUnsignedIntegerValue(WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE, SECURITY_IMPERSONATION))) {
		ptp_error_log(r, "Failed to set WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE: %d\n", GetLastError());
		return 1;
	}

	hr = CoCreateInstance(
		CLSID_PortableDeviceFTM,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IPortableDevice,
		(VOID**)&priv->pDevice
	);
	if (hr) {
		ptp_error_log(r, "Failed to create portable device\n");
		return 1;
	}

	hr = priv->pDevice->Open(wszPnPDeviceID, pClientInformation);
	if (hr == E_ACCESSDENIED) {
		// Attempt to open for read-only access
		pClientInformation->SetUnsignedIntegerValue(WPD_CLIENT_DESIRED_ACCESS, GENERIC_READ);
		hr = priv->pDevice->Open(wszPnPDeviceID, pClientInformation);
	}

	if (FAILED(hr)) {
		ptp_error_log(r, "Failed to open device %X\n", hr);
		return 1;
	}

	priv->pClientInformation = pClientInformation;

	// TODO:
	//HANDLE handle = CreateEvent(nullptr, false, false, nullptr);

	ptp_error_log(r, "Opened device %ls\n", wszPnPDeviceID);

	ULONG type = 0;
	{
		IPortableDeviceContent *content;
		hr = priv->pDevice->Content(&content);
		if (hr) { ptp_error_log(r, "content fail\n"); return -1; }

		IPortableDeviceProperties *properties;
		hr = content->Properties(&properties);
		if (hr) { ptp_error_log(r, "properties fail (%d)\n", hr); return -1; }

		IPortableDeviceKeyCollection *keys = NULL;
		IPortableDeviceValues *values = NULL;
		properties->GetValues(WPD_DEVICE_OBJECT_ID, keys, &values);

		hr = values->GetUnsignedIntegerValue(WPD_DEVICE_TYPE, &type);
		if (hr) {
			ptp_error_log(r, "WPD_DEVICE_TYPE not found\n");
			return -1;
		}
		ptp_verbose_log(r, "Device type: %d\n", type);

		values->Release();
		properties->Release();
		content->Release();
	}

	// WPD_DEVICE_TYPE_GENERIC could be an unrecognized camera
	if (type == WPD_DEVICE_TYPE_CAMERA || type == WPD_DEVICE_TYPE_GENERIC) {
		r->io_kill_switch = 0;
		r->operation_kill_switch = 0;
		ptp_mutex_unlock(r);
		return 0;
	}

	ptp_verbose_log(r, "Device is not a camera! (%d)\n", type);

	ptp_mutex_unlock(r);
	return PTP_OPEN_FAIL;
}

int ptp_device_connect(struct PtpRuntime *r) {
	// TODO
	return PTP_NO_DEVICE;
}

void ptpusb_free_device_list_entry(void *handle) {
	// TODO
}

static int wpd_prep_command(struct PtpRuntime *r, PROPERTYKEY type, struct PtpCommand *cmd) {
	struct PtpCommPriv *priv = r->comm_priv;
	priv->pDevValues = nullptr;
	HRESULT hr = CoCreateInstance(
		CLSID_PortableDeviceValues,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&priv->pDevValues)
	);

	hr |= priv->pDevValues->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, type.fmtid);
	hr |= priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, type.pid);
	hr |= priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_OPERATION_CODE, (ULONG)cmd->code);
	if (hr) {
		ptp_error_log(r, "Error setting command info\n");
		return hr;
	}

	IPortableDevicePropVariantCollection *spMtpParams = nullptr;
	hr = CoCreateInstance(
		CLSID_PortableDevicePropVariantCollection,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&spMtpParams)
	);
	if (hr) {
		ptp_error_log(r, "Error creating prop variant collection\n");
		return hr;
	}

	for (unsigned int i = 0; i < cmd->param_length; i++) {
		PROPVARIANT param = { 0 };
		param.vt = VT_UI4;
		param.ulVal = cmd->params[i];
		spMtpParams->Add(&param);
	}

	hr = priv->pDevValues->SetIPortableDevicePropVariantCollectionValue(WPD_PROPERTY_MTP_EXT_OPERATION_PARAMS, spMtpParams);
	if (SUCCEEDED(hr)) {
		ptp_error_log(r, "Set parameters\n");
	} else {
		ptp_error_log(r, "Error setting parameters\n");
		return hr;
	}

	spMtpParams->Release();

	return 0;
}

// Send the required "completion" command
static int send_finished_command(struct PtpRuntime *r, struct PtpCommand *cmd, wchar_t *pwszContext) {
	struct PtpCommPriv *priv = r->comm_priv;
	priv->spResults->Release();
	priv->pDevValues->Clear();

	HRESULT hr = priv->pDevValues->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_MTP_EXT_END_DATA_TRANSFER.fmtid);
	hr |= priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_MTP_EXT_END_DATA_TRANSFER.pid);
	hr |= priv->pDevValues->SetStringValue(WPD_PROPERTY_MTP_EXT_TRANSFER_CONTEXT, pwszContext);

	hr |= priv->pDevice->SendCommand(0, priv->pDevValues, &priv->spResults);
	if (SUCCEEDED(hr)) {
		ptp_error_log(r, "Sent the completion command.\n");
	} else {
		ptp_error_log(r, "Failed to send the completion command\n");
		return -1;
	}

	priv->spResults->GetErrorValue(WPD_PROPERTY_COMMON_HRESULT, &hr);
	if (FAILED(hr)) {
		ptp_error_log(r, "Completion command failed: %X\n", hr);
		return -1;
	}

	IPortableDevicePropVariantCollection *params;
	hr = priv->spResults->GetIPortableDevicePropVariantCollectionValue(WPD_PROPERTY_MTP_EXT_RESPONSE_PARAMS, &params);
	if (FAILED(hr)) {
		ptp_error_log(r, "Failed to get response params\n");
	} else {
		DWORD params_count;
		params->GetCount(&params_count);

		PROPVARIANT prop;
		cmd->param_length = (int)params_count;
		for (DWORD i = 0; i < params_count; i++) {
			params->GetAt(i, &prop);
			cmd->params[i] = prop.ulVal;
			PropVariantClear(&prop);
		}
	}

	DWORD dwResponseCode = 0;
	hr = priv->spResults->GetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_RESPONSE_CODE, &dwResponseCode);
	if (FAILED(hr)) {
		ptp_error_log(r, "Failed to get response code %X\n", hr);
		dwResponseCode = 0x2000;
	}
	ptp_error_log(r, "Return code: 0x%x\n", dwResponseCode);
	cmd->code = (int)dwResponseCode;
	return 0;
}

static uint32_t wpd_get_optimal_data_size(struct PtpRuntime *r) {
	struct PtpCommPriv *priv = r->comm_priv;
	ULONG cbOptimalDataSize = 0;
	HRESULT hr = priv->spResults->GetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_TRANSFER_TOTAL_DATA_SIZE, &cbOptimalDataSize);
	if (FAILED(hr)) {
		ptp_error_log(r, "Failed to get WPD_PROPERTY_MTP_EXT_TRANSFER_TOTAL_DATA_SIZE\n");
	}
	ptp_error_log(r, "Optimal data size: %d\n", cbOptimalDataSize);
	return cbOptimalDataSize;
}

static int wpd_receive_do_data(struct PtpRuntime *r, struct PtpCommand *cmd, uint8_t *buffer, int length) {
	struct PtpCommPriv *priv = r->comm_priv;
	wchar_t *pwszContext = NULL;
	priv->spResults->GetStringValue(WPD_PROPERTY_MTP_EXT_TRANSFER_CONTEXT, &pwszContext);

	HRESULT hr = priv->pDevValues->Clear();
	hr |= priv->pDevValues->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_MTP_EXT_READ_DATA.fmtid);
	hr |= priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_MTP_EXT_READ_DATA.pid);
	hr |= priv->pDevValues->SetStringValue(WPD_PROPERTY_MTP_EXT_TRANSFER_CONTEXT, pwszContext);
	hr |= priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_TO_READ, length);
	hr |= priv->pDevValues->SetBufferValue(WPD_PROPERTY_MTP_EXT_TRANSFER_DATA, buffer, length);

	hr = priv->pDevice->SendCommand(0, priv->pDevValues, &priv->spResults);
	if (FAILED(hr)) {
		ptp_error_log(r, "Failed to receive data");
		return -1;
	}

	priv->spResults->GetErrorValue(WPD_PROPERTY_COMMON_HRESULT, &hr);
	if (hr) {
		ptp_error_log(r, "Failed to finish command (%X)\n", hr);
		return -1;
	}

	BYTE *bufferOut = NULL;
	ULONG cbBytesRead = 0;
	hr = priv->spResults->GetBufferValue(WPD_PROPERTY_MTP_EXT_TRANSFER_DATA, &bufferOut, &cbBytesRead);
	if (hr) {
		ptp_error_log(r, "Failed to get data\n");
		return -1;
	}

	ptp_error_log(r, "Read %d bytes\n", cbBytesRead);

	memcpy(buffer, bufferOut, cbBytesRead);

	hr = send_finished_command(r, cmd, pwszContext);
	if (hr) return hr;

	CoTaskMemFree(pwszContext);

	return cbBytesRead;
}

int ptp_send_packet(struct PtpRuntime *r, unsigned int length) {
	if (r->io_kill_switch) return PTP_IO_ERR;
	struct PtpCommPriv *priv = r->comm_priv;
	if (priv == NULL) return PTP_IO_ERR;

	struct PtpBulkContainer *bulk = (struct PtpBulkContainer *)(r->data);
	if (bulk->code == PTP_OC_OpenSession || bulk->code == PTP_OC_CloseSession) {
		return length;
	}

	struct PtpCommand cmd;
	cmd.code = bulk->code;
	cmd.param_length = ptp_get_param_length(r);
	for (unsigned int i = 0; i < cmd.param_length; i++) {
		cmd.params[i] = ptp_get_param(r, i);
	}

	if (bulk->type == PTP_PACKET_TYPE_COMMAND) {	
		if (r->data_phase_length) {
			priv->spResults = NULL;
			HRESULT hr = wpd_prep_command(r, WPD_COMMAND_MTP_EXT_EXECUTE_COMMAND_WITH_DATA_TO_WRITE, &cmd);
			if (FAILED(hr)) {
				ptp_error_log(r, "Failed to prep command\n");
				return -1;
			}

			hr = priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_TRANSFER_TOTAL_DATA_SIZE, length);
			if (FAILED(hr)) {
				ptp_error_log(r, "Failed to set data size\n");
				return -1;
			}

			hr = priv->pDevice->SendCommand(0, priv->pDevValues, &priv->spResults);
			if (FAILED(hr)) {
				ptp_error_log(r, "Failed to send command\n");
				return -1;
			}
		
			r->data_phase_length = 0;
		} else {
			priv->spResults = NULL;
			HRESULT hr = wpd_prep_command(r, WPD_COMMAND_MTP_EXT_EXECUTE_COMMAND_WITH_DATA_TO_READ, &cmd);
			if (FAILED(hr)) {
				ptp_error_log(r, "Failed to prep command\n");
				return -1;
			}
			hr = priv->pDevice->SendCommand(0, priv->pDevValues, &priv->spResults);
			if (FAILED(hr)) {
				ptp_error_log(r, "Failed to send command\n");
				return -1;
			}

			uint32_t payload_size = wpd_get_optimal_data_size(r);
			const int max_packet_minus_payload_size = 12 + 12;
			if (payload_size > (r->data_length - max_packet_minus_payload_size)) {
				ptp_buffer_resize(r, payload_size + max_packet_minus_payload_size);
				bulk = (struct PtpBulkContainer *)(r->data);
			}

			// Receive the PTP response
			payload_size = wpd_receive_do_data(r, &cmd, r->data + 12, r->data_length - 12 - 12);

			// Place data packet and cmd packet in the out buffer
			struct PtpBulkContainer *out_cmd = (struct PtpBulkContainer *)(r->data);
			out_cmd->length = 12 + payload_size;
			out_cmd->type = PTP_PACKET_TYPE_DATA;
			out_cmd->code = cmd.code;
			out_cmd->transaction = bulk->transaction + 1;
			length = 12 + payload_size;

			// Place the response packet 12 bytes
			struct PtpBulkContainer *resp = (struct PtpBulkContainer *)(r->data + length);
			resp->length = 12;
			resp->type = PTP_PACKET_TYPE_RESPONSE;
			resp->code = cmd.code;
			resp->transaction = bulk->transaction + 1;
			length += 12;
		}

		return length;
	} else if (bulk->type == PTP_PACKET_TYPE_DATA) {
		cmd.param_length = 0;

		wchar_t *pwszContext = NULL;
		priv->spResults->GetStringValue(WPD_PROPERTY_MTP_EXT_TRANSFER_CONTEXT, &pwszContext);

		priv->pDevValues->Clear();

		HRESULT hr = priv->pDevValues->SetGuidValue(WPD_PROPERTY_COMMON_COMMAND_CATEGORY, WPD_COMMAND_MTP_EXT_WRITE_DATA.fmtid);
		if (FAILED(hr)) { ptp_error_log(r, "FAILED X\n"); return -1; }
		hr = priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_COMMON_COMMAND_ID, WPD_COMMAND_MTP_EXT_WRITE_DATA.pid);
		if (FAILED(hr)) { ptp_error_log(r, "FAILED Y\n"); return -1; }
		hr = priv->pDevValues->SetStringValue(WPD_PROPERTY_MTP_EXT_TRANSFER_CONTEXT, pwszContext);
		if (FAILED(hr)) { ptp_error_log(r, "FAILED Z\n"); return -1; }
		hr = priv->pDevValues->SetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_TO_WRITE, length);
		if (FAILED(hr)) { ptp_error_log(r, "FAILED A\n"); return -1; }
		hr = priv->pDevValues->SetBufferValue(WPD_PROPERTY_MTP_EXT_TRANSFER_DATA, ptp_get_payload(r), length - 12);
		if (FAILED(hr)) {
			ptp_error_log(r, "Failed to set data properties\n");
			return -1;
		}

		hr = priv->pDevice->SendCommand(0, priv->pDevValues, &priv->spResults);
		if (SUCCEEDED(hr)) {
			hr = priv->spResults->GetErrorValue(WPD_PROPERTY_COMMON_HRESULT, &hr);
			if (SUCCEEDED(hr)) {
				ptp_error_log(r, "Successfully sent data\n");
			} else {
				ptp_error_log(r, "Failed to send data\n");
				return -1;
			}
		} else {
			ptp_error_log(r, "Failed to send data\n");
			return -1;
		}

		DWORD cbBytesWritten = 0;
		hr = priv->spResults->GetUnsignedIntegerValue(WPD_PROPERTY_MTP_EXT_TRANSFER_NUM_BYTES_WRITTEN, &cbBytesWritten);

		ptp_verbose_log(r, "Sent %d bytes\n", cbBytesWritten);

		hr = send_finished_command(r, &cmd, pwszContext);
		if (hr) return hr;

		CoTaskMemFree(pwszContext);

		r->data_phase_length = 0; // Data was sent

		bulk->length = 12;
		bulk->type = PTP_PACKET_TYPE_RESPONSE;
		bulk->code = cmd.code;
		bulk->transaction = r->transaction + 1;
		
		return length;
	} else {
		return PTP_IO_ERR;
	}

	return 0;
}

int ptp_receive_all_packets(struct PtpRuntime *r) {
	// ptp_send_packet does everything
	return 0;
}
