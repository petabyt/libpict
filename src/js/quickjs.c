#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <quickjs/quickjs.h>
#include <quickjs/quickjs-libc.h>
#include <libpict.h>

static JSClassID class_id = 0;

static JSValue connect(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst argv[]) {
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry ptp_methods[] = {
	JS_CFUNC_DEF("connect", 1, connect),

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
    .class_name = "LibPict",
    .finalizer = js_directory_finalizer,
};

JSValue ptp_get_class(JSContext* ctx) {
	JS_NewClassID(&class_id);
	JS_NewClass(JS_GetRuntime(ctx), class_id, &ptp_class);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, ptp_methods, sizeof(ptp_methods) / sizeof(ptp_methods[0]));
	JS_SetClassProto(ctx, class_id, proto);

	JSValue class = JS_NewCFunction2(ctx, js_directory_constructor, "LibPict", 5, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, class, proto);

	return class;
}

int ptp_module_libpict(JSContext* ctx, JSModuleDef *m) {
  JS_SetModuleExport(ctx, m, "LibPict", ptp_get_class(ctx));
  return 0;
}

int ptp_run_quickjs(const char *filename) {
	JSRuntime *rt = JS_NewRuntime();

	JS_SetMemoryLimit(rt, 80 * 1024);
	JS_SetMaxStackSize(rt, 10 * 1024);

	JSContext *ctx = JS_NewContext(rt);
	js_std_add_helpers(ctx, 0, NULL);

    JSModuleDef *m = JS_NewCModule(ctx, "libpict", ptp_module_libpict);
    if (!m) return -1;
    JS_AddModuleExport(ctx, m, "LibPict");

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
