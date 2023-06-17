// autogenerated file
#include <cl_enum.h>
struct PtpEnum ptp_enums[] = {
{PTP_ENUM, 0, "PTP_PACKET_TYPE_COMMAND", 0x1},
{PTP_ENUM, 0, "PTP_PACKET_TYPE_DATA", 0x2},
{PTP_ENUM, 0, "PTP_PACKET_TYPE_RESPONSE", 0x3},
{PTP_ENUM, 0, "PTP_PACKET_TYPE_EVENT", 0x4},
{PTP_OC, 0, "GetDeviceInfo", 0x1001},
{PTP_OC, 0, "OpenSession", 0x1002},
{PTP_OC, 0, "CloseSession", 0x1003},
{PTP_OC, 0, "GetStorageIDs", 0x1004},
{PTP_OC, 0, "GetStorageInfo", 0x1005},
{PTP_OC, 0, "GetNumObjects", 0x1006},
{PTP_OC, 0, "GetObjectHandles", 0x1007},
{PTP_OC, 0, "GetObjectInfo", 0x1008},
{PTP_OC, 0, "GetObject", 0x1009},
{PTP_OC, 0, "GetThumb", 0x100A},
{PTP_OC, 0, "DeleteObject", 0x100B},
{PTP_OC, 0, "SendObjectInfo", 0x100C},
{PTP_OC, 0, "SendObject", 0x100D},
{PTP_OC, 0, "InitiateCapture", 0x100E},
{PTP_OC, 0, "FormatStore", 0x100F},
{PTP_OC, 0, "ResetDevice", 0x1010},
{PTP_OC, 0, "SelfTest", 0x1011},
{PTP_OC, 0, "SetObjectProtection", 0x1012},
{PTP_OC, 0, "PowerDown", 0x1013},
{PTP_OC, 0, "GetDevicePropDesc", 0x1014},
{PTP_OC, 0, "GetDevicePropValue", 0x1015},
{PTP_OC, 0, "SetDevicePropValue", 0x1016},
{PTP_OC, 0, "ResetDevicePropValue", 0x1017},
{PTP_OC, 0, "TerminateOpenCapture", 0x1018},
{PTP_OC, 0, "MoveObject", 0x1019},
{PTP_OC, 0, "CopyObject", 0x101A},
{PTP_OC, 0, "GetPartialObject", 0x101B},
{PTP_OC, 0, "InitiateOpenCapture", 0x101C},
{PTP_OC, 0, "MTP_GetObjectPropsSupported", 0x9801},
{PTP_OC, 0, "MTP_GetObjectPropDesc", 0x9802},
{PTP_OC, 0, "MTP_GetObjectPropValue", 0x9803},
{PTP_OC, 0, "MTP_SetObjectPropValue", 0x9804},
{PTP_OC, 0, "MTP_GetObjPropList", 0x9805},
{PTP_OC, 0, "MTP_SetObjPropList", 0x9806},
{PTP_OC, 0, "MTP_SendObjectPropList", 0x9808},
{PTP_OC, 0, "MTP_GetObjectReferences", 0x9810},
{PTP_OC, 0, "MTP_SetObjectReferences", 0x9811},
{PTP_OC, 0, "MTP_UpdateDeviceFirmware", 0x9812},
{PTP_OC, 0, "MTP_Skip", 0x9820},
{PTP_OC, 3, "Capture", 0x90C0},
{PTP_OC, 3, "AfCaptureSDRAM", 0x90CB},
{PTP_OC, 3, "StartLiveView", 0x9201},
{PTP_OC, 3, "EndLiveView", 0x9202},
{PTP_OC, 3, "GetEvent", 0x90C7},
{PTP_OC, 2, "ViewFinderOn", 0x900B},
{PTP_OC, 2, "ViewFinderOff", 0x900C},
{PTP_OC, 2, "InitCaptureInRAM", 0x901A},
{PTP_OC, 2, "GetViewFinderImage", 0x901D},
{PTP_OC, 2, "LockUI", 0x9004},
{PTP_OC, 2, "UnlockUI", 0x9005},
{PTP_OC, 1, "GetStorageIDs", 0x9101},
{PTP_OC, 1, "GetStorageInfo", 0x9102},
{PTP_OC, 1, "SetDevicePropValueEx", 0x9110},
{PTP_OC, 1, "SetRemoteMode", 0x9114},
{PTP_OC, 1, "SetEventMode", 0x9115},
{PTP_OC, 1, "GetEvent", 0x9116},
{PTP_OC, 1, "PCHDDCapacity", 0x911A},
{PTP_OC, 1, "SetUILock", 0x911B},
{PTP_OC, 1, "ResetUILock", 0x911C},
{PTP_OC, 1, "KeepDeviceOn", 0x911D},
{PTP_OC, 1, "UpdateFirmware", 0x911F},
{PTP_OC, 1, "BulbStart", 0x9125},
{PTP_OC, 1, "BulbEnd", 0x9126},
{PTP_OC, 1, "GetDevicePropValue", 0x9127},
{PTP_OC, 1, "RemoteReleaseOn", 0x9128},
{PTP_OC, 1, "RemoteReleaseOff", 0x9129},
{PTP_OC, 1, "DriveLens", 0x9155},
{PTP_OC, 1, "InitiateViewfinder", 0x9151},
{PTP_OC, 1, "TerminateViewfinder", 0x9152},
{PTP_OC, 1, "GetViewFinderData", 0x9153},
{PTP_OC, 1, "DoAutoFocus", 0x9154},
{PTP_OC, 1, "AfCancel", 0x9160},
{PTP_OC, 1, "SetDefaultSetting", 0x91BE},
{PTP_ENUM, 0, "EOS_DESTINATION_CAM", 0x2},
{PTP_ENUM, 0, "EOS_DESTINATION_PC", 0x4},
{PTP_ENUM, 0, "EOS_DESTINATION_BOTH", 0x6},
{PTP_RC, 0, "Undefined", 0x2000},
{PTP_RC, 0, "OK", 0x2001},
{PTP_RC, 0, "GeneralError", 0x2002},
{PTP_RC, 0, "SessionNotOpen", 0x2003},
{PTP_RC, 0, "InvalidTransactionID", 0x2004},
{PTP_RC, 0, "OperationNotSupported", 0x2005},
{PTP_RC, 0, "ParameterNotSupported", 0x2006},
{PTP_RC, 0, "IncompleteTransfer", 0x2007},
{PTP_RC, 0, "InvalidStorageId", 0x2008},
{PTP_RC, 0, "InvalidObjectHandle", 0x2009},
{PTP_RC, 0, "DevicePropNotSupported", 0x200A},
{PTP_RC, 0, "InvalidObjectFormatCode", 0x200B},
{PTP_RC, 0, "StoreFull", 0x200C},
{PTP_RC, 0, "ObjectWriteProtected", 0x200D},
{PTP_RC, 0, "StoreReadOnly", 0x200E},
{PTP_RC, 0, "AccessDenied", 0x200F},
{PTP_RC, 0, "NoThumbnailPresent", 0x2010},
{PTP_RC, 0, "SelfTestFailed", 0x2011},
{PTP_RC, 0, "PartialDeletion", 0x2012},
{PTP_RC, 0, "StoreNotAvailable", 0x2013},
{PTP_RC, 0, "SpecByFormatUnsupported", 0x2014},
{PTP_RC, 0, "NoValidObjectInfo", 0x2015},
{PTP_RC, 0, "InvalidCodeFormat", 0x2016},
{PTP_RC, 0, "UnknownVendorCode", 0x2017},
{PTP_RC, 0, "CaptureAlreadyTerminated", 0x2018},
{PTP_RC, 0, "DeviceBusy", 0x2019},
{PTP_RC, 0, "InvalidParentObject", 0x201A},
{PTP_RC, 0, "InvalidDevicePropFormat", 0x201B},
{PTP_RC, 0, "InvalidDevicePropValue", 0x201C},
{PTP_RC, 0, "InvalidParameter", 0x201D},
{PTP_RC, 0, "SessionAlreadyOpened", 0x201E},
{PTP_RC, 0, "TransactionCanceled", 0x201F},
{PTP_RC, 0, "SpecOfDestinationUnsupported", 0x2020},
{PTP_RC, 2, "Unknown", 0xA001},
{PTP_RC, 2, "NotReady", 0xA102},
{PTP_RC, 2, "BatteryLow", 0xA101},
{PTP_RC, 0, "UndefinedMTP", 0xA800},
{PTP_RC, 0, "InvalidObjPropCode", 0xA801},
{PTP_RC, 0, "InvalidObjPropCodeFormat", 0xA802},
{PTP_RC, 0, "InvalidObjPropCodeValue", 0xA803},
{PTP_RC, 0, "InvalidObjReference", 0xA804},
{PTP_RC, 0, "InvalidDataset", 0xA806},
{PTP_RC, 0, "GroupSpecUnsupported", 0xA807},
{PTP_RC, 0, "DepthSpecUnsupported", 0xA808},
{PTP_RC, 0, "ObjectTooLarge", 0xA809},
{PTP_RC, 0, "ObjectPropUnsupported", 0xA80A},
{PTP_EC, 0, "Undefined", 0x4000},
{PTP_EC, 0, "CancelTransaction", 0x4001},
{PTP_EC, 0, "ObjectAdded", 0x4002},
{PTP_EC, 0, "ObjectRemoved", 0x4003},
{PTP_EC, 0, "StoreAdded", 0x4004},
{PTP_EC, 0, "StoreRemoved", 0x4005},
{PTP_EC, 0, "DevicePropChanged", 0x4006},
{PTP_EC, 0, "ObjectInfoChanged", 0x4007},
{PTP_EC, 0, "DeviceInfoChanged", 0x4008},
{PTP_EC, 0, "RequestObjectTransfer", 0x4009},
{PTP_EC, 0, "StoreFull", 0x400A},
{PTP_EC, 0, "DeviceReset", 0x400B},
{PTP_EC, 0, "StorageInfoChanged", 0x400C},
{PTP_EC, 0, "CaptureComplete", 0x400D},
{PTP_EC, 0, "UnreportedStatus", 0x400E},
{PTP_EC, 0, "Canon_RequestObjectTransfer", 0xC009},
{PTP_EC, 1, "RequestGetEvent", 0xC101},
{PTP_EC, 1, "ObjectAddedEx", 0xC181},
{PTP_EC, 1, "ObjectRemoved", 0xC182},
{PTP_EC, 1, "RequestGetObjectInfoEx", 0xC183},
{PTP_EC, 1, "StorageStatusChanged", 0xC184},
{PTP_EC, 1, "StorageInfoChanged", 0xC185},
{PTP_EC, 1, "RequestObjectTransfer", 0xc186},
{PTP_EC, 1, "ObjectInfoChangedEx", 0xC187},
{PTP_EC, 1, "ObjectContentChanged", 0xC188},
{PTP_EC, 1, "PropValueChanged", 0xC189},
{PTP_EC, 1, "AvailListChanged", 0xC18A},
{PTP_EC, 1, "CameraStatusChanged", 0xC18B},
{PTP_EC, 1, "WillSoonShutdown", 0xC18D},
{PTP_EC, 1, "ShutdownTimerUpdated", 0xC18E},
{PTP_EC, 1, "RequestCancelTransfer", 0xC18F},
{PTP_EC, 1, "RequestObjectTransferDT", 0xC190},
{PTP_EC, 1, "RequestCancelTransferDT", 0xC191},
{PTP_EC, 1, "StoreAdded", 0xC192},
{PTP_EC, 1, "StoreRemoved", 0xC193},
{PTP_EC, 1, "BulbExposureTime", 0xC194},
{PTP_EC, 1, "RecordingTime", 0xC195},
{PTP_EC, 1, "RequestObjectTransferTS", 0xC1A2},
{PTP_EC, 1, "AfResult", 0xC1A3},
{PTP_EC, 1, "InfoCheckComplete", 0xC1A4},
{PTP_EC, 0, "Nikon_ObjectAddedInSDRAM", 0xC101},
{PTP_EC, 0, "Nikon_CaptureCompleteRecInSdram", 0xC102},
{PTP_OF, 0, "Undefined", 0x3000},
{PTP_OF, 0, "Association", 0x3001},
{PTP_OF, 0, "Script", 0x3002},
{PTP_OF, 0, "Executable", 0x3003},
{PTP_OF, 0, "Text", 0x3004},
{PTP_OF, 0, "HTML", 0x3005},
{PTP_OF, 0, "DPOF", 0x3006},
{PTP_OF, 0, "AIFF", 0x3007},
{PTP_OF, 0, "WAV", 0x3008},
{PTP_OF, 0, "MP3", 0x3009},
{PTP_OF, 0, "AVI", 0x300A},
{PTP_OF, 0, "MPEG", 0x300B},
{PTP_OF, 0, "ASF", 0x300C},
{PTP_OF, 0, "MOV", 0x300D},
{PTP_OF, 0, "JPEG", 0x3801},
{PTP_OF, 0, "TIFF_EP", 0x3802},
{PTP_OF, 0, "FlashPix", 0x3803},
{PTP_OF, 0, "BMP", 0x3804},
{PTP_OF, 0, "CIFF", 0x3805},
{PTP_OF, 0, "Reserved2", 0x3806},
{PTP_OF, 0, "GIF", 0x3807},
{PTP_OF, 0, "JFIF", 0x3808},
{PTP_OF, 0, "PCD", 0x3809},
{PTP_OF, 0, "PICT", 0x380A},
{PTP_OF, 0, "PNG", 0x380B},
{PTP_OF, 0, "Reserved1", 0x380C},
{PTP_OF, 0, "TIFF", 0x380D},
{PTP_OF, 0, "TIFF_IT", 0x380E},
{PTP_OF, 0, "JP2", 0x380F},
{PTP_OF, 0, "JPX", 0x3810},
{PTP_OF, 0, "Firmware", 0xB802},
{PTP_OF, 0, "WIF", 0xB881},
{PTP_OF, 0, "Audio", 0xB900},
{PTP_OF, 0, "WMA", 0xb901},
{PTP_OF, 0, "OGG", 0xb902},
{PTP_OF, 0, "AAC", 0xb903},
{PTP_OF, 0, "Audible", 0xb904},
{PTP_OF, 0, "FLAC", 0xb906},
{PTP_OF, 0, "SamsungPlaylist", 0xb909},
{PTP_OF, 0, "Video", 0xb980},
{PTP_OF, 0, "WMV", 0xb981},
{PTP_OF, 0, "MP4", 0xb982},
{PTP_OF, 0, "3GP", 0xb984},
{PTP_OF, 0, "MP2", 0xb983},
{PTP_OF, 2, "CRW", 0xb101},
{PTP_OF, 0, "RAW", 0xb103},
{PTP_OF, 2, "MOV", 0xb104},
{PTP_ENUM, 0, "PTP_AT_Folder", 0x1},
{PTP_ENUM, 0, "PTP_AT_Album", 0x1},
{PTP_PC, 0, "BatteryLevel", 0x5001},
{PTP_PC, 0, "FunctionalMode", 0x5002},
{PTP_PC, 0, "FocalLength", 0x5008},
{PTP_PC, 0, "FocalDistance", 0x5009},
{PTP_PC, 0, "FocusMode", 0x500A},
{PTP_PC, 0, "DateTime", 0x5011},
{PTP_PC, 2, "BeepCode", 0xD001},
{PTP_PC, 2, "ViewFinderMode", 0xD003},
{PTP_PC, 2, "ImageQuality", 0xD006},
{PTP_PC, 2, "ImageSize", 0xD008},
{PTP_PC, 2, "FlashMode", 0xD00a},
{PTP_PC, 2, "TvAvSetting", 0xD00c},
{PTP_PC, 2, "MeteringMode", 0xd010},
{PTP_PC, 2, "MacroMode", 0xd011},
{PTP_PC, 2, "FocusingPoint", 0xd012},
{PTP_PC, 2, "WhiteBalance", 0xd013},
{PTP_PC, 2, "AFMode", 0xD015},
{PTP_PC, 2, "Contrast", 0xD017},
{PTP_PC, 2, "ISOSpeed", 0xd01c},
{PTP_PC, 2, "Aperture", 0xd01c},
{PTP_PC, 2, "ShutterSpeed", 0xd01e},
{PTP_PC, 2, "ExpComp", 0xd01f},
{PTP_PC, 2, "Zoom", 0xd02a},
{PTP_PC, 2, "SizeQuality", 0xd02c},
{PTP_PC, 2, "FlashMemory", 0xd031},
{PTP_PC, 2, "CameraModel", 0xd032},
{PTP_PC, 2, "CameraOwner", 0xd033},
{PTP_PC, 2, "UnixTime", 0xd032},
{PTP_PC, 2, "ViewFinderOut", 0xD036},
{PTP_PC, 2, "RealImageWidth", 0xD039},
{PTP_PC, 2, "PhotoEffect", 0xD040},
{PTP_PC, 2, "AssistLight", 0xD041},
{PTP_PC, 1, "Aperture", 0xD101},
{PTP_PC, 1, "ShutterSpeed", 0xD102},
{PTP_PC, 1, "ISOSpeed", 0xD103},
{PTP_PC, 1, "ExpCompensation", 0xD104},
{PTP_PC, 1, "AutoExposureMode", 0xD105},
{PTP_PC, 1, "DriveMode", 0xD106},
{PTP_PC, 1, "MeteringMode", 0xD107},
{PTP_PC, 1, "FocusMode", 0xD108},
{PTP_PC, 1, "WhiteBalance", 0xD109},
{PTP_PC, 1, "ColorTemperature", 0xD10A},
{PTP_PC, 1, "WhiteBalanceAdjustA", 0xD10B},
{PTP_PC, 1, "WhiteBalanceAdjustB", 0xD10C},
{PTP_PC, 1, "WhiteBalanceXA", 0xD10D},
{PTP_PC, 1, "WhiteBalanceXB", 0xD10E},
{PTP_PC, 1, "ColorSpace", 0xD10F},
{PTP_PC, 1, "PictureStyle", 0xD110},
{PTP_PC, 1, "BatteryPower", 0xD111},
{PTP_PC, 1, "BatterySelect", 0xD112},
{PTP_PC, 1, "CameraTime", 0xD113},
{PTP_PC, 1, "Owner", 0xD115},
{PTP_PC, 1, "ModelID", 0xD116},
{PTP_PC, 1, "PTPExtensionVersion", 0xD119},
{PTP_PC, 1, "DPOFVersion", 0xD11A},
{PTP_PC, 1, "AvailableShots", 0xD11B},
{PTP_PC, 1, "CaptureDestination", 0xD11C},
{PTP_PC, 1, "CurrentFolder", 0xD11F},
{PTP_PC, 1, "ImageFormat", 0xD120},
{PTP_PC, 1, "ImageFormatCF", 0xD121},
{PTP_PC, 1, "ImageFormatSD", 0xD122},
{PTP_PC, 1, "ImageFormatExtHD", 0xD123},
{PTP_PC, 1, "AEModeDial", 0xD138},
{PTP_PC, 1, "VF_Output", 0xD1B0},
{PTP_PC, 1, "EVFMode", 0xD1B1},
{PTP_PC, 1, "DOFPreview", 0xD1B2},
{PTP_PC, 1, "VFSharp", 0xD1B3},
{PTP_PC, 1, "EVFWBMode", 0xD1B4},
{PTP_PC, 1, "FocusInfoEx", 0xD1D3},
{PTP_ST, 0, "Undefined", 0x0},
{PTP_ST, 0, "FixedROM", 0x1},
{PTP_ST, 0, "RemovableROM", 0x2},
{PTP_ST, 0, "FixedRAM", 0x3},
{PTP_ST, 0, "RemovableRAM", 0x4},
{PTP_FT, 0, "Undefined", 0x0},
{PTP_FT, 0, "GenericFlat", 0x1},
{PTP_FT, 0, "GenericHei", 0x2},
{PTP_FT, 0, "DCF", 0x3},
{PTP_AC, 0, "ReadWrite", 0x0},
{PTP_AC, 0, "Read", 0x1},
{PTP_AC, 0, "ReadDelete", 0x2},
{PTP_ENUM, 0, "PTP_TC_UNDEF", 0x0},
{PTP_ENUM, 0, "PTP_TC_INT8", 0x1},
{PTP_ENUM, 0, "PTP_TC_UINT8", 0x2},
{PTP_ENUM, 0, "PTP_TC_INT16", 0x3},
{PTP_ENUM, 0, "PTP_TC_UINT16", 0x4},
{PTP_ENUM, 0, "PTP_TC_INT32", 0x5},
{PTP_ENUM, 0, "PTP_TC_UINT32", 0x6},
{PTP_ENUM, 0, "PTP_TC_INT64", 0x7},
{PTP_ENUM, 0, "PTP_TC_UINT64", 0x8},
{PTP_ENUM, 0, "PTP_TC_INT128", 0x9},
{PTP_ENUM, 0, "PTP_TC_UINT128", 0xA},
{PTP_ENUM, 0, "PTP_TC_UINT8ARRAY", 0x4002},
{PTP_ENUM, 0, "PTP_TC_UINT16ARRAY", 0x4004},
{PTP_ENUM, 0, "PTP_TC_UINT32ARRAY", 0x4006},
{PTP_ENUM, 0, "PTP_TC_UINT64ARRAY", 0x4008},
{PTP_ENUM, 0, "PTP_TC_STRING", 0xFFFF},
{PTP_ENUM, 0, "PTPIP_INIT_COMMAND_REQ", 0x1},
{PTP_ENUM, 0, "PTPIP_INIT_COMMAND_ACK", 0x2},
{PTP_ENUM, 0, "PTPIP_INIT_EVENT_REQ", 0x3},
{PTP_ENUM, 0, "PTPIP_INIT_EVENT_ACK", 0x4},
{PTP_ENUM, 0, "PTPIP_INIT_FAIL", 0x5},
{PTP_ENUM, 0, "PTPIP_COMMAND_REQUEST", 0x6},
{PTP_ENUM, 0, "PTPIP_COMMAND_RESPONSE", 0x7},
{PTP_ENUM, 0, "PTPIP_EVENT", 0x8},
{PTP_ENUM, 0, "PTPIP_DATA_PACKET_START", 0x9},
{PTP_ENUM, 0, "PTPIP_DATA_PACKET", 0xA},
{PTP_ENUM, 0, "PTPIP_CANCEL_TRANSACTION", 0xB},
{PTP_ENUM, 0, "PTPIP_DATA_PACKET_END", 0xC},
{PTP_ENUM, 0, "PTPIP_PING", 0xD},
{PTP_ENUM, 0, "PTPIP_PONG", 0xE},
{PTP_ENUM, 0, "USB_REQ_RESET", 0x66},
{PTP_ENUM, 0, "USB_REQ_STATUS", 0x67},
{PTP_ENUM, 0, "USB_REQ_GET_STATUS", 0x00},
{PTP_ENUM, 0, "USB_REQ_CLEAR_FEATURE", 0x01},
{PTP_ENUM, 0, "USB_REQ_SET_FEATURE", 0x03},
{PTP_ENUM, 0, "USB_DP_HTD", 0x0},
{PTP_ENUM, 0, "USB_DP_DTH", 0x80},
{PTP_ENUM, 0, "USB_RECIP_DEVICE", 0x00},
{PTP_ENUM, 0, "USB_RECIP_INTERFACE", 0x01},
{PTP_ENUM, 0, "USB_RECIP_ENDPOINT", 0x02},
{PTP_ENUM, 0, "USB_TYPE_CLASS", 0x20},
{PTP_ENUM, 0, "FUJI_PROTOCOL_VERSION", 0x8f53e4f2},
{PTP_OC, 5, "SendObjectInfo", 0x900c},
{PTP_OC, 5, "SendObject2", 0x900d},
{PTP_OC, 5, "SendObject", 0x901d},
{PTP_PC, 0, "Fuji_EventsList", 0xd212},
{PTP_PC, 5, "Unknown3", 0xd220},
{PTP_PC, 5, "Unknown2", 0xd222},
{PTP_PC, 5, "Compression", 0xD226},
{PTP_PC, 5, "CompressionInfo", 0xD227},
{PTP_PC, 5, "PartialSize", 0xD235},
{PTP_PC, 5, "CardStatus", 0xd244},
{PTP_PC, 5, "Unknown3", 0xdc04},
{PTP_PC, 5, "DriveMode", 0xd246},
{PTP_PC, 0, "Fuji_TransferMode", 0xdf22},
{PTP_PC, 0, "Fuji_Unlocked", 0xdf00},
{PTP_PC, 0, "Fuji_Mode", 0xdf01},

};int ptp_enums_length = 355;
