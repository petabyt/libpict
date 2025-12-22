// Lua interface extension, not meant to be compiled with the library.
// The program linking in this file must provide luaptp_get_runtime()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <libpict.h>
#include <libpict_lua.h>

struct PtpLuaContext {
	struct PtpRuntime *r;
};

struct PtpRuntime *luaptp_get_runtime(lua_State *L) {
	struct PtpLuaContext *p = luaL_checkudata(L, 1, "PtpMetaTable");
	return p->r;
}

// lua-cjson
void lua_json_decode(lua_State *l, const char *json_text, int json_len);
void json_create_config(lua_State *l);

static int mylua_set_property(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);

	const char *name = luaL_checkstring(L, 1);
	int value = lua_tointeger(L, 1);

	int rc = ptp_set_generic_property(r, name, value);

	lua_pushinteger(L, rc);

	return 1;
}

static int mylua_open_session(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);
	lua_pushinteger(L, ptp_open_session(r));
	return 1;
}

static int mylua_close_session(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);
	lua_pushinteger(L, ptp_close_session(r));
	return 1;
}

static int mylua_disconnect(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);
	lua_pushinteger(L, ptp_device_close(r));
	return 1;
}

static int mylua_device_info(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);

	struct PtpDeviceInfo di;
	int rc = ptp_get_device_info(r, &di);
	if (rc) {
		lua_pushnil(L);
		lua_pushinteger(L, rc);
		return 1;
	}

	char buffer[4096];
	ptp_device_info_json(&di, buffer, sizeof(buffer));

	lua_json_decode(L, buffer, strlen(buffer));
	lua_pushinteger(L, 0);

	return 2;
}

static int mylua_take_picture(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);

	if (r->di == NULL) return 1;

	int rc = ptp_pre_take_picture(r);
	if (rc) goto err;

	rc = ptp_take_picture(r);
	if (rc) goto err;

	err:;
	lua_pushinteger(L, rc);

	return 1;
}

static int mylua_send_operation(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);

	int opcode = lua_tointeger(L, 2);
	int len = lua_gettop(L);

	if (!lua_istable(L, 3)) {
		return luaL_error(L, "arg2 expected array");
	}

	struct PtpCommand cmd;
	cmd.code = opcode;

	// Read parameters
	int param_length = 0;
	if (len >= 3) {
		param_length = luaL_len(L, 3);
		for (int i = 1; i <= param_length; ++i) {
			lua_rawgeti(L, 3, i);
			cmd.params[i - 1] = luaL_checkinteger(L, -1);
			lua_pop(L, 1);
		}
		cmd.param_length = param_length;
	}

	// Read payload if provided
	int data_length = 0;
	uint8_t *data_array = NULL;
	if (len >= 4) {
		data_length = luaL_len(L, 4);
		data_array = malloc(data_length * sizeof(int));
		for (int i = 1; i <= data_length; ++i) {
			lua_rawgeti(L, 4, i);
			data_array[i - 1] = (uint8_t)luaL_checkinteger(L, -1);
			lua_pop(L, 1);
		}
	}

	int rc = 0;
	if (data_array == NULL) {
		rc = ptp_send(r, &cmd);
	} else {
		rc = ptp_send_data(r, &cmd, data_array, data_length);
	}

    lua_newtable(L);

	lua_pushstring(L, "error");
	lua_pushinteger(L, rc);
	lua_settable(L, -3);

    lua_pushstring(L, "code");
    lua_pushinteger(L, ptp_get_return_code(r));
    lua_settable(L, -3);

	lua_pushstring(L, "payload");
	lua_newtable(L);
	for (int i = 0; i < ptp_get_payload_length(r); ++i) {
		lua_pushinteger(L, i + 1);
		lua_pushinteger(L, (int)(r->data[i]));
		lua_settable(L, -3);
	}

	lua_settable(L, -3);

    lua_pushstring(L, "id");
    lua_pushinteger(L, ptp_get_last_transaction_id(r));
    lua_settable(L, -3);

    lua_pushinteger(L, rc);

	return 2;
}

static void add_func(lua_State *L, const char *name, int (*func)(lua_State *L)) {
	lua_pushcfunction(L, func);
	lua_setfield(L, -2, name);
}

void ptp_init_lua_object(lua_State *L, struct PtpRuntime *r) {
	if (luaL_newmetatable(L, "PtpMetaTable")) {
		lua_newtable(L);
		add_func(L, "openSession", mylua_open_session);
		add_func(L, "closeSession", mylua_close_session);
		add_func(L, "disconnect", mylua_disconnect);
		add_func(L, "getDeviceInfo", mylua_device_info);
		add_func(L, "takePicture", mylua_take_picture);
		add_func(L, "setProperty", mylua_set_property);
		add_func(L, "sendOperation", mylua_send_operation);
		lua_setfield(L, -2, "__index");
		lua_pop(L, 1);
	}

	struct PtpLuaContext *p = lua_newuserdata(L, sizeof(struct PtpLuaContext));
	p->r = r;
	luaL_getmetatable(L, "PtpMetaTable");
	lua_setmetatable(L, -2);
}

static int mylua_test(lua_State *L) {
	struct PtpRuntime *r = luaptp_get_runtime(L);

	char *t = "{\"hello\": 123}";

	//json_create_config(L);
	lua_json_decode(L, t, strlen(t));

	return 1;
}

static int mylua_connect(lua_State *L) {
	struct PtpRuntime *r = ptp_new(PTP_USB);

	int rc = ptp_device_connect(r);
	if (rc) {
		ptp_close(r);
		lua_pushnil(L);
		lua_pushinteger(L, rc);
		return 1;
	}

	ptp_init_lua_object(L, r);
	lua_pushinteger(L, 0);

	return 2;
}

static const luaL_Reg ptplib[] = {
	{"test",			mylua_test},
	{"connect",			mylua_connect},
	{NULL, NULL}
};


static void new_const(lua_State *L, const char *name, int val) {
	lua_pushstring(L, name);
	lua_pushnumber(L, val);
	lua_settable(L, -3);
}

LUALIB_API int luaopen_ptp(lua_State *L) {
	luaL_newlib(L, ptplib);

	new_const(L, "OK", 0);
	new_const(L, "NO_DEVICE", -1);
	new_const(L, "NO_PERM", -2);
	new_const(L, "OPEN_FAIL", -3);
	new_const(L, "OUT_OF_MEM", -4);
	new_const(L, "IO_ERR", -5);
	new_const(L, "RUNTIME_ERR", -6);
	new_const(L, "UNSUPPORTED", -7);
	new_const(L, "CHECK_CODE", -8);

	return 1;
}

int ptp_run_lua(const char *filename) {
	lua_State *L = luaL_newstate();
	luaopen_base(L);
	luaL_requiref(L, "json", luaopen_cjson, 1);
	luaL_requiref(L, "ptp", luaopen_ptp, 1);
	luaL_requiref(L, "string", luaopen_string, 1);
	if (luaL_dofile(L, filename)) {
		printf("%s\n", lua_tostring(L, -1));
	}
	lua_close(L);
	return 0;
}
