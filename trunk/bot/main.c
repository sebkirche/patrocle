/*
 * VladBot  - servicebot for IRC.
 *
 * (c) 1993-94 VladDrac (irvdwijk@cs.vu.nl)
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
 */

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
//#include <sys/time.h>
#include <time.h>
#include <signal.h>
#ifdef AIX
#include <sys/select.h>
#endif /* AIX */
#include <unistd.h>
#ifndef __APPLE__
	#include <lua5.1/lua.h>
	#include <lua5.1/lualib.h>
	#include <lua5.1/lauxlib.h>
#else
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
#endif

#include "channel.h"
#include "config.h"
#include "cfgfile.h"
#include "debug.h"
#include "log.h"
#include "note.h"
#include "parse.h"
#include "send.h"
#include "session.h"
#include "userlist.h"
#include "vladbot.h"
#include "luainterface.h"

extern 	long	 uptime;	/* time when bot was started   */
extern 	char 	*configfile;
extern 	int 	rehash;
#ifdef NEXT
int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
int bzero(char *, int);
#endif /* NEXT */

/* TODO mettre un #ifdef ou #ifndef ici
int 	fork ();
int 	atoi (char *);
*/
void 	readcfg ();
void 	readlevelfiles ();

void 	sig_hup ();
void 	sig_cntl_c ();
void 	sig_bus ();
void 	sig_pipe ();
void 	sig_segv ();
void 	sig_alrm ();
int	alarme = 0;

void 	sig_hup ()
{
	globallog (ERRFILE, "REHASHING: Hangup (sighup) received");
	rehash = TRUE;
	signal (SIGHUP, sig_hup);
}

void 	sig_int ()
{
  	quit_all_bots (NULL, "Received SIGINT - Control-C!");
	cleanup_listsets();
  	globallog (ERRFILE, "SIGNING OFF: Control-C (sigint) received");
  	dump_notelist ();
	shutdown_lua();
	cleanupcfg();
  	exit (0);
}

void 	sig_bus ()
{
  	globallog (ERRFILE, "IGNORING: received Sigbus!");
  	signal (SIGBUS, sig_bus);
}

void 	sig_pipe ()
{
  	globallog (ERRFILE, "IGNORING: received Broken Pipe!");
  	signal (SIGPIPE, sig_pipe);
}

void 	sig_segv ()
{
  	quit_all_bots (NULL, "Received SIGSEGV - core dumped");
  	globallog (ERRFILE, "SIGNING OFF: Segmentation violation (sigsegv) received");
	botlog (LOGFILE, "SIGNING OFF: Received SIGSEGV - core dumped");
  	dump_notelist ();
	shutdown_lua();
	cleanupcfg();
  	exit (0);
}

void 	sig_alrm ()
{
	alarme = 1;
	sendprivmsg(currentchannel(),"C'est l'heure!!!!!");
	alarme = 0;
}

void 	bot_rehash ()
{
  	quit_all_bots (NULL, "Rehashing... brb");
  	readcfg ();
  	readlevelfiles ();
	load_lualogic(NULL);

  	start_all_bots ();
	
  	rehash = FALSE;
}

int start_bots ()
{
  	struct timeval timer;
  	fd_set rd, wd;

  /* The notelist survives rehashes so it has to be read only
	 at startup */
  	read_notelist ();
  	if (bot_init() != 0)
		exit(1);
  	/* set uptime */
  	uptime = time (NULL);
  	/* not really rehash, but read cfgfile etc. */
  	rehash = TRUE;

	while (1){
		if (rehash){
#ifdef DBUG
	  		globallog (ERRFILE, "start_bots: rehashing");
#endif
	  		bot_rehash ();
		}
		else{
#ifdef DBUG
	  		globallog (ERRFILE, "start_bots: reconnect");
#endif
	  		/* first, try to reconnect any dead sockets */
	  		try_reconnect ();
	  		send_pings ();		/* ping servers for activity 	*/
	  		reset_botstate ();	/* reset nickname and channels 	*/
		}
		timer.tv_sec = WAIT_SEC;
		timer.tv_usec = WAIT_MSEC;
		
		FD_ZERO (&rd);
		FD_ZERO (&wd);
		
		set_dcc_fds (&rd, &wd);
		set_server_fds (&rd, &wd);
		switch (select (NFDBITS, &rd, 0, 0, &timer)){
			case 0:
#ifdef DBUG
				global_dbg (NOTICE, "SELECT: timeout");
				break;
#endif
			case -1:
#ifdef DBUG
				global_dbg (ERROR, "SELECT: error");
#endif
				break;
			default:
				parse_server_input (&rd);
				parse_dcc_input (&rd);
				break;
		}
	}
}

int main (int argc, char *argv[])
{
  	char 	*arg;
  	char 	*myname;
  	int 	do_fork = FALSE;

  	myname = argv[0];

  	printf ("%s %s (c) 2009 Sébastien Kirche\n", myname, VERSION);
	printf ("Initial version by VladDrac\n");
	printf ("Portions for simulated AI by François Parmentier\n");
  	printf ("For more info: e-mail sebastien.kirche@free.fr or see http://sebastien.kirche.free.fr/patrocle/\n");

  	while (argc > 1 && argv[1][0] == '-'){
		argc--;
		arg = *++argv;
		switch (arg[1]){
			case 'h':
				printf ("usage: %s [switches [args]]\n", myname);
				printf ("-h               shows this help\n");
				printf ("-b               run %s in the background\n",
						myname);
				printf ("-f file          Read configfile 'file'\n");
#ifdef DBUG
				printf ("-d [0|1|2]	 Set debuglevel.\n");
				printf ("                   0 = debugging off\n");
				printf ("                   1 = show errors\n");
				printf ("                   2 = show detailed information\n");
#endif
				exit (0);
				break;
			case 'b':
				do_fork = TRUE;
				break;
			case 'f':
				++argv;
				if (!*argv){
	      			printf ("No configfile specified\n");
	      			exit (0);
	    		}
				configfile = *argv;
				argc--;
				printf ("Using cfg %s\n", configfile);
				break;
			case 'd':
#ifdef DBUG
				++argv;
				argc--;
				if (*argv && set_debuglvl (atoi (*argv)))
	    			printf ("Debuglevel set to %d\n", atoi (*argv));
				else{
	      			printf ("Invalid debugvalue!\n");
	      			exit (0);
	    		}
#else
				printf ("This version was not compiled with debugging enabled\n");
#endif
				break;
			default:
				printf ("Unknown option %s\n", arg);
				exit (1);
				break;
		}
	}

  	signal (SIGHUP, sig_hup);
  	signal (SIGINT, sig_int);
  	signal (SIGBUS, sig_bus);
  	signal (SIGPIPE, sig_pipe);
  	signal (SIGSEGV, sig_segv);

	if(init_lua()){
		globallog(ERRFILE, "cannot start lua interpreter");
		exit(1);
	}

	
  	if (do_fork){
		if (!fork ()){
			printf ("Running %s in background\n", myname);
			start_bots ();
		}
		else
			exit (0);
	}
  	else
		start_bots ();
	return 0;
}
