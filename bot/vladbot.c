/*
 VladBot  - servicebot for IRC.
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997, 1998 François Parmentier (H_I)
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
 
 
  This module deals with the creation, handling, deleting etc. of more
  than one bot. This module if NOT the main module and does not contain
  the main loop (see main.c) 
 
  WARNING: These functions should only be called from on_msg with great
           caution as wrong use can really mess things up (like changing
 	    currentbot etc.) Most of these functions should only be called
 	    from cfgfile.c and main.c (the "Creating" and "Maintaining"
 	    modules)
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/time.h>
#include <time.h>
#include <strings.h>
#include <string.h>
#include <stdarg.h>
#include <iconv.h>

#include "channel.h"
#include "config.h"
#include "dcc.h"
#include "debug.h"
#include "log.h"
#include "file.h"
#include "misc.h"
#include "parse.h"
#include "send.h"
#include "server.h"
#include "userlist.h"
#include "botlist.h"
#include "luainterface.h"
#include "vladbot.h"

//FIXME : weird, but fixes a link problem with my OSX
//#ifdef __APPLE__
//#define iconv_open iconv_open
//#define iconv iconv
//#define iconv_close iconv_close
//#endif

extern	void	signoff(const char *from, const char *reason);
extern	int	userlevel(const char *);
extern	int	shitlevel(const char *);
extern  short   logging;

long	uptime;
char	*botmaintainer = NULL;
char	*botmaintainername = NULL;
static	botinfo		*botlist[MAXBOTS];
static	listinfo	*listset[MAXBOTS];
botinfo 	*currentbot;
#ifdef LOG
short   logging = TRUE;
#else
short   logging = FALSE;
#endif /* LOG */
char    CommandChar = PREFIX_CHAR;

int	rehash = FALSE;	/* Set to true if rehash */

int	idletimeout = DCC_IDLETIMEOUT;
int	waittimeout = DCC_WAITTIMEOUT;
int	maxuploadsize = DCC_MAXFILESIZE;
char	*notefile = NOTEFILE;
int	number_of_bots = 0;

char *utf8 = "utf-8";
char *latin1 = "iso-8859-1";


int	find_channel(botinfo *bot, const char *channel)
/*
 * returns 1 if channel in bot, else 0
 */
{
	currentbot = bot;
	if(search_chan(channel))
		return 1;
	return 0;
}

int	add_channel_to_bot(botinfo *bot, const char *channel, const char *topic, const char *mode, const char *encoding)
/*
 * adds channel to bot, returns 0 if list full
 */
{
	currentbot = bot;
	return(join_channel(channel, topic, mode, encoding, FALSE));
}

int	find_server(botinfo *bot, const char *server, int port)
/*
 * returns 1 if server/port in bot, else 0
 */
{
	int	i;
	
	for(i=0; i<bot->num_of_servers; i++)
		if(STRCASEEQUAL(bot->serverlist[i].name, server) &&
		   (bot->serverlist[i].port == port))
			return 1;
	return 0;
}

int	add_server_to_bot(botinfo *bot, const char *server, int port)
/*
 * adds server to bot, returns 0 if list full
 */
{
	if(bot->num_of_servers == MAXSERVER)
		return 0;
	mstrcpy(&(bot->serverlist[bot->num_of_servers].name), server);
	mstrcpy(&(bot->serverlist[bot->num_of_servers].realname), server);
	bot->serverlist[bot->num_of_servers].port = port;
	bot->num_of_servers++;
	return 1;
}

int	delete_server_from_bot(botinfo *bot, const char *server, int port)
/*
 * Deletes server from list, returns zero if server not or last in list
 */
{
	int	i, j, found = FALSE;
	
	for(i=0; i<bot->num_of_servers; i++)
		if(STRCASEEQUAL(bot->serverlist[i].name, server) &&
		   (bot->serverlist[i].port == port))
			found = TRUE;
	/* We can't delete the last server */
	if(!found || bot->num_of_servers == 0)
		return 0;

	if(bot->current_server == i){
		close(bot->server_sock);
		bot->server_sock = -1;		/* reconnect */
		if(i == bot->num_of_servers - 1)
			bot->current_server--;
	}
	
	for(j=i; j < bot->num_of_servers-1; j++){
		free(bot->serverlist[j].name);
		mstrcpy(&bot->serverlist[j].name, bot->serverlist[j+1].name);
	}
	bot->num_of_servers--;
	return 1;
}
			
int	bot_init()
{
	int 	i;

	for(i=0; i<MAXBOTS; i++){
		botlist[i]=NULL;
		listset[i]=NULL;
	}
	return(0);
}

botinfo	*bot_created(const char *s)

{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(botlist[i] && STRCASEEQUAL(botlist[i]->botname, s))
			return botlist[i];
	return NULL;
}

botinfo	*add_bot(const char *s)
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(!botlist[i]){
			if((botlist[i] = 
				(botinfo *)malloc(sizeof(botinfo))) == NULL)
				return NULL;
			mstrcpy(&botlist[i]->botname, s);
			/* set some defaults. can be changed */
			strcpy(botlist[i]->nick, s);
			strcpy(botlist[i]->realnick, s);
			strcpy(botlist[i]->login, USERNAME);
			mstrcpy(&botlist[i]->name, IRCGECOS);
			mstrcpy(&botlist[i]->uploaddir, FILEUPLOADDIR);
			mstrcpy(&botlist[i]->downloaddir, FILEROOTDIR);
			mstrcpy(&botlist[i]->indexfile, CONTENTSFILE);
			mstrcpy(&botlist[i]->helpfile, HELPFILE);
			mstrcpy(&botlist[i]->stimfile, STIMFILE);
			mstrcpy(&botlist[i]->repfile, REPFILE);
			mstrcpy(&botlist[i]->botfile, BOTFILE);
			mstrcpy(&botlist[i]->parent, "NO PARENT");
			botlist[i]->uptime = time(NULL);
			botlist[i]->num_of_servers = 0;
			botlist[i]->current_server = 0;
			botlist[i]->Channel_list = NULL;
			botlist[i]->Client_list = NULL;
			botlist[i]->session_list = NULL;
			botlist[i]->server_sock = -1;
			botlist[i]->lastreset = time(NULL);
			botlist[i]->lastping = time(NULL);
			botlist[i]->server_ok = TRUE;
			botlist[i]->def_encoder = iconv_open(utf8, latin1); //to, from
			botlist[i]->def_decoder = iconv_open(latin1, utf8); //to, from
			botlist[i]->botlist = init_botlist();
			number_of_bots++;
			return botlist[i];
		}
	/* no more slots */
	return NULL;
}

listinfo *add_listset(const char *s)
/*
 * Adds listset to array and reads in the lists
 */
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(!listset[i]){
			if((listset[i] = (listinfo*)malloc(sizeof(listinfo))) == NULL)
				return NULL;
			mstrcpy(&listset[i]->listname, s);
			listset[i]->opperlist = init_levellist();
			listset[i]->protlist = init_levellist();
			listset[i]->shitlist = init_levellist();
			listset[i]->rellist = init_levellist();
/* 			listset[i]->botlist = init_botlist(); */
			listset[i]->ListeLocuteurs = InitLocuteurListe();
			mstrcpy(&listset[i]->opperfile, USERFILE);
			mstrcpy(&listset[i]->shitfile, SHITFILE);
			mstrcpy(&listset[i]->protfile, PROTFILE);
			mstrcpy(&listset[i]->relfile,  RELFILE);
/* 			mstrcpy(&listset[i]->botfile,  BOTFILE); */
			mstrcpy(&listset[i]->locuteurfile, LOCUTEURFILE);
			return listset[i];
		}
	return NULL;
}

listinfo	*listset_created(const char *s)
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(listset[i] && STRCASEEQUAL(listset[i]->listname, s))
			return listset[i];
	return NULL;
}

void	free_listset(listinfo *elem)
{
	if(elem){
		free(elem->listname);
		free(elem->opperfile);	
		free(elem->shitfile);	
		free(elem->protfile);
		free(elem->relfile);
		/*free(elem->botfile); */
		free(elem->locuteurfile);
		delete_levellist(elem->opperlist);
		delete_levellist(elem->protlist);
		delete_levellist(elem->shitlist);
		delete_levellist(elem->rellist);
		/*delete_botlist(elem->botlist); */
		DetruitListe(elem->ListeLocuteurs);
		free(elem);
		elem = NULL;
	}
}

void	delete_listset(const char *s)
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(listset[i] && STRCASEEQUAL(listset[i]->listname, s))
			free_listset(listset[i]);
}

void	readlevelfiles()
/*
 * Loops through all listsets and reads in the levelfiles
 */
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(listset[i]){
			readuserdatabase(listset[i]->opperfile, 
							 listset[i]->opperlist);
			readuserdatabase(listset[i]->protfile, 
							 listset[i]->protlist);
			readuserdatabase(listset[i]->shitfile, 
							 listset[i]->shitlist);
			readuserdatabase(listset[i]->relfile,
							 listset[i]->rellist);
/* 			readbotdatabase(listset[i]->botfile, */
/* 					 listset[i]->botlist); */
			ChargeLocuteurs(listset[i]->locuteurfile,
							 listset[i]->ListeLocuteurs);
		}
}

int	forkbot(const char *nick, const char *login, const char *name)
/*
 * Adds bot to list and starts it.
 * except for nick, login and name the bot is an identical
 * copy of its "parent"
 */
{
	botinfo	*newbot;
	int	i;
	char	botname[10];
	char	smallnick[6];
	CHAN_list	*c_list;

	/* Try to create a unique botname */
	strncpy(smallnick, nick, 5);
	smallnick[5] = '\0';
	sprintf(botname, "%s%d", smallnick, (int)(rand() % 10000));

	if((newbot = add_bot(botname))){
		strncpy(newbot->nick, nick, NICKLEN);
		strncpy(newbot->realnick, nick, NICKLEN);
		strncpy(newbot->login, login?login:currentbot->login, NICKLEN);
		mstrcpy(&newbot->name, name?name:currentbot->name);
		mstrcpy(&newbot->parent, currentbot->botname);
		for(i=0; i<currentbot->num_of_servers; i++)
			add_server_to_bot(newbot, 
                        currentbot->serverlist[i].name,
		        currentbot->serverlist[i].port);		
		newbot->lists = currentbot->lists;
		c_list = currentbot->Channel_list;
		currentbot = newbot;
		copy_channellist(c_list);
		connect_to_server();
		return 1;
	}
	return 0;
}

int	killbot(const char *reason)
/*
 * Kills bot current and goes to next bot.
 * If not bots are left, the program has to quit
 * (the calling funcion should check this!)
 */
{
	int	i;
	int	botnum = 0;

	/* first find bot's slot */
	for(i=0; i<MAXBOTS; i++)
		if(botlist[i] && STRCASEEQUAL(botlist[i]->botname, 
		   currentbot->botname))
			botnum = i;

	/* quit should be send by calling function */
	close_all_dcc();
	delete_all_channels();
	if(currentbot->server_sock != -1){
		sendquit(reason);
		close(currentbot->server_sock);
		if(currentbot->serverlist[currentbot->current_server].realname)
			free(currentbot->serverlist[currentbot->current_server].realname);
		if(currentbot->serverlist[currentbot->current_server].name)
			free(currentbot->serverlist[currentbot->current_server].name);
	}
	free(currentbot->botname);
	free(currentbot->name);
	free(currentbot->uploaddir);
	free(currentbot->downloaddir);
	free(currentbot->indexfile);
	free(currentbot->helpfile);
	free(currentbot->stimfile);
	free(currentbot->repfile);
	free(currentbot->botfile);
	free(currentbot->parent);
	iconv_close(currentbot->def_encoder);
	iconv_close(currentbot->def_decoder);
	free(currentbot);
	botlist[botnum] = NULL;

	/* find a bot.. doesn't matter which one.. just to be sure */
	number_of_bots--;
	for(i=0; i<MAXBOTS; i++)
		if(botlist[i]){
			currentbot=botlist[i];
			return 1;
		}
	/* No bot found! */
	return 0;	
}

void	cleanup_listsets()
{
	int i;

	for(i=0; i<MAXBOTS; i++){
		if(listset[i]){
			free_listset(listset[i]);
		}
		listset[i]=NULL;
	}
}
void	quit_all_bots(const char *from, const char *reason)
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(botlist[i]){
			currentbot = botlist[i];
			signoff(from, reason);
		}
	cleanup_listsets();
}

void	start_all_bots()
/*
 * Starts bot "current" (global var)
 */
{
	int	i;

	for(i=0; i<MAXBOTS; i++){
		if(botlist[i]){
			currentbot = botlist[i];
			/* don't use while(!..) anymore,
			   if one bot hangs, they all hang! */
			connect_to_server();
		}
	}
}

int	connect_to_server()
/*
 * connects to a server, and returns -1 if it fails.
 */
{
#ifdef DBUG
	debug(LVL_NOTICE, "connect_to_server(): Connecting to port %d of server %s",
		  currentbot->serverlist[currentbot->current_server].port, 
		  currentbot->serverlist[currentbot->current_server].name);
#endif
	if((currentbot->server_sock = connect_by_number(
													currentbot->serverlist[currentbot->current_server].port, 
													currentbot->serverlist[currentbot->current_server].name)) < 0){
		close(currentbot->server_sock);
		currentbot->server_sock = -1;
		return 0;
	}

	sendregister(currentbot->nick, currentbot->login, currentbot->name);
	/* channels will be joined ("reset") when we receive a '001' */
	return(TRUE);
}

void	reconnect_to_server()
/*
 * connects to a server (i.e. after a kill) and rejoins all channels
 * in the channellist. 
 */
{
	if(currentbot->server_sock != -1)
		close(currentbot->server_sock);
	/* Let try_reconnect do the work */
	currentbot->server_sock = -1;
}

void	set_server_fds( fd_set *reads, fd_set *writes )
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(botlist[i] && botlist[i]->server_sock != -1)
			FD_SET( botlist[i]->server_sock, reads );
}

int 	readln( char *buf )

{
	return(read_from_socket( currentbot->server_sock, buf ));
}

int	send_to_server(char *format, ...)
{
	va_list	msg;
	char	buf[WAYTOBIG];

	va_start(msg, format);
	vsprintf(buf, format, msg);
	va_end(msg);
	debug(LVL_DEBUG, "send_to_server(): \"%s\"", buf);
	//printf("send_to_server : %s\n", buf);
	return(send_to_socket(currentbot->server_sock, "%s", buf));
}

int	change_server(int serverno)
{
	char	reason[MAXLEN];

	if(serverno < 0 || serverno > currentbot->num_of_servers - 1)
		return FALSE;
	
	sprintf(reason, "Changing servers. Connecting to %s",
	        currentbot->serverlist[serverno].name);
	sendquit(reason);
	if(currentbot->server_sock != -1){
		close(currentbot->server_sock);
		currentbot->server_sock = -1;
	}
	currentbot->current_server = serverno;
	connect_to_server();
	/* assume server is ok */
	currentbot->lastping = time(NULL);
	currentbot->server_ok = TRUE;
	return TRUE;
}

void	try_reconnect()
/*
 * check for every bot if the socket is still connected (i.e. not -1) 
 * and if a ping has been received lately. If not, automatically go
 * to the next server
 */
{
	int	i;
	char	reason[MAXLEN];

	/*
	   Check for every slot if it contains a bot. If so, reconnect if
	   the socket = -1 or the server hasn't responded to a ping 
    	 */
	for(i=0; i<MAXBOTS; i++)
		if(botlist[i] && 
		   ((botlist[i]->server_sock == -1) || 
			(((time(NULL) - botlist[i]->lastping) > PINGINTERVAL) &&
			 not(botlist[i]->server_ok)))){
			currentbot = botlist[i];
#ifdef DBUG
			debug(LVL_ERROR, "try_reconnect(): Server %s not\
 responding, closing connection",
				  currentbot->serverlist[currentbot->
										 current_server].name);
#endif
			/* Make sure we'll connect to another server */
			sprintf(reason, "Changing servers. Connecting to %s",
					currentbot->serverlist[currentbot->current_server].name);
			sendquit(reason);
			if(currentbot->server_sock != -1)
				close(currentbot->server_sock);
			if(currentbot->current_server < currentbot->num_of_servers-1)
				currentbot->current_server++;
			else
				currentbot->current_server = 0;
			connect_to_server();
			/* assume server is ok */
			currentbot->lastping = time(NULL);
			currentbot->server_ok = TRUE;
		}	
}

void	reset_botstate()
/*
 * reset nickname to realnick and join all channels 
 * the bot might have been kicked off.
 */
{
	int	i;

	for(i=0; i<MAXBOTS; i++)
		if(botlist[i])
			if(time(NULL)-botlist[i]->lastreset > RESETINTERVAL){
#ifdef DBUG
				debug(LVL_DEBUG, "Resetting botstate");
#endif
				currentbot = botlist[i];
				currentbot->lastreset = time(NULL);
				reset_channels(SOFTRESET);
				cleanup_sessions();
				if(!STRCASEEQUAL(currentbot->nick,
								 currentbot->realnick)){
					strcpy(currentbot->nick,
						   currentbot->realnick);
					sendnick(currentbot->nick);
				}
			}
}

void	parse_server_input( fd_set *read_fds )
/*
 * If read_fds is ready, this routine will read a line from the 
 * the server and parse it.
 */
{
	char	line[MAXLEN];
	int	i;

	for(i=0; i<MAXBOTS; i++){
		if(botlist[i] && botlist[i]->server_sock != -1){
			currentbot = botlist[i];
			if( FD_ISSET( currentbot->server_sock, read_fds ) ){
				if( readln( line ) > 0 )
					parseline( line );
				else{
#ifdef DBUG
					debug(LVL_ERROR, "parse_server_input(): Server read FAILED!");
#endif
					close(currentbot->server_sock);
					currentbot->server_sock = -1;
				}
			}
		}
	}
}

void	set_dcc_fds( fd_set *read_fds, fd_set *write_fds )
{
	int 	i;

	for(i=0; i<MAXBOTS; i++){
		if(botlist[i]){
			currentbot = botlist[i];
			dccset_fds(read_fds, write_fds);
		}
	}
}

void	parse_dcc_input( fd_set *read_fds )
/*
 * Scan all bots for dcc input
 */
{
	int	i;

	for(i=0; i<MAXBOTS; i++){
		if(botlist[i]){
			currentbot = botlist[i];
			parse_dcc(read_fds);
		}
	}
}

void	send_pings()
/*
 * Send a ping to all servers and set lastping to NOW
 */
{
        int     i;

        for(i=0; i<MAXBOTS; i++){
			if(botlist[i]){
				currentbot = botlist[i];
				if(time(NULL) - currentbot->lastping/*send*/ > 
				   PINGSENDINTERVAL){
#ifdef DBUG
					debug(LVL_NOTICE, 
						  "send_ping(): Sending ping to server %s[%s]",
						  currentbot->serverlist[currentbot->current_server].realname,
	         	            currentbot->serverlist[currentbot->current_server].name);
#endif
					sendping(currentbot->serverlist[currentbot->current_server].realname);
					currentbot->server_ok = FALSE;
					currentbot->lastping = time(NULL);
				}
			}
		}
}

void	pong_received(char *nick, char *server)
/*
 * Pong received from currentbot->serverlist[currentbot->current_server]
 * Server must be ok.
 */
{
	currentbot->server_ok = TRUE;
#ifdef DBUG
	debug(LVL_NOTICE, "pong_received(): server %s[%s] ok",
		  currentbot->serverlist[currentbot->current_server].realname,
		  currentbot->serverlist[currentbot->current_server].name);
#endif
}


/* Global commands */
struct
{
	char	*name;
	void	(*function)(const char*, const char*);
	int	userlevel;
	int	forcedcc;
} global_cmds[] = {	
	{ "LIST",	global_list,	0,	TRUE	},
	{ "INFO",	global_list,	0,	TRUE	},
	{ "DIE",	global_die,	USERLVL_BOTOWNER,	FALSE	},
	{ "DEBUG",	global_debug,	USERLVL_BOTOWNER,	FALSE	},
	{ NULL,		null(void(*)())	}
};

void	parse_global(const char *from, const char *to, const char *rest)
{
	int	i;
	char	*command;
	DCC_list	*dccclient;
	
	if(!(command = get_token(&rest, " "))){
		send_to_user(from, "Global expects a subcommand!");
		return;
	}

	for(i=0; global_cmds[i].name; i++)
		if(STRCASEEQUAL(global_cmds[i].name, command)){
			if(userlevel(from) < global_cmds[i].userlevel){
				send_to_user(from, "Userlevel too low");
				return;
			}
			if(shitlevel(from) > 0){
				send_to_user(from, "Shitlevel too high");
				return;
			}
			dccclient = search_list("chat", from, DCC_CHAT);
  		        if(global_cmds[i].forcedcc && 
                          (!dccclient || (dccclient->flags&DCC_WAIT))){
#ifdef AUTO_DCC
					dcc_chat(from, rest);
					sendnotice(getnick(from), "Please type: /dcc chat %s",
							   currentbot->nick);
					sendnotice(getnick(from), 
							   "After that, please retype: /msg %s GLOBAL %s %s", 
							   currentbot->nick, command, 
							   rest?rest:"");
#else
					sendnotice(getnick(from), 
							   "Sorry, GLOBAL %s is only available through DCC",
							   command);
					sendnotice(getnick(from), 
							   "Please start a dcc chat connection first");
#endif
					return;
				}
				global_cmds[i].function(from, *rest?rest:NULL);
				return;
		}
	send_to_user(from, "Unknown global command");
	return;
}

void	global_not_imp(const char *from, const char *rest)
{
	send_to_user(from, "Global command not implemented");
}

void	global_debug(const char *from, const char *rest)
{
#ifdef DBUG
	if(!rest){
		send_to_user(from, "Pls specify a level between 0 and %d", MAX_LVL);
		return;
	}
	if(userlevel(from)>=USERLVL_BOTOWNER)
		if(set_debuglvl(atoi(rest)))
			send_to_user(from, "Debug set to debuglevel %d", atoi(rest));
		else
			send_to_user(from, "Could not set debuglevel");
	else
		send_to_user(from, "Sorry, only bot-operator can change debuglevel");
#else
	send_to_user(from, "This version was not compiled with debugging enabled");
#endif
}

void	global_die(const char *from, const char *rest)
{
	botinfo	*bot;

	if(userlevel(from) >= USERLVL_BOTOWNER)
		if(rest){
			if((bot = bot_created(rest))){
				currentbot = bot;
				signoff(from, "Global kill");
				if(number_of_bots == 0)
					exit(0);
			}
			else
				send_to_user(from, "No such bot");
		}
		else{
			quit_all_bots(from, "Global die");
			exit(0);
		}
	else
		send_to_user(from, "Sorry, Incorrect levels");
}	

void	global_list(const char *from, const char *rest)
{
	int		i;
	CHAN_list	*chan;
	botinfo		*bot;

	SKIPSPC(rest);
	if(rest && (bot = bot_created(rest))){
		send_to_user(from, "\002Botname\002 %s \002nick\002 %s \002realnick\002 %s",
					 bot->botname, bot->nick, bot->realnick);
		send_to_user(from, "\002Parent:\002 %s", bot->parent);
		send_to_user(from, "%d servers in list:", bot->num_of_servers);
		for(i=0; i<bot->num_of_servers; i++){
			send_to_user(from, "\002server\002 %s[%s] \002port\002 %d%s",
						 bot->serverlist[i].realname, bot->serverlist[i].name,
						 bot->serverlist[i].port, i==bot->current_server?", current":"");
		}
		/* show_channellist(from); won't work. It'll show the channellist
		   of currentbot. And setting currentbot to botlist[i] won't work
		   either, the server_sock for sending will point to the wrong
		   server. */
		if(!bot->Channel_list)
			send_to_user(from, "Not active on any channels!");
		else
			for(chan = bot->Channel_list; chan; chan = chan->next)
				send_to_user(from, "\002Active on:\002 %s", chan->name);
		send_to_user(from, "\002helpfile:\002    %s", bot->helpfile);
		send_to_user(from, "\002uploaddir:\002   %s", bot->uploaddir);
		send_to_user(from, "\002downloaddir:\002 %s", bot->downloaddir);
		send_to_user(from, "\002indexfile:\002   %s", bot->indexfile);
		send_to_user(from, "\002log:\002         %s", (logging?"on":"off"));
		return;
	}
	/* else */
	send_to_user(from, "Total number:  %d", MAXBOTS);
	send_to_user(from, "Total running: %d", number_of_bots);
	send_to_user(from, "Total free:    %d", MAXBOTS-number_of_bots);
	send_to_user(from, "Started:       %-20.20s",
				 time2str(uptime));
	send_to_user(from, "Uptime:        %-30s", 
				 idle2str(time(NULL)-uptime));
	send_to_user(from, "Log:           %s", (logging?"on":"off"));
	send_to_user(from, " ");
	send_to_user(from, "Num botname              nickname  current server");
	for(i=0; i<MAXBOTS; i++)
		if(botlist[i])
			send_to_user(from, "%2d: %-20s %-9s %s", i, 
						 botlist[i]->botname, botlist[i]->nick,
						 botlist[i]->serverlist[botlist[i]->current_server].realname);
		else
			send_to_user(from, "%2d: Free", i);
}
						   
// Local variables:
// coding: utf-8
// end:
