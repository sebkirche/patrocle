/* 
   Interface between the bot and lua
   
   We provide some interface funcs that are accessible to the lua part.
*/

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#ifndef __APPLE__
	#include <lua5.1/lua.h>
	#include <lua5.1/lualib.h>
	#include <lua5.1/lauxlib.h>
#else
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
#endif

#include "log.h"
#include "cfgfile.h"
#include "debug.h"
#include "misc.h"
#include "parsing.h"
#include "vlad-ons.h"
#include "send.h"
#include "vladbot.h"
#include "phrase.h"

lua_State *L = NULL;

int c2l_ischannel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushboolean(L, ischannel(name));
	return 1;
}

int c2l_islogon(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushboolean(L, is_log_on(name));
	return 1;
}

int c2l_nickuserstr(lua_State *L)
{
	char *NUS;
	const char *name = luaL_checkstring(L, 1);
	NUS = NickUserStr(name);
	lua_pushstring(L, NUS);
	if(NUS)
		free(NUS);
	return 1;
}

int c2l_userlevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, userlevel(name));
	return 1;
}

int c2l_protlevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, protlevel(name));
	return 1;
}

int c2l_shitlevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, shitlevel(name));
	return 1;
}

int c2l_rellevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, rellevel(name));
	return 1;
}

int c2l_chaineestdans(lua_State *L)
{
	const char *afouiller = luaL_checkstring(L, 1);
	const char *achercher = luaL_checkstring(L, 2);
	lua_pushboolean(L, ChaineEstDans(afouiller, achercher));
	return 1;
}

int c2l_sendtouser(lua_State *L)
{
	const char *to = luaL_checkstring(L, 1);
	const char *fmt = luaL_checkstring(L, 2);
	lua_pushboolean(L, send_to_user(to, fmt));
	return 1;
}

int c2l_sendnotice(lua_State *L)
{
	const char *to = luaL_checkstring(L, 1);
	const char *fmt = luaL_checkstring(L, 2);
	lua_pushboolean(L, sendnotice(to, fmt));
	return 1;
}

int c2l_repondre(lua_State *L)
{
	int i;
	char **Rpos, **Rneg;
	const char *from = luaL_checkstring(L, 1);
	const char *to = luaL_checkstring(L, 2);
	int humpos = luaL_checknumber(L, 3);
	int nbpos = lua_objlen(L, 4);//luaL_checknumber(L, 4);
	int humneg = luaL_checknumber(L, 5);
	int nbneg = lua_objlen(L, 6);//luaL_checknumber(L, 7);
	if(!lua_istable(L, 4))
		return 0;
	if(!lua_istable(L, 6))
		return 0;
	Rpos = malloc(nbpos * sizeof(char *));
	for(i=0; i<nbpos; i++){
		lua_rawgeti(L, 4, i+1);
		Rpos[i] = strdup(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	Rneg = malloc(nbneg * sizeof(char *));
	for(i=0; i<nbneg; i++){
		lua_rawgeti(L, 6, i+1);
		Rneg[i] = strdup(lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	Repondre(from, to, humpos, nbpos, Rpos, humneg, nbneg, Rneg);
	return 0;
}

int c2l_botlog(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	const char *fmt, *arg1, *arg2, *arg3, *arg4;
	int nbargs = lua_gettop(L);

	if(nbargs > 1)
		fmt = luaL_checkstring(L, 2);

	switch(nbargs){
		case 2:
			botlog(filename, fmt);
			break;
		case 3:
			arg1 = luaL_checkstring(L, 3);
			botlog(filename, fmt, arg1);
			break;
		case 4:
			arg1 = luaL_checkstring(L, 3);
			arg2 = luaL_checkstring(L, 4);
			botlog(filename, fmt, arg1, arg2);
			break;
		case 5:
			arg1 = luaL_checkstring(L, 3);
			arg2 = luaL_checkstring(L, 4);
			arg3 = luaL_checkstring(L, 5);
			botlog(filename, fmt, arg1, arg2, arg3);
			break;
		case 6:
			arg1 = luaL_checkstring(L, 3);
			arg2 = luaL_checkstring(L, 4);
			arg3 = luaL_checkstring(L, 5);
			arg4 = luaL_checkstring(L, 6);
			botlog(filename, fmt, arg1, arg2, arg3, arg4);
			break;
	}
	return 0;
}

void register_cstuff()
{
	//strings
	lua_pushstring(L, LOGFILE);
	lua_setglobal(L, "LOGFILE");
	lua_pushstring(L, ERRFILE);
	lua_setglobal(L, "ERRFILE");

	//interface functions
	lua_register(L, "is_channel", c2l_ischannel);
	lua_register(L, "is_log_on", c2l_islogon);
	lua_register(L, "nick_user_str", c2l_nickuserstr);
	lua_register(L, "userlever", c2l_userlevel);
	lua_register(L, "shitlevel", c2l_shitlevel);
	lua_register(L, "protlevel", c2l_protlevel);
	lua_register(L, "rellevel", c2l_rellevel);
	lua_register(L, "chaine_est_dans", c2l_chaineestdans);
	lua_register(L, "send_to_user", c2l_sendtouser);
	lua_register(L, "send_notice", c2l_sendnotice);
	lua_register(L, "repondre", c2l_repondre);
	lua_register(L, "botlog", c2l_botlog);
}

int init_lua()
{
	L = luaL_newstate();
	if(!L)
		return 1;
	luaL_openlibs(L);
	//register_cstuff();
	return 0;
}

void shutdown_lua()
{
	if(L)
		lua_close(L);
	L = NULL;
}

void load_lualogic(char **return_msg)
{
	register_cstuff();
	if(luaL_loadfile(L,"reactions.lua") || lua_pcall(L, 0,0,0)){
#ifdef DBUG
		printf("Error while loading reactions.lua file : %s\n", lua_tostring(L, -1));
#endif
		botlog(ERRFILE, "Error while loading reactions.lua file");
		botlog(ERRFILE, lua_tostring(L, -1));
		if(return_msg)
			mstrcpy(return_msg, lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

void    LuaTraite (botinfo	*currentbot, char *from, char *to, char *msg, int numphrase)
{
	/* do not pass a complex botinfo struct, just expose the bot nick */
	lua_pushstring(L, "BotNick");
	lua_pushstring(L, currentbot->nick);
	lua_settable(L, LUA_GLOBALSINDEX);

	lua_getglobal(L, "TraiteMessage");
	lua_pushstring(L, from);
	lua_pushstring(L, to);
	lua_pushstring(L, msg);
	lua_pushnumber(L, numphrase);
	if(lua_pcall(L, 4, 0, 0)){
#ifdef DBUG
                printf("Error while executing lua TraiteMessage : %s\n", lua_tostring(L, -1));
#endif
		botlog(ERRFILE, "Error while executing lua TraiteMessage");
                botlog(ERRFILE, lua_tostring(L, -1));
                lua_pop(L, 1);
        }
}