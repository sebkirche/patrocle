/*
 * cfgfile.c - Simpele parser voor configfile
 * Copyright (C) 1993-94 VladDrac (irvdwijk@cs.vu.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
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

#include "cfgfile.h"
#include "debug.h"
#include "misc.h"
#include "parsing.h"
#include "vladbot.h"
#include "phrase.h"
#include "luainterface.h"

/*
 * Some short info on how things are parsed.
 * A line can be one of four things:
 * 1) %command 	params		(i.e. %debug 2)
 * 2) BotName	definition	params (i.e. NederServ channel #dutch)
 * 3)		definition	params (when %with was used)
 * 4) # comments....
 *
 * Lines are parsed one by one. When looking at the first token,
 * the parser looks if the token is a command (started with %),
 * a botname (in this case, the definition takes effect on the
 * specified bot) or a just a definition (in this case the default
 * bot is assumed). When the first token starts with '#', the rest 
 * of the line is ignored. 
 * NOTE: botname has higher priority than definition,
 * i.e look at this piece of code:
 * %bot Channel		(weird name for a bot)
 * %bot Blah
 * %with Blah
 * Channel server "irc.funet.fi"
 *
 * The last line will add the server to Channel's serverlist.
 *
 * After all parsing on a line has been finished, the rest of the
 * line is discarded.
 */

struct
{
	char	*name;
	void	(*function)(char *s);
} config_cmds[] =
{
	/* Table with commands used to configure */
	{ "SET",	set_var		},
	{ "DEBUG",	get_globaldebug	},
	{ "CREATE",	create_bot      },
	{ "BOT",	create_bot	},
	{ "LISTSET",	create_userlist	},
	{ "WITH",	set_default     },
	{ NULL,		(void(*)())(NULL) }
};

struct
{
	char	*name;
	int	bot_def;	/* bot or list definition */
	void    (*function)();
} definition_cmds[] =
/* for bots AND for userlists */
{
	/* list definitions */
	{ "USERLIST",	FALSE,	set_listuser	},
	{ "PROTLIST",	FALSE,	set_listprot	},
	{ "SHITLIST",	FALSE,	set_listshit	},
	{ "RELLIST",    FALSE,  set_listrel     },
	/* Bot definitions */
	{ "NICK",	TRUE,	set_nick	},
	{ "LOGIN",	TRUE,	set_login       },
	{ "NAME",	TRUE,	set_name        },
	{ "SERVER",	TRUE,	add_server      },
	{ "CHANNEL",	TRUE,	add_to_channellist     },
	{ "UPLOAD",	TRUE,	set_uploaddir	},
	{ "DOWNLOAD",	TRUE,	set_downloaddir	},
	{ "INDEX",	TRUE,	set_indexfile	},
	{ "HELP",	TRUE,	set_helpfile	},
	{ "LISTSET",	TRUE,	set_listset	},
	{ "STIM",       TRUE,   set_stimfile    },
	{ "REPS",       TRUE,   set_repfile     },
	{ "BOTSLIST",   TRUE,   set_botfile     },
	{ NULL,		0,	(void(*)())(NULL) }
};

struct
{
	char	*name;
	void    (*function)();
} setting_cmds[] =
{	
	{ "IDLETIMEOUT",	get_idletimeout 	},
	{ "WAITTIMEOUT",	get_waittimeout 	},
	{ "MAXUPLOADSIZE",	get_maxuploadsize 	},
	{ "NOTEFILE",		get_notefile 		},
	{ "DEBUG",		get_globaldebug		},
	{ "MAINTAINER",		get_maintainer		},
	{ NULL,         	(void(*)())(NULL) 	}
};


extern	int	number_of_bots;
extern	int	find_channel(botinfo *bot, char *channel);
extern	int	add_channel_to_bot(botinfo *bot, char *channel, char *topic, char *mode, char *encoding);
extern	int	find_server(botinfo *bot, char *server, int port);
extern	int	add_server_to_bot(botinfo *bot, char *server, int port);
extern	botinfo	*bot_created(char *s);
extern	botinfo	*add_bot(char *s);
extern	int	idletimeout;
extern	int	waittimeout;
extern	int	maxuploadsize;
extern	char	*notefile;
extern	char	*botmaintainer;

static	botinfo		*defaultbot = NULL;
static	listinfo	*defaultset = NULL;
static	int	linenum = 0;
static	int	dbg_lvl = QUIET;

extern lua_State *L;
char	*configfile = "cfg.lua";

/*
 * misc functions
 */

static	void	cfg_debug(int lvl, char *form, ...)
{
	va_list	msg;
	char	buf[MAXLEN];

	if(lvl>dbg_lvl)
		return;
	va_start(msg, form);
	vsprintf(buf, form, msg);
	printf("[%d] %s\n", linenum, buf);
	va_end(msg);
}

/* commands etc. */

int get_num(char *name)
{
    if(lua_isnumber(L, -1)){
		return 1;
    }
    else if(!lua_isnil(L, -1)){
		printf("ERROR: %s requires a numeric compatible value\n", name?name:"parameter");
    }
	lua_pop(L, 1);
	return 0;
}
int get_globalnum(char *name)
{
	lua_getglobal(L, name);
	return get_num(name);
}

int	get_str(char *name)
{
    if(lua_isstring(L, -1)){
		return 1;
    }
    else if(!lua_isnil(L, -1)){
		printf("ERROR: %s requires a string compatible value\n", name?name:"parameter");
    }
	lua_pop(L, 1);
	return 0;
}

int get_globalstr(char *name)
{
	lua_getglobal(L, name);
	return get_str(name);
}

void	get_globaldebug()
/*
 * Sets the debuglevel for the parser.
 * Syntax: %debug INTEGER[0..2]
 */
{
	int	value;
	if(get_globalnum("debug")){
		value = lua_tonumber(L, -1);
		lua_pop(L, 1);
		if(value < QUIET || value > NOTICE)
			cfg_debug(ERROR, "%%DEBUG expects 0 <= argument <= 2!");
		else
		{
			cfg_debug(NOTICE, "DEBUG set to %d", value);
			dbg_lvl = value;
		}
	}
}

void	create_bot(char *botname)
/*
 * Creates a bot.
 * Syntax: %bot IDENTIFIER
 */
{
	//char	botname[MAXLEN];

	if(listset_created(botname)){
		cfg_debug(ERROR, "There is already a listset called \"%s\"", botname);
		return;
	}
	if(bot_created(botname)){
		cfg_debug(ERROR, "\"%s\" is already created!", botname);
		return;
	}
	if(!add_bot(botname))
		cfg_debug(ERROR, "Too many bots created!");
	else
		cfg_debug(NOTICE, "CREATE: bot \"%s\"", botname);
}

void	create_userlist(char *s)
/*
 * Create a userlist.
 * Syntax: %create IDENTIFIER
 */
{
	char	listname[MAXLEN];
	
	if(readident(&s, listname))
	{
		if(bot_created(listname))
		{
			cfg_debug(ERROR, "There is already a bot called \"%s\"", listname);
			return;
		}
		if(listset_created(listname))
		{
			cfg_debug(ERROR, "\"%s\" is already created!", listname);
			return;
		}
		if(!add_listset(listname))
			cfg_debug(ERROR, "Too many listsets created!");
		else
			cfg_debug(NOTICE, "LISTSET: listset \"%s\"", listname);
	}
	else
		cfg_debug(ERROR, "%%LISTSET requires an identifier-argument!");
}


void	set_default(char *botname)
/*
 * Set default bot/listset for %with
 * Syntax: %with IDENTIFIER
 */
{
	if(bot_created(botname)){
		cfg_debug(NOTICE, "DEFAULT: bot \"%s\"", botname);
		defaultbot = bot_created(botname);
	}
	else if(listset_created(botname)){
		cfg_debug(NOTICE, "DEFAULT: listset \"%s\"", botname);
		defaultset = listset_created(botname);
	}
	else
		cfg_debug(ERROR, "\"%s\" is not created!", botname);
}

/* settings/definitions */

void	set_listprot(listinfo *list, char *listname)
/*
 * Set the protlist for list.
 * Syntax: protlist STRING
 */
{
	//char	listname[MAXLEN];
	char	*path;

		if(not(path = expand_twiddle(listname)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", listname);
			path = "";
		}
		free(list->protfile);
		mstrcpy(&list->protfile, path);
		cfg_debug(NOTICE, "Setting protlist for %s to \"%s\"",
			  list->listname, path);
		return;
}

void	set_listshit(listinfo *list, char *listname)
/*
 * Set the shitlist for list.
 * Syntax: shitlist STRING
 */
{
	//	char	listname[MAXLEN];
	char	*path;
	
		if(not(path = expand_twiddle(listname)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", listname);
			path = "";
		}
		free(list->shitfile);
		mstrcpy(&list->shitfile, path);
		cfg_debug(NOTICE, "Setting shitlist for %s to \"%s\"",
			  list->listname, path);
		return;
}

void	set_listuser(listinfo *list, char *listname)
/*
 * Set the userlist for list.
 * Syntax: userlist STRING
 */
{
	//char	listname[MAXLEN];
	char	*path;
	
		if(not(path = expand_twiddle(listname)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", listname);
			path = "";
		}		
		free(list->opperfile);
		mstrcpy(&list->opperfile, path);
		cfg_debug(NOTICE, "Setting userlist for %s to \"%s\"",
			  list->listname, path);
		return;
}

void    set_listrel(listinfo *list, char *listname)
/*
 * Set the rellist for list.
 * Syntax: rellist STRING
 */
{
	//  char listname[MAXLEN];
  char *path;

    if (not (path = expand_twiddle (listname)))
    {
      cfg_debug (ERROR, "Error in pathname \"%s\"!", listname);
      path = "";
    }
    free (list->relfile);
    mstrcpy(&list->relfile, path);
    cfg_debug(NOTICE, "Setting userlist for %s to \"%s\"",
	      list->listname, path);
    return;
}

void	set_listset(botinfo *bot, char *s)
/*
 * Set the listset for bot.
 * Syntax: listset IDENTIFIER
 */
{
	char	listname[MAXLEN];
	
	if(readident(&s, listname))
	{
		if(listset_created(listname))
		{
			bot->lists = listset_created(listname);
			cfg_debug(NOTICE, "Setting lists for %s to \"%s\"",
				  bot->botname, listname);
		}
		else
			cfg_debug(ERROR, "No listset \"%s\"!", listname);
	}
	else
		cfg_debug(ERROR, "%s listset expects a identifier-argument!",
			  bot->botname);
}

void	set_nick(botinfo *bot, char *nickname)
/*
 * Set the nickname for bot.
 * Syntax: nick STRING
 */
{
	//char	nickname[MAXLEN];
	
		if(!isnick(nickname))
		{
			cfg_debug(ERROR, "Illegal nickname \"%s\"!", nickname);
			return;
		}
		cfg_debug(NOTICE, "Setting nick for %s to \"%s\"", 
			  bot->botname, nickname);
		strcpy(bot->nick, nickname);
		strcpy(bot->realnick, nickname);
}

void	set_login(botinfo *bot, char *loginname)
/*
 * Set the loginname for bot.
 * Syntax: login STRING
 */
{
	//char	loginname[MAXLEN];
	
		cfg_debug(NOTICE, "Setting login for %s to \"%s\"", 
                          bot->botname, loginname);
		strcpy(bot->login, loginname);
}

void	set_name(botinfo *bot, char *realname)
/*
 * Set the name for bot.
 * Syntax: name STRING
 */
{  
	//char	realname[MAXLEN];
	
                cfg_debug(NOTICE, "Setting name for %s to \"%s\"", 
			  bot->botname, realname);
		free(bot->name);
		mstrcpy(&bot->name, realname);
}

void	add_server(botinfo *bot, char *servername)
/*
 * Adds a server to the serverlist of bot
 * Syntax: server STRING [, INTEGER]
 *                (name)    (port)
 */
{  
	//char	*servername;//[MAXLEN];
	int	port = 0;
	
	//servername = get_token(&s, ":");
	//todo parse the port
	port = 6667;
		
	if(find_server(bot, servername, port))
		cfg_debug(ERROR, "Server \"%s\" port %d already in list!", 
				  servername, port);
	else
		if(add_server_to_bot(bot, servername, port))
			cfg_debug(NOTICE, 
					  "Server \"%s\" with port %d added to \"%s\"",
					  servername, port, bot->botname);
		else
			cfg_debug(ERROR, "Serverlist full!");
}

void	add_to_channellist(botinfo *bot)
/*
 * Adds a channel to bot's channellist.
 * Syntax: channel STRING [, STRING [, STRING [, STRING ]]]
 *                 (name)    (mode)    (topic)   (encoding)
 */
{  
	char	channelname[MAXLEN];
	char	mode[MAXLEN];
	char	topic[MAXLEN];
	char	encoding[MAXLEN];
	
 
	strcpy(mode, "");
	strcpy(topic, "");
	strcpy(encoding, "");

	if(!lua_istable(L, -1))
		return;
	lua_getfield(L, -1, "name");
	if(get_str("name")){
		strncpy(channelname, lua_tostring(L, -1), sizeof(channelname)-1);
		lua_pop(L, 1);
	}
	lua_getfield(L, -1, "mode");
	if(get_str("mode")){
		strncpy(mode, lua_tostring(L, -1), sizeof(mode)-1);
		lua_pop(L, 1);
	}
	lua_getfield(L, -1, "topic");
	if(get_str("topic")){
		strncpy(topic, lua_tostring(L, -1), sizeof(topic)-1);
		lua_pop(L, 1);
	}
	lua_getfield(L, -1, "encoding");
	if(get_str("encoding")){
		strncpy(encoding, lua_tostring(L, -1), sizeof(encoding)-1);
		lua_pop(L, 1);
	}
	
	if(find_channel(bot, channelname))
		cfg_debug(ERROR, "Channel \"%s\" already in list!", channelname);
	else
		if(add_channel_to_bot(bot, channelname, topic, mode, encoding))
			cfg_debug(NOTICE, "Channel \"%s\" \"%s\" \"%s\" \"%s\" added to %s", 
					  channelname, mode, topic, encoding, bot->botname);
		else
			cfg_debug(ERROR, "Channellist full!");
}

void	set_uploaddir(botinfo *bot, char *upload)
/*
 * Sets the uploaddir for bot.
 * Syntax: upload STRING
 */
{
	//char	upload[MAXLEN];	
	char	*path;
	
	if(not(path = expand_twiddle(upload))){
		cfg_debug(ERROR, "Error in pat	hname \"%s\"!", upload);
		path = "";
	}         
	cfg_debug(NOTICE, "Setting upload for %s to \"%s\"", 
			  bot->botname, path);
	free(bot->uploaddir);
	mstrcpy(&bot->uploaddir, path);
}

void	set_downloaddir(botinfo *bot, char *download)
/*
 * Sets downloaddir for bot.
 * Syntax: download STRING
 */
{
	//char	download[MAXLEN];
	char	*path;

       		if(not(path = expand_twiddle(download)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", download);
			path = "";
		}         
		cfg_debug(NOTICE, "Setting download for %s to \"%s\"",
                          bot->botname, path);
		free(bot->downloaddir);
                mstrcpy(&bot->downloaddir, path);
}

void	set_indexfile(botinfo *bot, char *index)
/*
 * Sets indexfile for bot.
 * Syntax: index STRING
 */
{
	//char	index[MAXLEN];
	char	*path;

       		if(not(path = expand_twiddle(index)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", index);
			path = "";
		}         
		cfg_debug(NOTICE, "Setting indexfile for %s to \"%s\"",
                          bot->botname, path);
		free(bot->indexfile);
                mstrcpy(&bot->indexfile, path);
}

void    set_helpfile(botinfo *bot, char *help)
/*
 * Sets helpfile for bot.
 * Syntax: help STRING
 */
{
	//char	help[MAXLEN];
	char	*path;
	
       		if(not(path = expand_twiddle(help)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", help);
			path = "";
		}         
		cfg_debug(NOTICE, "Setting helpfile for %s to \"%s\"",
                          bot->botname, path);
		free(bot->helpfile);
                mstrcpy(&bot->helpfile, path);
}

void    set_stimfile(botinfo *bot, char *stim)
/*
 * Sets stimfile for bot.
 * Syntax: stim STRING
 */
{
	//char	stim[MAXLEN];
	char	*path;
	
       		if(not(path = expand_twiddle(stim)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", stim);
			path = "";
		}         
		cfg_debug(NOTICE, "Setting stimfile for %s to \"%s\"",
                          bot->botname, path);
		free(bot->stimfile);
                mstrcpy(&bot->stimfile, path);
		ChargeStimuli (path);
}

void    set_repfile(botinfo *bot, char *rep)
/*
 * Sets repfile for bot.
 * Syntax: rep STRING
 */
{
	//char	rep[MAXLEN];
	char	*path;
	
       		if(not(path = expand_twiddle(rep)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", rep);
			path = "";
		}         
		cfg_debug(NOTICE, "Setting repfile for %s to \"%s\"",
                          bot->botname, path);
		free(bot->repfile);
                mstrcpy(&bot->repfile, path);
		ChargeReponses (path);
}

void    set_botfile (botinfo *bot, char *Bot)
{
	//char Bot[MAXLEN];
	char *path;

    if (not (path = expand_twiddle (Bot))) {
		cfg_debug (ERROR, "Error in pathname \"%s\"!", Bot);
		path = "";
    }
    cfg_debug(NOTICE, "Setting botfile for %s to \"%s\"",
			  bot->botname, path);
    free (bot->botfile);
    mstrcpy (&bot->botfile, path);
    readbotdatabase (path, bot->botlist);
}

/* settings */

void	get_notefile()
/*
 * Sets global notefile.
 * Syntax: notefile STRING
 */
{
	char	nfile[MAXLEN];
	char	*path;

	if(get_globalstr("notefile")){
		memset(nfile, 0, sizeof(nfile));
		strncpy(nfile, lua_tostring(L, -1), MAXLEN - 1);
		if(not(path = expand_twiddle(nfile)))
		{
			cfg_debug(ERROR, "Error in pathname \"%s\"!", nfile);
			path = "";
		}		
		else
		{
			cfg_debug(NOTICE, "Setting notefile to \"%s\"", path);
			mstrcpy(&notefile, path);
		}
		lua_pop(L, 1);
	}
}


void	get_idletimeout()
/*
 * Sets global idletimeout.
 * Syntax: idletimeout INTEGER
 */
{
	if(get_globalnum("idletimeout")){
		idletimeout = lua_tonumber(L, -1);
		lua_pop(L, 1);
		cfg_debug(NOTICE, "Setting idletimeout to %d", idletimeout);
	}
	else
	{
		idletimeout = DCC_IDLETIMEOUT;
	}
}

void	get_waittimeout()
/*
 * Sets global waittimeout.
 * Syntax: waittimeout INTEGER
 */
{
	if(get_globalnum("waittimeout")){
		waittimeout = lua_tonumber(L, -1);
		lua_pop(L, 1);
		cfg_debug(NOTICE, "Setting waittimeout to %d", waittimeout);
	}
	else
	{
		waittimeout = DCC_WAITTIMEOUT;
	}
}

void	get_maxuploadsize()
/*
 * Sets global maxuploadsize.
 * Syntax: maxuploadsize INTEGER
 */
{
	if(get_globalnum("maxuploadsize")){
		maxuploadsize = lua_tonumber(L, -1);
		lua_pop(L, 1);
		cfg_debug(NOTICE, "Setting maxuploadsize to %d", maxuploadsize);
	}
	else
	{
		maxuploadsize = DCC_MAXFILESIZE;
	}
}

void	set_globaldebug(char *s)
/*
 * Sets global debug value.
 * Syntax: debug INTEGER
 */
{
	int	value;

	if(readint(&s, &value))
        {
		if(set_debuglvl(value))
			cfg_debug(NOTICE, "Setting debug to %d", value);
		else
			cfg_debug(ERROR, "debug expects a number 0..2 (integer)!");
		
	}
	else
		cfg_debug(ERROR, "debug expects a number 0..2 (integer)!");
}

void	get_maintainer()
/*
 * Sets the name of the botmaintainer
 * Syntax: maintainer STRING
 */
{
	char	name[MAXLEN];
	if(get_globalstr("maintainer")){
		if(botmaintainer)
			free(botmaintainer);
		memset(name, 0, sizeof(name));
		strncpy(name, lua_tostring(L, -1), MAXLEN - 1);
		mstrcpy(&botmaintainer, name);		
		lua_pop(L, 1);
		cfg_debug(NOTICE, "Setting maintainer to \"%s\"", botmaintainer);
	}
}

void	set_var(char *s)
/*
 * Assigns value to variable.
 * Syntax: %set VARIABLE VALUE [, VALUE [, ...]]
 */
{
	int	i;
        char    command[MAXLEN];;

	if(not(readident(&s, command)))
	{
		cfg_debug(ERROR, "Setting expected after %%SET");
		return;
	}
	for(i=0; setting_cmds[i].name; i++)
		if(STRCASEEQUAL(setting_cmds[i].name, command))
		{
			skipspc(&s);
			setting_cmds[i].function(s);
			return;
		}	
	cfg_debug(ERROR, "Unknown setting %s", command);
}

void	parsecommand(char *s)
{
        int     i;
        char    command[MAXLEN];

        s++;	/* skip % */
	if(not(readident(&s, command)))
		return;
        for(i=0; config_cmds[i].name != NULL; i++)
                if(STRCASEEQUAL(config_cmds[i].name, command))
                {
			skipspc(&s);
                        config_cmds[i].function(s);
                        return;
                }
        cfg_debug(ERROR, "ERROR: unknow command %%%s", command);
        return;
}

void	parsedef(char *s)
/*
 * BotNick	Definition	Values
 * (botnick created)
 * Definition	Values
 * (default bot set)
 */
{
	char	firsttok[MAXLEN];
	char	command[MAXLEN];
	botinfo	*defbot = NULL;
	listinfo *deflist = NULL;
	int	i;

	skipspc(&s);
	if(not(readident(&s, firsttok)))
		return;

/* BotNick has higer priority (because command could be nick) */
	if((defbot = bot_created(firsttok)) || 
            (deflist = listset_created(firsttok)))
	{
		if(not(readident(&s, command)))
		{
			cfg_debug(ERROR, "Definition expected!");
			return;
		}
		skipspc(&s);
		for(i=0; definition_cmds[i].name; i++)
			if(STRCASEEQUAL(definition_cmds[i].name, command))
			{
				if(definition_cmds[i].bot_def)
					if(defbot == NULL)
					{
						cfg_debug(ERROR, "No default bot set!");
						return;
					}
					else
						definition_cmds[i].function(defbot, s);
				else
					if(deflist == NULL)
					{
						cfg_debug(ERROR, "No default list set!");
						return;
					}
					else
						definition_cmds[i].function(deflist, s);
				return;
			}
		cfg_debug(ERROR, "Unknown definition %s", command);
	}
	else
	{
		if(!defaultbot && !defaultset)
		{
			cfg_debug(ERROR, "ERROR: No default bot/listset set!");
			return;
		}	
                strcpy(command, firsttok);
                for(i=0; definition_cmds[i].name; i++)
                        if(STRCASEEQUAL(definition_cmds[i].name, command))
                        {
				skipspc(&s);
       				if(definition_cmds[i].bot_def)
				{	
					if(defaultbot == NULL)
					{
						cfg_debug(ERROR, "No default bot set!");
						return;
					}
					definition_cmds[i].function(defaultbot, s);
				}
				else
				{
					if(defaultset == NULL)
					{
						cfg_debug(ERROR, "No default list set!");
						return;
					}
					definition_cmds[i].function(defaultset, s);
	                        }
				return;
                        }
		cfg_debug(ERROR, "Unknown definition %s", command);
	}
}

void	parsecfg(char *s)
{
	KILLNEWLINE(s);
	skipspc(&s);
	switch(*s)
	{
	case '%':
		cfg_debug(NOTICE, ">> %s", s);
		parsecommand(s);
		break;
	case '#':
		break;
	default:
		cfg_debug(NOTICE, " > %s", s);
		parsedef(s);
		break;
	}
}

char	*readline(FILE *f, char *s)
{
	assert(f != NULL);
	if(fgets(s,255,f))
		return s;
	return NULL;
}

void parse_table_setting()
{
	lua_pushnil(L);
	while(lua_next(L, -2) != 0){
		if(strcasecmp(lua_tostring(L, -4), "channels"))
			//it is not channels	
			printf("    table elem = %s\n", lua_tostring(L, -1));
		else{
			//channels items are also tables
			lua_pushnil(L);
			while(lua_next(L, -2) != 0){
				printf("    %s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);
	}
}

void parse_channels()
{
	if(lua_istable(L, -1)){
		lua_pushnil(L);
		while(lua_next(L, -2) != 0){
			add_to_channellist(defaultbot);
			lua_pop(L, 1);
		}
	}
}

void parse_servers()
{
	char server[MAXLEN];
	if(lua_istable(L, -1)){
		lua_pushnil(L);
		while(lua_next(L, -2) != 0){
			strncpy(server, lua_tostring(L, -1), sizeof(server)-1);
			add_server(defaultbot, server);
			lua_pop(L, 1);
		}
	}
}

void parse_fileset()
{
	listinfo *list;
	char name[MAXLEN];
	char filepath[MAXLEN];
	char *expandedpath;

	if(lua_istable(L, -1)){
		lua_getfield(L, -1, "name");
		if(lua_isstring(L, -1)){
			strncpy(name, lua_tostring(L, -1), sizeof(name)-1);
			if(!(list = listset_created(name))){
				if(list = add_listset(name)){
					//user
					lua_getfield(L, -2, "userlist");
					strncpy(filepath, lua_tostring(L, -1), sizeof(filepath)-1);
					if(not(expandedpath = expand_twiddle(filepath)))
						expandedpath = "";
					free(list->opperfile);
					mstrcpy(&list->opperfile, expandedpath);
					lua_pop(L, 1);
					//prot
					lua_getfield(L, -2, "protlist");
					strncpy(filepath, lua_tostring(L, -1), sizeof(filepath)-1);
					if(not(expandedpath = expand_twiddle(filepath)))
						expandedpath = "";
					free(list->protfile);
					mstrcpy(&list->protfile, expandedpath);
					lua_pop(L, 1);
					//shit
					lua_getfield(L, -2, "shitlist");
					strncpy(filepath, lua_tostring(L, -1), sizeof(filepath)-1);
					if(not(expandedpath = expand_twiddle(filepath)))
						expandedpath = "";
					free(list->shitfile);
					mstrcpy(&list->shitfile, expandedpath);
					lua_pop(L, 1);
					//rel
					lua_getfield(L, -2, "rellist");
					strncpy(filepath, lua_tostring(L, -1), sizeof(filepath)-1);
					if(not(expandedpath = expand_twiddle(filepath)))
						expandedpath = "";
					free(list->relfile);
					mstrcpy(&list->relfile, expandedpath);
					lua_pop(L, 1);
				}
			}
			defaultbot->lists = list;
		}
		lua_pop(L, 1);
	}
}

void parse_bot(void)
{
	char buf[255];

    //the bot table is on top of stack
    printf("parse 1 bot...\n");
    if(lua_istable(L, -1)){
		/*
		lua_pushnil(L);
		while(lua_next(L, -2) != 0){
			printf("  parse 1 bot setting, type = %s\n", lua_typename(L, lua_type(L, -1)));
			if(lua_type(L, -2) == LUA_TSTRING){
				printf("   found bot parameter %s\n", lua_tostring(L, -2));
			}
			if(lua_isstring(L, -1)){
				printf("   parameter value =  %s\n", lua_tostring(L, -1));
			}
			if(!strcasecmp(lua_tostring(L, -2), "channels")
			   || !strcasecmp(lua_tostring(L, -2), "servers")
			   || !strcasecmp(lua_tostring(L, -2), "listset")
			   ){
				parse_table_setting();
			}
			lua_pop(L, 1);
		}*/
		//on ne va pas parcourir la table, on va interroger les items voulus
		// create bot
		lua_getfield(L, -1, "id");
		if(get_str("id")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			create_bot(buf);
			set_default(buf);
			lua_pop(L, 1);
		}

		//set its nick
		lua_getfield(L, -1, "nick");
		if(get_str("nick")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_nick(defaultbot, buf);
			lua_pop(L, 1);
		}

		//set its name
		lua_getfield(L, -1, "name");
		if(get_str("name")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_name(defaultbot, buf);
			lua_pop(L, 1);
		}

		//set its login
		lua_getfield(L, -1, "login");
		if(get_str("login")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_login(defaultbot, buf);
			lua_pop(L, 1);
		}
		
		//process the channels
		lua_getfield(L, -1, "channels");
		if(lua_istable(L, -1))
			parse_channels();
		lua_pop(L, 1);

		//process the servers
		lua_getfield(L, -1, "servers");
		if(lua_istable(L, -1))
			parse_servers();
		lua_pop(L, 1);

		//set its stimlist
		lua_getfield(L, -1, "stims");
		if(get_str("stims")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_stimfile(defaultbot, buf);
			lua_pop(L, 1);
		}

		//set its replist
		lua_getfield(L, -1, "reps");
		if(get_str("reps")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_repfile(defaultbot, buf);
			lua_pop(L, 1);
		}

		//set its botlist
		lua_getfield(L, -1, "botlist");
		if(get_str("botlist")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_botfile(defaultbot, buf);
			lua_pop(L, 1);
		}

		//set its index
		lua_getfield(L, -1, "index");
		if(get_str("index")){
			//memset(buf, 0, sizeof(buf));
			strncpy(buf, lua_tostring(L, -1), sizeof(buf)-1);
			set_indexfile(defaultbot, buf);
			lua_pop(L, 1);
		}

		//get the files list
		lua_getfield(L, -1, "fileset");
		if(lua_istable(L, -1)){
			parse_fileset();
		}
		lua_pop(L, 1);
    }
}

void	processcfg()
{
	//get global settings
    get_globaldebug();
	get_maintainer();
    get_notefile();
	get_idletimeout();
	get_waittimeout();
	get_maxuploadsize();

	//get the bots 
    lua_getglobal(L, "Bots");
    if(lua_istable(L, -1)){
		lua_pushnil(L);
		while(lua_next(L, -2) != 0){//la table est à l'index t
			parse_bot();
			lua_pop(L, 1); //enlève value, garde key pour l'itération suivante
		}
    }
    else
		printf("ERROR: Bots is not a table\n");
    lua_pop(L, 1); //pop Bots
}

void	readcfg()
{
	FILE	*infile;
	char	buf[255];
	int 	error;

	defaultbot = NULL;
	defaultset = NULL;
	linenum = 0;
	dbg_lvl = QUIET;

	/*
	if(STREQUAL(configfile, "-"))
		infile = stdin;
	else if((infile = fopen(configfile, "r")) == NULL)
	{
		printf("Configfile '%s' could not be read!\n",
			configfile);
		exit(1);
	}
	while(readline(infile, buf))
	{
		linenum++;
		parsecfg(buf);
	}
	fclose(infile);
	*/
	error = luaL_dofile(L, configfile);
	if(error){
		printf("erreur avec le fichier : %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	else
		processcfg();
}

void cleanupcfg()
{
	if(botmaintainer)
		free(botmaintainer);
	//perhaps not alloc'ed
	//if(notefile)
	//	free(notefile);
}
