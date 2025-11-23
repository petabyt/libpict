#ifndef LIBPICT_LUA
#define LIBPICT_LUA

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <libpict.h>

LUALIB_API int luaopen_ptp(lua_State *L);
LUALIB_API int luaopen_cjson(lua_State *l);

void ptp_init_lua_object(lua_State *L, struct PtpRuntime *r);

struct PtpRuntime *luaptp_get_runtime(lua_State *L);

#endif
