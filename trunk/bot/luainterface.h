/*
 luainterface.h - Interface between the bot and lua
 Copyright (C) 2009 Sébastien Kirche 
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 We provide some interface funcs that are accessible to the lua part.
 */

#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#ifdef __linux__
#include <lua5.1/lua.h>
#else //__APPLE__, WIN32
#include <lua.h>
#endif
#include "vladbot.h"

int init_lua();
void shutdown_lua();
void register_cstuff();
void load_lualogic(char **ret_msg);
void    LuaTraite(botinfo	*currentbot, char *from, char *to, char *msg, int numphrase);

//extern Lua_State *L;
#endif

// Local variables:
// coding: utf-8
// end:
