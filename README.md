# libpict
This is a Picture Transfer Protocol (PTP) library written in C.  

[Documentation](https://danielc.dev/libpict/structPtpRuntime.html)

*(This library doesn't have a stable release yet, use at your own risk)*

## Features
- Complete working implemention of PTP as per ISO 15740
- Working PTP/IP implementation (tested on Windows/Linux/Mac/Android/iOS)
- LibUSB and [libwpd](https://github.com/petabyt/libwpd) USB backends
- APIs to convert most PTP data structures to JSON
- Implements basic EOS/Canon vendor functionality
- Lua bindings
- Thread safe
- Uses a single in/out buffer (no memory allocations between operations)
- Fujifilm support: Available [here as libfudge](https://github.com/petabyt/fudge)


## Sample
Get device info:
```
#include <libpict.h>

int main() {
	struct PtpRuntime *r = ptp_new(PTP_USB);

	if (ptp_device_init(r)) {
		printf("Device connection error\n");
		return 0;
	}

	struct PtpDeviceInfo di;

	char buffer[2048];
	ptp_get_device_info(r, &di);
	ptp_device_info_json(&di, buffer, sizeof(buffer));
	printf("%s\n", buffer);

	ptp_device_close(r);
	ptp_close(r);
	return 0;
}
```
Calling a custom opcode:
```
// Send a command, and recieve packet(s)
struct PtpCommand cmd;
cmd.code = 0x1234;
cmd.param_length = 3;
cmd.params[0] = 420;
cmd.params[1] = 420;
cmd.params[2] = 420;
return ptp_send(r, &cmd);

// Send a command with data payload
struct PtpCommand cmd;
cmd.code = 0x1234;
cmd.param_length = 1;
cmd.params[0] = 1234;
uint32_t dat[2] = {123, 123};
return ptp_send_data(r, &cmd, dat, sizeof(dat));
```
Explore the filesystem:
```
struct PtpArray *arr;
int rc = ptp_get_storage_ids(r, &arr);
int id = arr->data[0];
free(arr);
rc = ptp_get_object_handles(r, id, PTP_OF_JPEG, 0, &arr);

for (int i = 0; i < arr->length; i++) {
	struct PtpObjectInfo oi;
	ptp_get_object_info(r, arr->data[i], &oi);
	printf("Filename: %s\n", oi.filename);
}

free(arr);
```

## License
Licensed under the Apache License 2.0.  
lua-cjson: http://tools.ietf.org/html/rfc4627 (MIT License)  
