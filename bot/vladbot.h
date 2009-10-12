/*
 VladBot  - servicebot for IRC.
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997, 1998 François Parmentier (H_I)
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
 
 
 This module deals with the creation, handling, deleting etc. of more
 than one bot. This module if NOT the main module and does not contain
 the main loop (see main.c) 
 
 WARNING: These functions should only be called from on_msg with great
 caution as wrong use can really mess things up (like changing
 currentbot etc.) Most of these functions should only be called
 from cfgfile.c and main.c (the "Creating" and "Maintaining"
 modules)
 */
#ifndef VLADBOT_H
#define VLADBOT_H

#include <iconv.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#include "session.h"
#include "userlist.h"
#include "botlist.h"
#include "locuteur.h"
#include "channel.h"
#include "dcc.h"
#include "config.h"

struct	liststruct
{
	char		*listname;
	char		*opperfile;
	char		*protfile;
	char		*shitfile;
	char		*relfile;
/*         char            *botfile; */
	char		*locuteurfile;
	USERLVL_list	**opperlist;
	USERLVL_list	**protlist;
	USERLVL_list	**shitlist;
	USERLVL_list    **rellist;
/*         BOT_list        **botlist; */
	locuteur        **ListeLocuteurs;
};

typedef	struct liststruct	listinfo;

struct	botstruct	
{
	char		*botname; 		/* to identify bot */
	char		nick[MAXNICKLEN];	/* nick on IRC */
	char		realnick[MAXNICKLEN];	/* what nick should be */
	char		login[MAXNICKLEN];	/* desired login */
	char		*name;			/* "IRCNAME" */

	int		server_sock;	
	
	int		num_of_servers;
	int		current_server;
	struct
	{
		char	*name;
		char	*realname;	/* server might have a different name */
		int	port;
	} serverlist[MAXSERVER];

	CHAN_list	*Channel_list;
	CHAN_list	*Current_chan;	/* Pointer to current channel */
	DCC_list	*Client_list;
	SESSION_list	*session_list;
        

	char		*helpfile;
	char		*notefile;
	char		*uploaddir;
	char		*downloaddir;
	char		*indexfile;
	listinfo	*lists;

	long		uptime;
	long		lastreset;	/* last time bot was reset */
	long		lastping;	/* to check activity server */
	int		server_ok;
	
	char		*parent;	/* Not really needed, but nice to know */
	char		*stimfile;
	char		*repfile;
	char		*botfile;
	iconv_t		def_encoder;
	iconv_t		def_decoder;

	BOT_list **botlist;
};


typedef struct botstruct	botinfo;	

int	find_channel(botinfo *bot, char *channel);
int	add_channel_to_bot(botinfo *bot, char *channel, char *topic, char *mode, char *encoding);
int	find_server(botinfo *bot, char *server, int port);
int	add_server_to_bot(botinfo *bot, char *server, int port);
int	delete_server_from_bot(botinfo *bot, char *server, int port);
int	bot_init();
botinfo	*bot_created(char *s);
botinfo	*add_bot(char *s);
listinfo	*listset_created(char *s);
listinfo	*add_listset(char *s);
void	delete_listset(char *s);
void    readlevelfiles();
int	forkbot(char *nick, char *login, char *name);
int	killbot(char *reason);
void	cleanup_listsets();
void	quit_all_bots(char *from, char *reason);
void	start_all_bots();
int	connect_to_server();
void	reconnect_to_server();
void	set_server_fds( fd_set *reads, fd_set *writes );
int 	readln( char *buf );
int	send_to_server(char *format, ...);
int     change_server(int serverno);
void	try_reconnect();
void	reset_botstate();
void	parse_server_input( fd_set *read_fds );
void	set_dcc_fds( fd_set *read_fds, fd_set *write_fds );
void	parse_dcc_input( fd_set *read_fds );
void	send_pings();
void	pong_received(char *nick, char *server);

void	parse_global(char *from, char *to, char *rest);
void	global_not_imp(char *from, char *rest);
void	global_die(char *from, char *rest);
void	global_debug(char *from, char *rest);
void	global_list(char *from, char *rest);

#endif
// Local variables:
// coding: utf-8
// end:
