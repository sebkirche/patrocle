#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#ifndef __APPLE__
#include <lua5.1/lua.h>
#else
#include <lua.h>
#endif
#include "vladbot.h"

int init_lua();
void shutdown_lua();
void register_cstuff();
void load_lualogic(char **ret_msg);
void    LuaTraite (botinfo	*currentbot, char *from, char *to, char *msg, int numphrase);

//extern Lua_State *L;
#endif
