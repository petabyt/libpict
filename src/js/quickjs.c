#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <quickjs/quickjs.h>
#include <quickjs/quickjs-libc.h>
#include <libpict.h>

enum GenericOperation {
	OPEN_SESSION,
	CLOSE_SESSION,
	GET_DEVICE_INFO,
	GET_PROPERTY,
	SET_RPOPERTY,
};

static JSClassID class_id = 0;
const static char class_name[] = "LibPict";

static JSValue throw_rc_err(JSContext *ctx, int rc) {
	switch (rc) {
	case 0: JS_UNDEFINED;
	default: JS_ThrowInternalError(ctx, "%s", ptp_perror(rc)); break;
	}
	return JS_EXCEPTION;
}

static JSValue connect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst argv[]) {
	struct PtpRuntime *r = JS_GetOpaque(this_val, class_id);
	int rc = ptp_device_init(r);
	return JS_NewInt32(ctx, rc);
}

static JSValue disconnect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst argv[]) {
	struct PtpRuntime *r = JS_GetOpaque(this_val, class_id);
	int rc = ptp_device_close(r);
	return JS_NewInt32(ctx, rc);
}

static JSValue generic_operation(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst argv[], int magic) {
	struct PtpRuntime *r = JS_GetOpaque(this_val, class_id);

	struct PtpDeviceInfo di;
	char buffer[4096];
	int rc = 0;
	switch (magic) {
	case OPEN_SESSION:
		rc = ptp_open_session(r);
		break;
	case CLOSE_SESSION:
		rc = ptp_close_session(r);
		break;
	case GET_DEVICE_INFO:
		rc = ptp_get_device_info(r, &di);
		if (rc) break;
		ptp_device_info_json(&di, buffer, sizeof(buffer));
		return JS_ParseJSON(ctx, buffer, strlen(buffer), "");		
	}

	return throw_rc_err(ctx, rc);
}

static const JSCFunctionListEntry ptp_methods[] = {
	JS_CFUNC_DEF("connect", 1, connect),
	JS_CFUNC_DEF("disconnect", 0, disconnect),
	JS_CFUNC_MAGIC_DEF("openSession", 0, generic_operation, OPEN_SESSION),
	JS_CFUNC_MAGIC_DEF("closeSession", 0, generic_operation, CLOSE_SESSION),
	JS_CFUNC_MAGIC_DEF("getDeviceInfo", 0, generic_operation, GET_DEVICE_INFO),

	JS_PROP_INT32_DEF("OK", 0, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("NO_DEVICE", -1, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("NO_PERM", -2, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("OPEN_FAIL", -3, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("OUT_OF_MEM", -4, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("IO_ERR", -5, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("RUNTIME_ERR", -6, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("UNSUPPORTED", -7, JS_PROP_ENUMERABLE),
	JS_PROP_INT32_DEF("CHECK_CODE", -8, JS_PROP_ENUMERABLE),
};

static JSValue js_directory_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst argv[]) {
	JSValue proto = JS_GetPropertyStr(ctx, new_target, "prototype");
	JSValue obj = JS_NewObjectProtoClass(ctx, proto, class_id);
	JS_FreeValue(ctx, proto);

	struct PtpRuntime *r = ptp_new(PTP_USB);

	JS_SetOpaque(obj, r);
	
	return obj;
}

static void js_directory_finalizer(JSRuntime *rt, JSValue val) {
	struct PtpRuntime *r = (struct PtpRuntime *)JS_GetOpaque(val, class_id);
	ptp_close(r);
}

static JSClassDef ptp_class = {
    .class_name = class_name,
    .finalizer = js_directory_finalizer,
};

static JSValue ptp_get_class(JSContext* ctx) {
	JS_NewClassID(&class_id);
	JS_NewClass(JS_GetRuntime(ctx), class_id, &ptp_class);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, ptp_methods, sizeof(ptp_methods) / sizeof(ptp_methods[0]));
	JS_SetClassProto(ctx, class_id, proto);

	JSValue class = JS_NewCFunction2(ctx, js_directory_constructor, class_name, 5, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, class, proto);

	return class;
}

static int ptp_module_libpict(JSContext* ctx, JSModuleDef *m) {
  JS_SetModuleExport(ctx, m, class_name, ptp_get_class(ctx));
  return 0;
}

JSModuleDef *js_init_module_libpict(JSContext *ctx, const char *module_name) {
	JSModuleDef *m = JS_NewCModule(ctx, module_name, ptp_module_libpict);
	if (!m) return NULL;
	return m;
}

int ptp_run_quickjs(const char *filename) {
	JSRuntime *rt = JS_NewRuntime();

	JSContext *ctx = JS_NewContext(rt);
	js_std_add_helpers(ctx, 0, NULL);

    JS_AddModuleExport(ctx, js_init_module_libpict(ctx, "libpict"), class_name);
	JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
	js_std_init_handlers(rt);

    FILE *file = fopen(filename, "rb");
    if (!file) return -1;
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) return -1;
    
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);

	JSValue val = JS_Eval(ctx, buffer, file_size, filename, JS_EVAL_TYPE_MODULE);
	if (JS_IsException(val)) {
		js_std_dump_error(ctx);
//		const char *str = JS_ToCString(ctx, val);
//		printf("JS error: %s\n", str);
//		JS_FreeCString(ctx, str);
		return -1;
	}

	JS_FreeValue(ctx, val);

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

	return 0;
}
