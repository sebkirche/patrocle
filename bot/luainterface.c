/*
 luainterface.c - Interface between the bot and lua
 Copyright (C) 2009, 2010 Sébastien Kirche 
 
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

#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
//#ifndef __APPLE__
//	#include <lua5.1/lua.h>
//	#include <lua5.1/lualib.h>
//	#include <lua5.1/lauxlib.h>
//#else
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
//#endif
#include <sys/types.h>
#include <time.h>

#include "log.h"
#include "cfgfile.h"
#include "debug.h"
#include "misc.h"
#include "parsing.h"
#include "vlad-ons.h"
#include "send.h"
#include "vladbot.h"
#include "phrase.h"

extern int loglevel;
extern short logging;
extern	botinfo	*currentbot;
extern char *botmaintainer;
lua_State *L = NULL;

//lua wrapper for C ischannel()
int c2l_ischannel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushboolean(L, ischannel(name));
	return 1;
}

//lua getter to check the talking status of the given channel name
int c2l_cantalk(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	CHAN_list *Channel_to  = (ischannel(name) ? search_chan(name) : 0);
	lua_pushboolean(L, (Channel_to ? Channel_to->talk : TRUE));
	return 1;
}

//lua setter to change the talking status of the given channel 
int c2l_settalk(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	int talk;
	if lua_isboolean(L, 2){
		talk = lua_toboolean(L, 2);
		lua_pop(L, 1);
		CHAN_list *Channel_to  = (ischannel(name) ? search_chan(name) : 0);
#ifdef DBUG
		printf("c2l_settalk() on <%s> (we received '%s') to %d\n", Channel_to?Channel_to->name:"chan inconnu", name, talk);
#endif
		if(Channel_to)
			Channel_to->talk = talk;
	}
	return 0;
}

//lua wrapper for the C is_log_on()
int c2l_islogon(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushboolean(L, is_log_on(name));
	return 1;
}

//lua wrapper for the C NickUserStr()
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

//lua wrapper for the C GetNick()
int c2l_getnick(lua_State *L)
{
	const char *from = luaL_checkstring(L, 1);
	lua_pushstring(L, GetNick(from));
	return 1;
}

//lua wrapper to get a locutor via LocuteurExiste() 
//returns a light user data = simple pointer
int c2l_locutorexists(lua_State *L)
{
	locuteur *Locuteur;
	const char *name = luaL_checkstring(L, 1);
	Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, name);
	//lua_pushboolean(L, Locuteur != NULL);
	lua_pushlightuserdata(L, (void *) Locuteur);
	return 1;
}

//lua wrapper to AjouteLocuteur()
int c2l_addlocutor(lua_State *L)
{
	locuteur *Locuteur;
	const char *name = luaL_checkstring(L, 1);
	Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, name);
	if(!Locuteur)
		Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs, name);
	lua_pushlightuserdata(L, (void *)Locuteur);
	return 1;
}

//TODO: remplacer le light user data pour passer le pointeur Locuteur par un userdata avec metatables

//lua getter to the Locuteur->Bonjours counter
int c2l_getlocutorsalutes(lua_State *L)
{
	locuteur *Locuteur;
	if(lua_isuserdata(L, 1)){
		Locuteur = (locuteur*)lua_touserdata(L, 1);
		lua_pop(L, 1);
#ifdef DBUG
		printf("c2l_getlocutorsalutes called on Locuteur %s\n", Locuteur?Locuteur->nuh:"<null>");
#endif
		lua_pushnumber(L, Locuteur?Locuteur->Bonjours:-1);
	}
	else
		lua_pushnumber(L, -1);
	return 1;
}

//lua setter fir the Locuteur->Bonjour counter
int c2l_setlocutorsalutes(lua_State *L)
{
	locuteur *Locuteur;
	int newbjr = luaL_checknumber(L, 2);
	if(lua_isuserdata(L, 1)){
		Locuteur = (locuteur*)lua_touserdata(L, 1);
		lua_pop(L, 1);
		if(Locuteur)
			Locuteur->Bonjours = newbjr;
	}
	return 0;
}

//lua getter for the Locuteur->DernierContact field
int c2l_getlocutorlastcontact(lua_State *L)
{
	locuteur *Locuteur;
	if(lua_isuserdata(L, 1)){
		Locuteur = (locuteur*)lua_touserdata(L, 1);
		lua_pop(L, 1);
		lua_pushnumber(L, Locuteur?Locuteur->DernierContact:-1);
	}
	else
		lua_pushnumber(L, 0);
	return 1;
}

//lua wrapper to the C userlevel()
int c2l_getuserlevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, userlevel(name));
	return 1;
}

//lua wrapper to the C protlevel()
int c2l_getprotlevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, protlevel(name));
	return 1;
}

//lua wrapper to the C shitlevel()
int c2l_getshitlevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, shitlevel(name));
	return 1;
}

//lua wrapper to the C rellevel()
int c2l_getrellevel(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	lua_pushnumber(L, rellevel(name));
	return 1;
}

//lua wrapper to the C exist_userhost()
int c2l_existuserhost(lua_State *L)
{
	const char *from = luaL_checkstring(L, 1);
	lua_pushboolean(L, exist_userhost(currentbot->lists->rellist, from) != NULL);
	return 1;
}

//lua wrapper to the C add_to_levellist() with a level of 0
int c2l_addtolevellist(lua_State *L)
{
	const char *from = luaL_checkstring(L, 1);
	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0);
	return 0;
}

//lua wrapper to the C add_to_levellist() with the given level
int c2l_addtolevel(lua_State *L)
{
	const char *from = luaL_checkstring(L, 1);
	int level = luaL_checknumber(L, 2);

	add_to_levellist(currentbot->lists->rellist, from, level);
	return 0;
}

//lua wrapper to the C ChaineEstDans()
int c2l_chaineestdans(lua_State *L)
{
	const char *afouiller = luaL_checkstring(L, 1);
	const char *achercher = luaL_checkstring(L, 2);
	lua_pushboolean(L, ChaineEstDans(afouiller, achercher));
	return 1;
}

//lua wrapper to the C send_to_user()
int c2l_sendtouser(lua_State *L)
{
	const char *to = luaL_checkstring(L, 1);
	const char *fmt = luaL_checkstring(L, 2);
	lua_pushboolean(L, send_to_user(to, fmt));
	return 1;
}

//lua wrapper to the C sendnotice()
int c2l_sendnotice(lua_State *L)
{
	const char *to = luaL_checkstring(L, 1);
	const char *fmt = luaL_checkstring(L, 2);
	lua_pushboolean(L, sendnotice(to, fmt));
	return 1;
}

//lua wrapper for the bot commands
int c2l_callbotfunc(lua_State *L)
{
	const char *from = luaL_checkstring(L, 1);
	const char *to = luaL_checkstring(L, 2);
	char *args = luaL_checkstring(L, 3);
	char *command;
	int i;
	
	command = get_token(&args, ",: ");
	if(command){
		//TODO : factoriser ce qui est fait dans on_msg pour les tests de niveaux
		for(i = 0; on_msg_commands[i].name != NULL; i++){
			if(STRCASEEQUAL(on_msg_commands[i].name, command)){
				on_msg_commands[i].function(from, to, *args?args:NULL);
			}
		}
	}
	return 0;
}

//lua wrapper to the C Repondre()
int c2l_repondre(lua_State *L)
{
	int i;
	char **Rpos = 0, **Rneg = 0;
	const char *from = luaL_checkstring(L, 1);
	const char *to = luaL_checkstring(L, 2);
	int humpos = luaL_checknumber(L, 3);
	int nbpos = lua_objlen(L, 4);//luaL_checknumber(L, 4);
	int humneg = luaL_checknumber(L, 5);
	int nbneg = lua_objlen(L, 6);//luaL_checknumber(L, 7);
	if(lua_istable(L, 4)){
		Rpos = malloc(nbpos * sizeof(char *));
		for(i=0; i<nbpos; i++){
			lua_rawgeti(L, 4, i+1);
			Rpos[i] = strdup(lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	if(lua_istable(L, 6)){
		Rneg = malloc(nbneg * sizeof(char *));
		for(i=0; i<nbneg; i++){
			lua_rawgeti(L, 6, i+1);
			Rneg[i] = strdup(lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	Repondre(from, to, humpos, nbpos, Rpos, humneg, nbneg, Rneg);
	return 0;
}

//lua wrapper to the C KickerRepondre()
int c2l_kickerrepondre(lua_State *L)
{
	int i;
	char **Rpos = 0, **Rneg = 0;
	const char *from = luaL_checkstring(L, 1);
	const char *to = luaL_checkstring(L, 2);
	int humpos = luaL_checknumber(L, 3);
	int nbpos = lua_objlen(L, 4);//luaL_checknumber(L, 4);
	int humneg = luaL_checknumber(L, 5);
	int nbneg = lua_objlen(L, 6);//luaL_checknumber(L, 7);
	if(lua_istable(L, 4)){
		Rpos = malloc(nbpos * sizeof(char *));
		for(i=0; i<nbpos; i++){
			lua_rawgeti(L, 4, i+1);
			Rpos[i] = strdup(lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	if(lua_istable(L, 6)){
		Rneg = malloc(nbneg * sizeof(char *));
		for(i=0; i<nbneg; i++){
			lua_rawgeti(L, 6, i+1);
			Rneg[i] = strdup(lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	KickerRepondre(from, to, humpos, nbpos, Rpos, humneg, nbneg, Rneg);
	return 0;
}

//lua wrapper to the C botlog()
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

//lua wrapper to the C time2hours()
int c2l_time2hours(lua_State *L)
{
	const int instant = luaL_checknumber(L, 1);
	lua_pushnumber(L, time2hours(instant));
	return 1;
}

//lua getter to size of the Stimuli table
int c2l_stimuli_count(lua_State *L)
{
#ifdef DBUG
	printf("::c2l_stimuli_count\n");
#endif
	lua_pushnumber(L, TailleStim);
	return 1;
}

//lua getter to the given stimulus
int c2l_stimuli_get(lua_State *L)
{
	phr_tbl *stim;
#ifdef DBUG
	printf("::c2l_stimuli_get\n");
#endif
	int index = luaL_checkint(L, 1);
	luaL_argcheck(L, index >= 1 && index <= TailleStim, 1, "index out of range");
	stim = TableDesStimuli[index -1];
	lua_pushstring(L, stim->Stimulus);
	return 1;
}

//lua getter for the present flag of a stimulus
int c2l_stimuli_ispresent(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleStim)
		lua_pushboolean(L, FALSE);
	else
		lua_pushboolean(L, TableDesStimuli[index - 1]->Present ? TRUE : FALSE);
	
	return 1;
}

//lua getter for the active flag of a stimulus
int c2l_stimuli_isactive(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleStim)
		lua_pushboolean(L, FALSE);
	else
		lua_pushboolean(L, TableDesStimuli[index - 1]->Actif ? TRUE : FALSE);
	
	return 1;
}

//lua getter for the name of a stimulus
int c2l_stimuli_getname(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleStim)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesStimuli[index - 1]->NomStimulus);
	
	return 1;
}

//lua getter for the stimulus of a stimulus
int c2l_stimuli_getstimulus(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleStim)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesStimuli[index - 1]->Stimulus);
	
	return 1;
}

//lua getter for the author of a stimulus
int c2l_stimuli_getauthor(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleStim)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesStimuli[index - 1]->Auteur);
	
	return 1;
}

//methods of the bot.stimuli meta-table
static const struct luaL_reg stimuli_m[] = {
	{"size", c2l_stimuli_count},
	{"get", c2l_stimuli_get},
	{"get_name", c2l_stimuli_getname},
	{"get_stimulus", c2l_stimuli_getstimulus},
	{"get_author", c2l_stimuli_getauthor},
	{"is_present", c2l_stimuli_ispresent},
	{"is_active", c2l_stimuli_isactive},
	{NULL, NULL}
};

//lua getter to size of the Response table
int c2l_response_count(lua_State *L)
{
#ifdef DBUG
	printf("::c2l_response_count\n");
#endif
	lua_pushnumber(L, TailleRep);
	return 1;
}

//lua getter to the given response
int c2l_response_get(lua_State *L)
{
	rep_tbl *resp;
#ifdef DBUG
	printf("::c2l_response_get\n");
#endif
	int index = luaL_checkint(L, 1);
	luaL_argcheck(L, index >= 1 && index <= TailleRep, 1, "index out of range");
	resp = TableDesReponses[index -1];
	lua_pushstring(L, resp->Reponse);
	return 1;
}

//lua getter for the stimulus of a response
int c2l_response_getstimulus(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleRep)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesReponses[index - 1]->NomStimulus);
	
	return 1;
}

//lua getter for the response of a response
int c2l_response_getresponse(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleRep)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesReponses[index - 1]->Reponse);
	
	return 1;
}

//lua getter for the author of a response
int c2l_response_getauthor(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleRep)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesReponses[index - 1]->Auteur);
	
	return 1;
}

//lua getter for the channel of a response
int c2l_response_getchannel(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleRep)
		lua_pushstring(L, "");
	else
		lua_pushstring(L, TableDesReponses[index - 1]->Canal);
	
	return 1;
}

//lua getter for the active flag of a response
int c2l_response_isactive(lua_State *L)
{
	int index = luaL_checkint(L, 1);
	if(index > TailleRep)
		lua_pushboolean(L, FALSE);
	else
		lua_pushboolean(L, TableDesReponses[index - 1]->Active ? TRUE : FALSE);
	
	return 1;
}

//methods of the bot.answer meta-table
static const struct luaL_reg response_m[] = {
	{"size", c2l_response_count},
	{"get", c2l_response_get},
	{"get_stimulus", c2l_response_getstimulus},
	{"get_response", c2l_response_getresponse},
	{"get_author", c2l_response_getauthor},
	{"get_channel", c2l_response_getchannel},
	{"is_active", c2l_response_isactive},
	{NULL, NULL}
};

//initialize the TableDesStimuli global user object as an accessor to the TableDesStimuli
//and configure a metatable with the needed methods
void expose_stimlist(void)
{
	//create a metatable
	luaL_newmetatable(L, "bot.stimuli");

	//connect the __index method to the list of our methods
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);	//push metatable
	lua_settable(L, -3);	//metatable.__index = bot.stimuli metatable
	
	//associate the list of methods to the metatable
	//with the NULL name, assume that the package to populate is the metatable on the stack
	luaL_openlib(L, NULL, stimuli_m, 0);
	
	//Create a user object that points to TableDesStimuli
	phr_tbl **p = (phr_tbl**)lua_newuserdata(L, sizeof(phr_tbl*));
	p = TableDesStimuli;
	
	//set the meta-table of our TableDesStimuli global object
	luaL_getmetatable(L, "bot.stimuli");
	lua_setmetatable(L, -2);
	lua_setglobal(L, "TableDesStimuli");
}

//initialize the TableDesReponses global user object as an accessor to the TableDesReponses
//and configure a metatable with the needed methods
void expose_resplist(void)
{
	//create a metatable
	luaL_newmetatable(L, "bot.responses");
	
	//connect the __index method to the list of our methods
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);	//push metatable
	lua_settable(L, -3);	//metatable.__index = bot.responses metatable
	
	//associate the list of methods to the metatable
	//with the NULL name, assume that the package to populate is the metatable on the stack
	luaL_openlib(L, NULL, response_m, 0);
	
	//Create a user object that points to TableDesReponses
	rep_tbl **p = (rep_tbl**)lua_newuserdata(L, sizeof(rep_tbl*));
	p = TableDesReponses;
	
	//set the meta-table of our TableDesStimuli global object
	luaL_getmetatable(L, "bot.responses");
	lua_setmetatable(L, -2);
	lua_setglobal(L, "TableDesReponses");
}

//register the C methods and values accessible to the lua engine
void register_cstuff()
{
	//some constants that can be useful to lua too
	lua_pushnumber(L, HOURS_BETWEEN_SALUTES);
	lua_setglobal(L, "HOURS_BETWEEN_SALUTES");
	lua_pushstring(L, botmaintainer);
	lua_setglobal(L, "botmaintainer");
	
	//file names
	lua_pushstring(L, LOGFILE);
	lua_setglobal(L, "LOGFILE");
	lua_pushstring(L, ERRFILE);
	lua_setglobal(L, "ERRFILE");
	
	//levels
	lua_pushnumber(L, AUTO_OPLVL);
	lua_setglobal(L, "AUTO_OPLVL");
	lua_pushnumber(L, SYMPA_LVL);
	lua_setglobal(L, "SYMPA_LVL");
	lua_pushnumber(L, CONFIDENCE_LVL);
	lua_setglobal(L, "CONFIANCE_LVL");
	lua_pushnumber(L, DEFAUT_LVL);
	lua_setglobal(L, "DEFAUT_LVL");
	lua_pushnumber(L, loglevel);
	lua_setglobal(L, "LogLevel");
	

	//interface functions
	lua_register(L, "is_channel", c2l_ischannel);
	lua_register(L, "can_talk", c2l_cantalk);
	lua_register(L, "set_talk", c2l_settalk);
	lua_register(L, "is_log_on", c2l_islogon);
	lua_register(L, "nick_user_str", c2l_nickuserstr);
	lua_register(L, "getnick", c2l_getnick);
	lua_register(L, "locuteur_existe", c2l_locutorexists);
	lua_register(L, "ajoute_locuteur", c2l_addlocutor);
	lua_register(L, "locuteur_getbonjours", c2l_getlocutorsalutes);
	lua_register(L, "locuteur_setbonjours", c2l_setlocutorsalutes);
	lua_register(L, "locuteur_derniercontact", c2l_getlocutorlastcontact);
	lua_register(L, "userlevel", c2l_getuserlevel);
	lua_register(L, "shitlevel", c2l_getshitlevel);
	lua_register(L, "protlevel", c2l_getprotlevel);
	lua_register(L, "rellevel", c2l_getrellevel);
	lua_register(L, "exist_userhost", c2l_existuserhost);	
	lua_register(L, "add_to_levellist", c2l_addtolevellist);	
	lua_register(L, "add_to_level", c2l_addtolevel);
	lua_register(L, "chaine_est_dans", c2l_chaineestdans);
	lua_register(L, "send_to_user", c2l_sendtouser);
	lua_register(L, "send_notice", c2l_sendnotice);
	lua_register(L, "repondre", c2l_repondre);
	lua_register(L, "kicker_repondre", c2l_kickerrepondre);
	lua_register(L, "botlog", c2l_botlog);
	lua_register(L, "time2hours", c2l_time2hours);
	lua_register(L, "call_bot_func", c2l_callbotfunc);
	
	expose_stimlist();
	expose_resplist();
}

//initialize the lua engine
int init_lua()
{
	L = luaL_newstate();
	if(!L)
		return 1;
	luaL_openlibs(L);
	//register_cstuff();
	return 0;
}

//shutdown the lua engine
void shutdown_lua()
{
	if(L)
		lua_close(L);
	L = NULL;
}

//(re)load the lua logic from the lua scripts
//That method must be called first when the bot starts
//and later it can also make the bot refresh its logic without restarting 
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

//processing of a public (in a channel) or private message by the lua engine
void LuaTraite(botinfo	*currentbot, char *from, char *to, char *msg, int numphrase)
{
	static int Jour = 0;

	/* do not pass a complex botinfo struct, just expose the bot nick */
	lua_pushstring(L, "BotNick");
	lua_pushstring(L, currentbot->nick);
	lua_settable(L, LUA_GLOBALSINDEX);
	
	lua_pushstring(L, "Logging");
	lua_pushboolean(L, logging);
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
	// était dans Traite()
	/* Si c'est un nouveau jour */
	if(Jour != gettimeday(time(NULL))) {
		Jour = gettimeday(time(NULL));
		
		/* On sauvegarde le fichier des relations */
		cancel_level(currentbot->lists->rellist, DEFAUT_LVL);
		write_lvllist(currentbot->lists->rellist,
					   currentbot->lists->relfile,
					  SORT_DESC);
		
		/* On nettoie la liste des locuteurs des locuteurs muets anciens */
		NettoieListeLocuteurs(currentbot->lists->ListeLocuteurs);
		
		/* On sauvegarde aussi le fichier des locuteurs */
		SauveLocuteurs(currentbot->lists->ListeLocuteurs,
						currentbot->lists->locuteurfile);
	}
	
}




// Local variables:
// coding: utf-8
// end:

