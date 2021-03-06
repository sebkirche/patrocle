/*
 vlad-ons.c - kinda like /on ^.. in ircII
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997, 1998 François Parmentier
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
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <iconv.h>

#define _GNU_SOURCE
#include "fnmatch.h"
//#include <fnmatch.h>
#include "botlist.h"
#include "channel.h"
#include "cfgfile.h"
#include "config.h"
#include "dcc.h"
#include "debug.h"
#include "ftext.h"
#include "ftp_dcc.h"
#include "log.h"
#include "misc.h"
#include "note.h"
#include "parsing.h"
#include "parsetime.h"
#include "send.h"
#include "session.h"
#include "userlist.h"
#include "botlist.h"
#include "locuteur.h"
#include "vladbot.h"
#include "vlad-ons.h"
#include "phrase.h"
#include "luainterface.h"

//FIXME : weird, but fixes a link problem with my OSX
//#ifdef __APPLE__
//#define iconv_open iconv_open
//#define iconv iconv
//#define iconv_close iconv_close
//#endif

extern	botinfo	*currentbot;
extern	int	number_of_bots;
extern	int	rehash;
extern	long	uptime;
extern	char	*botmaintainer;
extern	char	*botmaintainername;
extern  short   logging;
extern  char    CommandChar;
extern char	*months[];

/* external parseroutines ("prefix commands")	*/
extern	void	parse_note(const char *from, const char *to, const char *s);
extern	void	parse_global(const char *from, const char *to, const char *rest);
extern void	sig_alrm();

int GNumPhrase = 0;	/* counter for the number of processed reactions */

command_tbl on_msg_commands[] =

/* Any command which produces more than 2 lines output can be 
   used to make the bot "excess flood". Make it "forcedcc"     */

/*  Command	function   userlvl    shitlvl   rellvl      forcedcc */
	{
		{ "HELP",		show_help,		USERLVL_ANONYMOUS,	100,	-100,			TRUE  },		
		{ "APPRENDS",	do_apprends,    USERLVL_ANONYMOUS,  100,	CONFIDENCE_LVL,	FALSE },		
		{ "DESACTIVE",	do_desactive,   USERLVL_ANONYMOUS,  100,    CONFIDENCE_LVL,	FALSE },		
		{ "ACTIVE",		do_active,      USERLVL_ANONYMOUS,  100,    CONFIDENCE_LVL,	FALSE },		
		{ "FUCK",		do_fuck,        USERLVL_ANONYMOUS,  0,      SYMPA_LVL,		FALSE },		
		{ "WHOAMI",		show_whoami,  	USERLVL_ANONYMOUS,	100,	-100,			FALSE },		
		{ "INFO",		show_info,		USERLVL_ANONYMOUS,	100,	-50,			FALSE },		
		{ "TIME",		show_time,		USERLVL_ANONYMOUS,	100,	-10,			FALSE },		
		/* { "NRELADD", do_nreladd,     USERLVL_ANONYMOUS,  0,      -500,			FALSE },		*/
		{ "LISTDCC",	do_listdcc,		USERLVL_ANONYMOUS,	100,	0,				TRUE  },		
		{ "CHAT",		do_chat,		USERLVL_ANONYMOUS,	100,	0,				FALSE },		
		/* { "SEND",	do_send,		USERLVL_ANONYMOUS,	100,	-10,			FALSE },		*/
		/* These two are just aliases for ftp-dcc		 */
		/* { "GET",		do_send,		USERLVL_ANONYMOUS,	100,	-10,			FALSE },		*/
		/* { "MGET",	do_send,		USERLVL_ANONYMOUS,	100,	-10,			FALSE },		*/
		/* Some more ftp-dcc functions				 */
		/* { "LS",		show_dir,		USERLVL_ANONYMOUS,	100,	-10,			TRUE  },		*/
		/* { "PWD",		show_cwd,		USERLVL_ANONYMOUS,	100,	-10,			FALSE },		*/
		/* { "CD",		do_cd,			USERLVL_ANONYMOUS,	100,	-10,			FALSE },		*/
		/* { "QUEUE",	show_queue,		USERLVL_ANONYMOUS,	100,	-10,			TRUE  },		*/
		/* { "FILES",	do_flist,       USERLVL_ANONYMOUS,  100,    -10,			TRUE  },		*/
		/* { "FLIST",	do_flist,		USERLVL_ANONYMOUS,	100,	-10,			TRUE  },		*/
		/* userlevel and dcc are dealt with within NOTE 	*/
		{ "NOTE",		parse_note,		USERLVL_ANONYMOUS,	100,	-10,			FALSE /*FIXME: should be TRUE*/  },
		/* userlevel and dcc are dealt with within GLOBAL 	*/
		{ "GLOBAL",		parse_global,	USERLVL_ANONYMOUS,	100,	CONFIDENCE_LVL,	FALSE },		
		{ "MYCMDS",		show_mycmds,    USERLVL_ANONYMOUS,  100,    -10,			TRUE  },		
		{ "STIMLIST",	do_stimlist,    USERLVL_REMOTE,     100,    SYMPA_LVL,		TRUE  },		
		{ "REPLIST",	do_replist,     USERLVL_REMOTE,     100,    SYMPA_LVL,		TRUE  },		
		{ "STIMDEL",	do_stimdel,     USERLVL_REMOTE,     0,      SYMPA_LVL,		FALSE },		
		{ "REPDEL",		do_repdel,      USERLVL_REMOTE,     0,      SYMPA_LVL,		FALSE },		
		{ "CMDSLVLS",	show_cmdlvls,   USERLVL_REMOTE,     100,    SYMPA_LVL,		TRUE  },		
		{ "INVITE",		do_invite,		USERLVL_REMOTE,		0,		SYMPA_LVL,		FALSE },		
		{ "NWHOIS",		show_nwhois,	USERLVL_REMOTE,		0,		SYMPA_LVL,		FALSE },		
		{ "RELADD",		do_reladd,      USERLVL_REMOTE,     0,      SYMPA_LVL,		FALSE },		
		{ "SAY",		do_say,			USERLVL_REMOTE,		0,		CONFIDENCE_LVL,	FALSE },		
		{ "ME",			do_me,          USERLVL_REMOTE,     0,      CONFIDENCE_LVL,	FALSE },		
		{ "TOPIC",		do_topic,       USERLVL_REMOTE,     0,      SYMPA_LVL,		FALSE },		
		{ "SEEN",		do_seen,		USERLVL_ANONYMOUS,	0,		SYMPA_LVL,		FALSE },		
		{ "USERLIST",	show_userlist,	USERLVL_MODLIST,	100,	0,				TRUE  },		
		{ "SHITLIST",	show_shitlist,	USERLVL_MODLIST,	100,	SYMPA_LVL,		TRUE  },		
		{ "PROTLIST",	show_protlist,	USERLVL_MODLIST,	100,	CONFIDENCE_LVL,	TRUE  },		
		{ "RELLIST",	show_rellist,   USERLVL_MODLIST,    0,      CONFIDENCE_LVL,	TRUE  },		
		{ "OP",			do_op,			USERLVL_MODLIST,	0,		SYMPA_LVL,		FALSE },		
		{ "DEOP",		do_deop,		USERLVL_MODLIST,	0,		SYMPA_LVL,		FALSE },		
		{ "WHOIS",		show_whois,		USERLVL_MODLIST,	0,		SYMPA_LVL,		FALSE },		
		{ "USERWRITE",	do_userwrite,	USERLVL_MODLIST,	0,		0,				FALSE },		
		{ "RELWRITE",	do_relwrite,    USERLVL_MODLIST,    0,      0,				FALSE },		
		{ "RELDEL",		do_reldel,      USERLVL_MODLIST,    0,      SYMPA_LVL,		FALSE },		
		{ "SHOWUSERS",	do_showusers,	USERLVL_MODLIST,	0,		SYMPA_LVL,		TRUE  },		
		{ "STIMWRITE",	do_stimwrite,   USERLVL_MODLIST,    0,      CONFIDENCE_LVL,	FALSE },		
		{ "REPWRITE",	do_repwrite,    USERLVL_MODLIST,    0,      CONFIDENCE_LVL,	FALSE },		
		{ "KICK",		do_kick,		USERLVL_MODLIST,	0,		SYMPA_LVL,		FALSE },		
		{ "GIVEOP",		do_giveop,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "NUSERADD",	do_nuseradd,	USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "LOCWRITE",	do_locwrite,    USERLVL_MODLISTPOW, 0,      SYMPA_LVL,		FALSE },		
		{ "USERDEL",	do_userdel,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "NSHITADD",	do_nshitadd,	USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "SHITWRITE",	do_shitwrite,	USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "SHITDEL",	do_shitdel,		USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "PROTWRITE",	do_protwrite,   USERLVL_MODLISTPOW, 0,		SYMPA_LVL,		FALSE },		
		{ "PROTDEL",	do_protdel,     USERLVL_MODLISTPOW, 0,		CONFIDENCE_LVL,	FALSE },		
		{ "BANUSER",	do_banuser,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "UNBAN",		do_unban,		USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "OPEN",		do_open,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "LEAVE",		do_leave,		USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "NICK",		do_nick,		USERLVL_MODLISTPOW,	0,		SYMPA_LVL,		FALSE },		
		{ "REPLOAD",	do_repload,     USERLVL_MODLISTPOW, 0,      CONFIDENCE_LVL,	FALSE },		
		{ "USERADD",	do_useradd,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "LOGON",		do_logon,       USERLVL_MODLISTPOW, 0,      CONFIDENCE_LVL,	FALSE },		
		{ "LOGOFF",		do_logoff,      USERLVL_MODLISTPOW, 0,      CONFIDENCE_LVL,	FALSE },		
		{ "JOIN",		do_join,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "SHITADD",	do_shitadd,		USERLVL_MODLISTPOW,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "SHOWLOCS",	do_showlocs,    USERLVL_POWERUSER,  0,      SYMPA_LVL,		TRUE  },		
		{ "OUBLIE",		do_oublie,      USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "STIMLOAD",	do_stimload,    USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "PROTADD",	do_protadd,     USERLVL_POWERUSER,  0,		CONFIDENCE_LVL,	FALSE },		
		{ "NPROTADD",	do_nprotadd,    USERLVL_POWERUSER,  0,		SYMPA_LVL,		FALSE },		
		{ "CHANNELS",	show_channels,	USERLVL_POWERUSER,	0,		SYMPA_LVL,		FALSE },		
		{ "MASSOP",		do_massop,		USERLVL_POWERUSER,	0,		SYMPA_LVL,		FALSE },		
		{ "MASSDEOP",	do_massdeop,	USERLVL_POWERUSER,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "MASSKICK",	do_masskick,	USERLVL_POWERUSER,	0,		CONFIDENCE_LVL,	FALSE },		
		{ "MASSUNBAN",	do_massunban,	USERLVL_POWERUSER,	0,		SYMPA_LVL,		FALSE },		
		{ "SERVER",		do_server,		USERLVL_POWERUSER,	0,		SYMPA_LVL,		FALSE },		
		{ "MSGLOGON",	do_msglogon,    USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "MSGLOGOFF",	do_msglogoff,   USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "COMCHAR",	do_comchar,     USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "BOTLOAD",	do_botload,     USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "BOTLIST",	do_showbots,    USERLVL_POWERUSER,  0,      SYMPA_LVL,		TRUE  },		
		{ "BOTADD",		do_botadd,      USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "BOTDEL",		do_botdel,      USERLVL_POWERUSER,  0,      CONFIDENCE_LVL,	FALSE },		
		{ "BOTWRITE",	do_botwrite,    USERLVL_POWERUSER,  0,      SYMPA_LVL,		FALSE },		
		/* Priviliged commands					*/	
		{ "FORK",		do_fork,		USERLVL_BOTOWNER,	0,		-100,			FALSE },		
		{ "REHASH",		do_rehash,		USERLVL_BOTOWNER,	0,		-100,			FALSE },		
		{ "RELOADLOGIC",do_reloadlogic,	USERLVL_BOTOWNER,	0,		-100,			FALSE },		
		{ "DO",			do_do,			USERLVL_BOTOWNER,	0,		-100,			FALSE },		
		{ "DIE",		do_die,			USERLVL_BOTOWNER,	0,		-100,			FALSE },		
		{ "DIEDIE",		do_die,			USERLVL_BOTOWNER,	0,		-100,			FALSE },		
		{ "QUIT",		do_quit,		USERLVL_BOTOWNER,	0,		-100,			FALSE },		
#ifndef WIN32
		{ "ALARM",		do_alarm,		USERLVL_REMOTE,		0,		SYMPA_LVL,		FALSE },
#endif
		/*
		  :
		  :
	    */
		{ NULL,		null(void(*)()), 0,	0,	0,            FALSE }
};


/* Don't forget to delete the string returned. */
char *UserStr(const char *nuh) {
  /* nuh = nick!user@host */
  char *NUH; 
  char *ret;
  char *nick;
  char *user;
  char *host;
  char *domain;

  NUH = strdup(nuh);
  ret = malloc(MAXLEN*sizeof(char));

  nick = get_token(&NUH, "!");
  user = get_token(&NUH, "@");
  if(*user == '~' || *user == '#') user++;
  host = get_token(&NUH, ".");
  domain = get_token(&NUH, "");

  sprintf(ret, "*!*%s@*%s", user, domain?domain:host);
  return ret;
}

/* Don't forget to delete the string returned. */
char *NickUserStr(const char *nuh) {
  /* nuh = nick!user@host */
  char *NUH, *pNUH; 
  char *ret;
  char *nick;
  char *user;
  char *host;
  char *domain;
  char i;
  char *biz;
  int numbers = FALSE;

  NUH = strdup(nuh);
  pNUH = NUH;
  ret = malloc(MAXLEN*sizeof(char));

  nick = get_token(&NUH, "!");
  
  while((biz = strchr(nick, '\\')))
    *biz = '*';
  while((biz = strchr(nick, '|')))
    *biz = '*';
  while((biz = strchr(nick, '[')))
    *biz = '*';
  while((biz = strchr(nick, ']')))
    *biz = '*';

	if(!NUH){
		//seen in a message from irc.freenode.net
		strcpy(ret, nick);
		if(pNUH)
			free(pNUH);
		return ret;
	}
	
  user = get_token(&NUH, "@");
	if(user)
		if(*user == '~' || *user == '#') user++;
  host = get_token(&NUH, ".");
  domain = get_token(&NUH, "");/* warning, can be nul */
  								/* seen with a user whose host was pdpc/supporter/active/user on irc.freenode.net */

  if(domain){
	  for(i='0'; i<='9' && !numbers; i++)
		  if(strchr(domain, i))
			  numbers = TRUE;
	  
	  for(i='a'; i<='z' && numbers; i++)
		  if(strchr(domain, i))
			  numbers = FALSE;

	  for(i='A'; i<= 'Z' && numbers; i++)
		  if(strchr(domain, i))
			  numbers = FALSE;
  }

  if(!numbers)
	  sprintf(ret, "%s!*%s@*.%s", nick, user, domain?(strlen(domain)?domain:host):host);
  else {
    domain = get_token(&domain, ".");
    sprintf(ret, "%s!*%s@%s.%s.*", nick, user, host, domain?domain:"");
  }

  if(pNUH)
    free(pNUH);

  return ret;
}

void ForceDCC(const char *from, const char *to, const char *msg) {
  DCC_list	*userclient;
  char	msg_copy[BIG_BUFFER];	/* for session */
  strcpy(msg_copy, msg);

  userclient = search_list("chat", from, DCC_CHAT);
  if(!userclient || (userclient->flags&DCC_WAIT))
    {
#ifdef AUTO_DCC
      dcc_chat(from, msg);
/*       nodcc_session(from, to, msg_copy); */
      sendnotice(getnick(from), "S'il-te-plaît, tape: /dcc chat %s", 
		 currentbot->nick);
#else
      sendnotice( getnick(from), 
		  "Désolé, je ne peux faire ceci que par DCC." );
      sendnotice( getnick(from), 
		  "Il faudrait ouvrir une session DCC." );
#endif /* AUTO_DCC */
      return;
    }
}

void Ecrire(const char *from, const char *to, const char *phrase) {
	char Ecrite[MAXLEN];

	if(!phrase)
		return;

	sprintf(Ecrite, phrase, GetNick(from), GetNick(from), GetNick(from), GetNick(from), GetNick(from));

	if(ischannel(to)) /* C'est un canal */
	{
		if(strncmp(Ecrite, "/me ", 4) == 0) {
			sendaction(to, Ecrite+4, GetNick(from));
			if(is_log_on(to))
				botlog(LOGFILE, "*%	s#%s* %s", currentbot->botname, to, Ecrite+4);
		} else if(strncmp(Ecrite, "/nil", 4) != 0) {
			sendprivmsg(to, /*phrase*/Ecrite, GetNick(from));
			if(is_log_on(to))
				botlog(LOGFILE, "<%s#%s>	 %s", currentbot->botname, to, Ecrite);
		}
    }
	else /* C'est un /msg */
    {
		if(strncmp(phrase, "/me ", 4) == 0) {
			sendaction(getnick(from), phrase+4, GetNick(from), GetNick(from), GetNick(from), GetNick(from));
			if(logging)
				botlog(LOGFILE, "*%s#%s* %s", currentbot->botname, getnick(from), Ecrite+4);
		} else if(strncmp(phrase, "/nil", 4) != 0) {
			send_to_user(from, phrase, GetNick(from), GetNick(from), GetNick(from), GetNick(from), GetNick(from));
			if(logging)
				botlog(LOGFILE, "<%s#%s> %s", currentbot->botname, from, Ecrite);
		}
    }
} /* void Ecrire() */

void    Repondre(const char *from, const char *to,
		  int AjoutHumeurPos,
		  int NbRepPos, char **RepPos,
		  int AjoutHumeurNeg,
		  int NbRepNeg, char **RepNeg) {

	static int NumRep = 0;
	int Humeur, i;
	locuteur *Locuteur = 0;
	char *NUS = NickUserStr(from);
	
	Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, from);
	if(!Locuteur)
		Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs, NUS);
	
	Humeur = rellevel(from);
	
	if(Humeur >= 0) {
		
		/* Addition des points d'humeur positifs aux points de relation du
		 locuteur */
		AjoutePoints(Locuteur, AjoutHumeurPos);
		
		if(time2hours(time(NULL)) - Locuteur->PremierContact > 12 &&
		   Locuteur->bonus_malus != 0) {
			if(!exist_userhost(currentbot->lists->rellist, from))
				add_to_levellist(currentbot->lists->rellist, NUS, DEFAUT_LVL);
			
			if(Locuteur->bonus_malus > 0)
				add_to_level(currentbot->lists->rellist, from, +1);
			else if(Locuteur->bonus_malus < 0)
				add_to_level(currentbot->lists->rellist, from, -2);
			AnnulePoints(Locuteur);
			AnnuleBonjours(Locuteur);
			MAJPremierContact(Locuteur);
		}
		
		if(RepPos && NbRepPos)
			Ecrire(from, to, RepPos[NumRep%NbRepPos]);
		MAJDernierContact(Locuteur);
	} else {
		/* Addition des points d'humeur negatifs aux points de relation du
		 locuteur */
		AjoutePoints(Locuteur, AjoutHumeurNeg);
		
		if(time2hours(time(NULL)) - Locuteur->PremierContact > 12 &&
		   Locuteur->bonus_malus != 0) {
			if(!exist_userhost(currentbot->lists->rellist, from))
				add_to_levellist(currentbot->lists->rellist, NUS, DEFAUT_LVL);
			
			if(Locuteur->bonus_malus > 0)
				add_to_level(currentbot->lists->rellist, from, +1);
			else if(Locuteur->bonus_malus < 0)
				add_to_level(currentbot->lists->rellist, from, -2);
			AnnulePoints(Locuteur);
			AnnuleBonjours(Locuteur);
			MAJPremierContact(Locuteur);
		}
		
		if(NbRepNeg && RepNeg)
			Ecrire(from, to, RepNeg[NumRep%NbRepNeg]);
		MAJDernierContact(Locuteur);
	}
	
	NumRep++;
	
	/* Desallocation des reponses positives */
	for(i = 0; i < NbRepPos; i++)
		if(RepPos[i])
			free(RepPos[i]);
	
	if(NbRepPos > 0 && RepPos)
		free(RepPos);
	
	/* Desallocation des reponses negatives */
	for(i = 0; i < NbRepNeg; i++)
		if(RepNeg[i])
			free(RepNeg[i]);
	
	if(NbRepNeg > 0 && RepNeg)
		free(RepNeg);
	
	if(NUS)
		free(NUS);

} /* void Repondre */

void    Kicker(const char *from, const char *to, const char *phrase) {
  char Ecrite[MAXLEN];
  const char *Canal = ischannel(to)?to:(channelname(getnick(from))?channelname(getnick(from)):currentchannel());
 
 sprintf(Ecrite, (phrase?phrase:""), GetNick(from), GetNick(from), GetNick(from), GetNick(from), GetNick(from));

  if(protlevel(from) < 50) {
/*     if(username(from)) { */
      

    if(is_log_on(Canal))
      botlog(LOGFILE, "<%s kicke %s de %s> %s", currentbot->botname, getnick(from),
	      /* (ischannel(to)?to:currentchannel()), */
	      Canal,
	      Ecrite);

    sendkick(
	      /*(ischannel(to)?to:currentchannel()), */
	      Canal,
	      getnick(from), Ecrite);
/*     } else { */
/*       char **Reponse, **Reponse2; */
      
/*       Reponse = malloc(2*sizeof(char *)); */
      
/*       Reponse[0] = strdup("T'as de la chance de ne pas etre dans un canal ou je suis, je me serai vu oblige de te kicker, %s."); */
/*       Reponse[1] = strdup("La, %s, normalement je te kickais."); */

/*       Reponse2 = malloc(4*sizeof(char *)); */
      
/*       Reponse2[0] = strdup("C'est bien parce que tu n'es pas dans un canal que je ne te kicke pas, %s!"); */
/*       Reponse2[1] = strdup("Tiens le toi pour dit, %s: quand tu seras sur un canal ou je suis, je ne me generai pas pour te kicker!"); */
/*       Reponse2[2] = strdup("Je t'en prie, ne vient pas dans un canal ou je suis, %s!"); */
/*       Reponse2[3] = strdup("Va te faire voir chez les Grecs, %s!"); */
      
/*       Repondre(from, to, 0, 2, Reponse, 0, 4, Reponse2); */
/*     }  */

  } else {
    char **Reponse2;

    Reponse2 = malloc(4*sizeof(char *));

    Reponse2[0] = strdup("C'est bien parce que je suis censé te protéger que je ne te kicke pas, %s!");
    Reponse2[1] = strdup("Tiens le toi pour dit, %s: quand tu ne seras plus protégé, je ne me gênerai pas pour te kicker!");
    Reponse2[2] = strdup("Je t'en prie, ne reste pas dans ce canal, %s!");
    Reponse2[3] = strdup("Va te faire voir chez les Grecs, %s!");

    Repondre(from, to, 0, 0, 0, 0, 4, Reponse2);
  }
} /* void Kicker */


void    KickerRepondre(const char *from, const char *to,
			int AjoutHumeurPos,
			int NbRepPos, char **RepPos,
			int AjoutHumeurNeg,
			int NbRepNeg, char **RepNeg) {
  static  int NumRep = 0;
  int Humeur, i;
  locuteur *Locuteur = 0;
  char *NUS = NickUserStr(from);

  Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, from);
  if(!Locuteur)
    Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs,
			       NUS);
  
  Humeur = rellevel(from);

  if(Humeur >= 0) {
	
    /* Addition des points d'humeur positifs aux points de relation du
       locuteur */
    AjoutePoints(Locuteur, AjoutHumeurPos);

    if(time2hours(time(NULL)) - Locuteur->PremierContact > 12 &&
	Locuteur->bonus_malus != 0) {
      if(!exist_userhost(currentbot->lists->rellist, from))
		  add_to_levellist(currentbot->lists->rellist, NickUserStr(from), DEFAUT_LVL);

      if(Locuteur->bonus_malus > 0)
	add_to_level(currentbot->lists->rellist, from, +1);
      else if(Locuteur->bonus_malus < 0)
	add_to_level(currentbot->lists->rellist, from, -2);
      AnnulePoints(Locuteur);
      AnnuleBonjours(Locuteur);
      MAJPremierContact(Locuteur);
    }

    if(RepPos && NbRepPos>0)
      Ecrire(from, to, RepPos[NumRep%NbRepPos]);
    MAJDernierContact(Locuteur);
  } else {
    /* Addition des points d'humeur negatifs aux points de relation du
       locuteur */
    AjoutePoints(Locuteur, AjoutHumeurNeg);

    if(time2hours(time(NULL)) - Locuteur->PremierContact > 12 &&
	Locuteur->bonus_malus != 0) {
      if(!exist_userhost(currentbot->lists->rellist, from))
	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0);

      if(Locuteur->bonus_malus > 0)
	add_to_level(currentbot->lists->rellist, from, +1);
      else if(Locuteur->bonus_malus < 0)
	add_to_level(currentbot->lists->rellist, from, -2);
      AnnulePoints(Locuteur);
      AnnuleBonjours(Locuteur);
      MAJPremierContact(Locuteur);
    }
    
    if(RepNeg && NbRepNeg>0)
      Kicker(from, to, RepNeg[NumRep%NbRepNeg]);
    MAJDernierContact(Locuteur);
  }

  NumRep++;

  /* Desallocation des reponses positives */
  for(i = 0; i < NbRepPos; i++)
    if(RepPos[i])
      free(RepPos[i]);

  if(NbRepPos > 0 && RepPos)
    free(RepPos);

  /* Desallocation des reponses negatives */
  for(i = 0; i < NbRepNeg; i++)
    if(RepNeg[i])
      free(RepNeg[i]);

  if(NbRepNeg > 0 && RepNeg)
    free(RepNeg);
  
  if(NUS)
    free(NUS);

} /* void KickerRepondre */

void    KickerKicker(const char *from, const char *to,
			int AjoutHumeurPos,
			int NbRepPos, char **RepPos,
			int AjoutHumeurNeg,
			int NbRepNeg, char **RepNeg) {
  static  int NumRep = 0;
  int Humeur, i;
  locuteur *Locuteur = 0;
  char *NUS = NickUserStr(from);

  Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, from);
  if(!Locuteur)
    Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs,
			       NUS);
  
  Humeur = rellevel(from);

  if(Humeur >= 0) {
	
    /* Addition des points d'humeur positifs aux points de relation du
       locuteur */
    AjoutePoints(Locuteur, AjoutHumeurPos);

    if(time2hours(time(NULL)) - Locuteur->PremierContact > 12 &&
	Locuteur->bonus_malus != 0) {
      if(!exist_userhost(currentbot->lists->rellist, from))
	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0);

      if(Locuteur->bonus_malus > 0)
	add_to_level(currentbot->lists->rellist, from, +1);
      else if(Locuteur->bonus_malus < 0)
	add_to_level(currentbot->lists->rellist, from, -2);
      AnnulePoints(Locuteur);
      AnnuleBonjours(Locuteur);
      MAJPremierContact(Locuteur);
    }

    if(RepPos && NbRepPos>0)
      Kicker(from, to, RepPos[NumRep%NbRepPos]);
    MAJDernierContact(Locuteur);
  } else {
    /* Addition des points d'humeur negatifs aux points de relation du
       locuteur */
    AjoutePoints(Locuteur, AjoutHumeurNeg);

    if(time2hours(time(NULL)) - Locuteur->PremierContact > 12 &&
	Locuteur->bonus_malus != 0) {
      if(!exist_userhost(currentbot->lists->rellist, from))
	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0);

      if(Locuteur->bonus_malus > 0)
	add_to_level(currentbot->lists->rellist, from, +1);
      else if(Locuteur->bonus_malus < 0)
	add_to_level(currentbot->lists->rellist, from, -2);
      AnnulePoints(Locuteur);
      AnnuleBonjours(Locuteur);
      MAJPremierContact(Locuteur);
    }
    
    if(RepNeg && NbRepNeg>0)
      Kicker(from, to, RepNeg[NumRep%NbRepNeg]);
    MAJDernierContact(Locuteur);
  }

  NumRep++;

  /* Desallocation des reponses positives */
  for(i = 0; i < NbRepPos; i++)
    if(RepPos[i])
      free(RepPos[i]);

  if(NbRepPos > 0 && RepPos)
    free(RepPos);

  /* Desallocation des reponses negatives */
  for(i = 0; i < NbRepNeg; i++)
    if(RepNeg[i])
      free(RepNeg[i]);

  if(NbRepNeg > 0 && RepNeg)
    free(RepNeg);
  
  if(NUS)
    free(NUS);

} /* void KickerKicker */


void	on_kick(const char *from, const char *channel, const char *nick, const char *reason)
{
  char **RepPos, **RepNeg;
  /*
   * If user A kicks user B then kick user A if B was protected
   * and A has lower protlevel
   */
  if(is_log_on(channel))
    botlog(LOGFILE, "<%s kicke %s de %s> %s", from, nick, channel, reason?reason:"");
  /*
   * Perhaps vladbot is getting to much warbot with this piece
   * of code. Maybe add an #ifdef...
   */
  if(shitlevel(username(nick)) == 0 &&
     protlevel(username(nick)) >= 50 &&
     (protlevel(from) < 50 || !is_bot(currentbot->botlist, from, channel)) &&
     /* I should never revenge myself :) */
     !STRCASEEQUAL(currentbot->nick, getnick(from))) {
    
    if(rellevel(username(nick)) > 0) {
      RepPos = malloc(3*sizeof(char *));
      RepPos[0] = strdup("Ça n'était pas très gentil, ça, %s!");
      RepPos[1] = strdup("Je ne peux permettre cela, %s.");
      RepPos[2] = strdup("Oui mais non!");
      RepNeg = malloc(3*sizeof(char *));
      RepNeg[0] = strdup("Ça va pas, non?");
      RepNeg[1] = strdup("Méchant pas beau!");
      RepNeg[2] = strdup("Décidément, ta tête me r'vient pas, %s!");
      KickerKicker(from, channel, -10, 3, RepPos, -10, 3, RepNeg);
    } else {
      RepPos = malloc(3*sizeof(char *));
      RepPos[0] = strdup("Je sais bien qu'il est pas très aimable, mais quand-même, %s!");
      RepPos[1] = strdup("J'aime pas l'ambiance qui s'installe ici, %s.");
      RepPos[2] = strdup("Bon, je tolère, mais paske je l'aime pas.");
      RepNeg = malloc(3*sizeof(char *));
      RepNeg[0] = strdup("Allez vous tapez dessus ailleurs!");
      RepNeg[1] = strdup("C'est pas un tripot ici!");
      RepNeg[2] = strdup("Dehors %s!");
      KickerRepondre(from, channel, -5, 3, RepPos, -9, 3, RepNeg);
    }

  } else if(rellevel(username(nick)) > 0 && /* nick sympathique */
	     rellevel(username(nick)) > rellevel(from) && /* plus que le kickeur */
	     !STRCASEEQUAL(currentbot->nick, getnick(from)) /* nick pas moi */) {
    RepPos = malloc(3*sizeof(char *));

    RepPos[0] = strdup("/me n'aime pas qu'on kicke ses amis, %s.");
    RepPos[1] = strdup("Calme toi %s. Je vais finir par ne plus t'apprécier du tout!");
    RepPos[2] = strdup("Mais c'est pas vrai, %s! Du calme!");

    RepNeg = malloc(3*sizeof(char *));
    
    RepNeg[0] = strdup("C'est pas bientôt fini, oui?");
    RepNeg[1] = strdup("On se calme!");
    RepNeg[2] = strdup("Laisse mes copains tranquilles!");

    if(protlevel(from) >= 50)
      Repondre(from, channel, -3, 3, RepPos, -5, 3, RepNeg);
    else 
      KickerRepondre(from, channel, -2, 3, RepPos, -4, 3, RepNeg);
  }

  if(STRCASEEQUAL(currentbot->nick, nick)) {
    send_to_user(from, "Je te préviens: je n'ai pas beaucoup apprécié. Continue comme ça et tu te retrouveras sur ma liste noire!");
    if(rellevel(from) < SYMPA_LVL) {
      if(!exist_userhost(currentbot->lists->shitlist, from))
	add_to_levellist(currentbot->lists->shitlist, NickUserStr(from), +1);
      else
	add_to_level(currentbot->lists->shitlist, from, +1);
    } else {
      char **RepPos;
      RepPos = malloc(6*sizeof(char *));
      RepPos[0] = strdup("Je ne supporte pas ça, %s!");
      RepPos[1] = strdup("Qu'est-ce qui te prends, %s?");
      RepPos[2] = strdup("Oui mais non!");
      RepPos[3] = strdup("Ça va pas, non?");
      RepPos[4] = strdup("Tu as décidé de me décevoir, %s?");
      RepPos[5] = strdup("Fichtre! Diantre, %s! Je ne supporterai pas ça derechef!");
      KickerKicker(from, channel, -10, 6, RepPos, -10, 0, NULL);
    }
  }
}

void 	on_join(const char *who, const char *channel)
{
	static int NumPhrase = -1;
	int HeureCourante;
	int pendingNotes;
	locuteur *Locuteur;
	char **Reponse, **Reponse2;
	char *NUS = NickUserStr(who);
	char *pNUS = NUS;
  
	/* si c'est moi, je ne fais rien (je ne peux rien faire d'autre que
	   dire bonjour...) */
  //  if(strcmp(NUS,currentbot->botname)==0) {
	if(strcmp(get_token(&pNUS,"!"),currentbot->nick)==0) {
		Reponse = malloc(6*sizeof(char *));
		Reponse[0] = strdup("Bonjour à tous!");
		Reponse[1] = strdup("Bonjour, c'est moi, %s.");
		Reponse[2] = strdup(" La foule: «Ah, enfin!» :)");
		Reponse[3] = strdup("Merci de m'accueillir avec bienveillance!");
		Reponse[4] = strdup("Heureux de vous voir.");
		Reponse[5] = strdup("Bonjour, ça va?");
		Repondre(who, channel, +0, 6, Reponse, +0, 0, 0);
		if(NUS)
			free(NUS);
		return;
	}

#ifdef DBUG
	debug(LVL_NOTICE, "%s (%s) has joined %s", getnick(who), who, channel);
#endif

	if(shitlevel(who) == 100){
		char **RepPos, **RepNeg;
		
		ban_user(who, channel);
      
		RepPos = malloc(3*sizeof(char *));
		RepPos[0] = strdup("Tu n'as plus rien à faire ici, %s.");
		RepPos[1] = strdup("Il m'est interdit de t'admettre en ces lieux, %s.");
		RepPos[2] = strdup("Fais-toi enlever de ma liste noire, %s.");
		RepNeg = malloc(3*sizeof(char *));
		RepNeg[0] = strdup("Reste dehors, %s!");
		RepNeg[1] = strdup("On ne veut plus de toi ici, %s!");
		RepNeg[2] = strdup("Dégage, %s!");
		KickerKicker(who, channel, 0, 3, RepPos, 0, 3, RepNeg);
		if(NUS)
			free(NUS);
		return;
    }

	if(userlevel(who) >= AUTO_OPLVL && shitlevel(who) == 0 && 
	   is_bot(currentbot->botlist, channel, who)) {
		giveop(channel, getnick(who));
		if(NUS)
			free(NUS);
		return;
	}

	Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, who);
	if(!Locuteur)
		Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs,
								  NUS);
  
	HeureCourante = time2hours(time(NULL));
  
	if(rellevel(who) >= SYMPA_LVL &&
	   !is_bot(currentbot->botlist, channel, who)) {
		NumPhrase++;
		/* Si ca fait plus de HOURS_BETWEEN_SALUTES heures qu'on n'a pas entendu le locuteur */
		if(HeureCourante-Locuteur->DernierContact > HOURS_BETWEEN_SALUTES) {
			Reponse = malloc(6*sizeof(char *));
			Reponse[0] = strdup("Salut %s.");
			Reponse[1] = strdup("Bienvenue à toi, %s.");
			Reponse[2] = strdup("Enfin de retour, %s! :)");
			Reponse[3] = strdup("Super! %s est arrivé!");
			Reponse[4] = strdup("Heureux de te voir, %s!");
			Reponse[5] = strdup("Bonjour %s.");
      
			Repondre(who, channel, +0, 6, Reponse, +0, 0, 0);
			if(!LocuteurExiste(currentbot->lists->ListeLocuteurs, who))
				Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs,
										  NUS);
			Locuteur->Bonjours = 1;
		} else {
			Reponse = malloc(8*sizeof(char *));
			Reponse[0] = strdup("re %s.");
			Reponse[1] = 0;
			Reponse[2] = strdup("Areuh %s.");
			Reponse[3] = 0;
			Reponse[4] = strdup("Bon retour parmi nous, %s!");
			Reponse[5] = strdup("Tiens! Revoila %s. :)");
			Reponse[6] = strdup("Super! %s est de retour!");
			Reponse[7] = 0;
      
			Repondre(who, channel, +0, 8, Reponse, +0, 0, 0);
		}
	} else if(rellevel(who) < -SYMPA_LVL
			  && HeureCourante - Locuteur->DernierContact > 8
			  && !is_bot(currentbot->botlist, channel, who)) {
		Reponse2 = malloc(7*sizeof(char *));
		Reponse2[0] = strdup("%s: Grrrr");
		Reponse2[1] = strdup("T'ose encore revenir ici, %s? T'es gonflé!");
		Reponse2[2] = strdup("Oh! Non! %s! :[");
		Reponse2[3] = strdup("Zut! %s est arrivé!");
		Reponse2[4] = strdup("Encore toi, %s! :(");
		Reponse2[5] = strdup("Je ne te souhaite pas le bonjour %s.");
		Reponse2[6] = strdup("T'as intérêt à t'excuser, %s.");
    
		Repondre(who, channel, +0, 0, 0, +0, 7, Reponse2);
		if(!LocuteurExiste(currentbot->lists->ListeLocuteurs, who))
			Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs,
									  NUS);
		Locuteur->Bonjours = 1;
	}
	
	if((pendingNotes = note_exist(who))){
		send_to_user(who, "Vous avez %d messages en attente.", pendingNotes);
		send_to_user(who, "Pour la liste des messages : /msg %s note list", currentbot->nick);
	}
	
	if(NUS)
		free(NUS);
}

void	on_mode(const char *from, const char *rest)
/*
 * from = who did the modechange
 * rest = modestring, usually of form +mode-mode param1 .. paramN
 */
#define REMOVEMODE(chr, prm) do{ strcat(unminmode, chr);	\
		strcat(unminparams, prm);							\
		strcat(unminparams, " ");							\
	} while(0)

#define ADDMODE(chr, prm)    do{ strcat(unplusmode, chr);	\
		strcat(unplusparams, prm);							\
		strcat(unplusparams, " ");							\
	} while(0)
{
    /*static  int NumPhrase = 0;
	int     Numero;*/
	int	did_change=FALSE;	/* If we have to change anything */
	char	*channel;

	char	*chanchars;	/* i.e. +oobli */
	char	*params;
	char	sign = '+';	/* +/-, i.e. +o, -b etc */

	char	unminmode[MAXLEN] = "";		/* used to undo modechanges */
	char	unminparams[MAXLEN] = "";
	char	unplusmode[MAXLEN] = "";	/* used to undo modechanges */
	char	unplusparams[MAXLEN] = "";

	/* these are here merely to make things clearer */
	char	*nick;
	char	*banstring;
	char	*key;
	char	*limit;

	char **RepPos, **RepNeg;

#ifdef DBUG
	debug(LVL_NOTICE, "on_mode(\"%s\", \"%s\")", from, rest);
#endif

	channel = get_token(&rest, " ");
	if(STRCASEEQUAL(channel, currentbot->nick))	/* if target is me... */
		return;			/* mode NederServ +i etc */

	chanchars = get_token(&rest, " ");
	params = rest;

	while( *chanchars ){
		switch( *chanchars ){
			case '+':
			case '-':
				sign = *chanchars;
				break;
			case 'o':
				nick = get_token(&params, " ");
				if(sign == '+'){
					add_channelmode(channel, CHFL_CHANOP, nick);
#ifdef ONLY_OPS_FOR_REGISTRED
					if(shitlevel(username(nick)) > 0 ||
					   userlevel(username(nick)) == 0)
#else
					if(shitlevel(username(nick))>=50)
#endif
					{
						REMOVEMODE("o", nick);
						send_to_user(from, 
#ifdef ONLY_OPS_FOR_REGISTRED
									 "Sorry, I can't allow this :)");
#else
									 "%s is shitted! :P", nick);
#endif
						did_change=TRUE;
					}
					/* Si c'est quelqu'un qu'on voulait opper */
					else if(STRCASEEQUAL(nick, currentbot->nick)&&
							!is_bot(currentbot->botlist, from, channel)){ /* Merci */
						RepPos = malloc(7 * sizeof(char *));
						RepPos[0] = strdup("Merci %s.");
						RepPos[1] = 0;
						RepPos[2] = strdup("C'est gentil, ça, %s.");
						RepPos[3] = 0;
						RepPos[4] = strdup("Tu as toute ma gratitude, %s!");
						RepPos[5] = strdup("C'est gentil de ta part, %s.");
						RepPos[6] = 0;
							
						RepNeg = malloc(3 * sizeof(char *));
						RepNeg[0] = strdup("Étonnant de ta part, %s!");
						RepNeg[1] = strdup("Venant de toi, ça m'étonne, %s!");
						RepNeg[2] = strdup("Je suppose que tu attends un peu de gratitude de ma part, %s.");
						Repondre(from, channel, +1, 7, RepPos, +2, 3, RepNeg);
					}
				}
				else{
					if(STRCASEEQUAL(nick, currentbot->nick) &&
					   !is_bot(currentbot->botlist, from, channel)) {
						RepPos = malloc(3 * sizeof(char *));
						RepPos[0] = strdup("Je suis déçu %s.");
						RepPos[1] = strdup("C'est pas gentil, ça, %s.");
						RepPos[2] = strdup("S'il-te-plaît, %s, redonne-moi les droits!");
						
						RepNeg = malloc(3 * sizeof(char *));
						RepNeg[0] = strdup(""/*Espèce de charogne, %s!"*/);
						RepNeg[1] = strdup("Pourquoi tu me déoppes %s?");
						RepNeg[2] = strdup("Ne recommence jamais ça!");
						Repondre(from, channel, -2, 3, RepPos, -10, 3, RepNeg);
						return;
					}
			
					del_channelmode(channel, CHFL_CHANOP, nick);
					if((protlevel(username(nick))>=100)&&
					   (shitlevel(username(nick))==0)){
						ADDMODE("o", nick);
						RepPos = malloc(3 * sizeof(char *));
						RepPos[0] = strdup("Je protège cet utilisateur, désolé %s.");
						RepPos[1] = strdup("Prière de ne pas recommencer, %s. Je ne le permets pas.");
						RepPos[2] = strdup("Ne recommence pas tant que je suis chargé de protéger cet utilisateur, si tu veux rester en bon termes avec moi, %s.");
							
						RepNeg = malloc(3 * sizeof(char *));
						RepNeg[0] = strdup("Et pis quoi encore %s? Tu te crois tout permis. Je ne veux pas!");
						RepNeg[1] = strdup("Estime-toi heureux que je ne te kicke pas, %s! Je suis chargé de protéger cet utilisateur.");
						RepNeg[2] = strdup("Pas question de déopper cet utilisateur!");
						Repondre(from, channel, 0, 3, RepPos, -1, 3, RepNeg);
/*
						NumPhrase ++;
						Numero = NumPhrase % 3;
						switch(Numero) {
						case 0:
						  send_to_user(from, 
									   "%s is protected!",
									   nick);
						  break;
						case 1:
						  send_to_user(from,
									   "Pas question: je protège %s!",
									   nick);
						  break;
						case 2:
						  send_to_user(from,
										"%s est sous ma protection.",
										nick);
						  break;
						}
*/
						did_change=TRUE;
					}	
				}
				break;
			case 'v':
				nick = get_token(&params, " ");
				if(sign == '+')
					add_channelmode(channel, CHFL_VOICE, nick);
				else
					del_channelmode(channel, CHFL_VOICE, nick);
				break;
			case 'b':
				banstring = get_token(&params, " ");
				if(sign == '+'){
					add_channelmode(channel, MODE_BAN, banstring);
					if(find_highest(channel, banstring) >= 50){
						REMOVEMODE("b", banstring);
						did_change = TRUE;
					}
				}
				else
					del_channelmode(channel, MODE_BAN, banstring);
				break;
			case 'p':
				if(sign == '+')
					add_channelmode(channel, MODE_PRIVATE, "");
				else
					del_channelmode(channel, MODE_PRIVATE, "");
				break;
			case 's':
				if(sign == '+')
					add_channelmode(channel, MODE_SECRET, "");
				else
					del_channelmode(channel, MODE_SECRET, "");
				break;
			case 'm':
				if(sign == '+')
					add_channelmode(channel, MODE_MODERATED, "");
				else
					del_channelmode(channel, MODE_MODERATED, "");
				break;
			case 't':
				if(sign == '+')
					add_channelmode(channel, MODE_TOPICLIMIT, "");
				else
					del_channelmode(channel, MODE_TOPICLIMIT, "");
				break;
			case 'i':
				if(sign == '+')
					add_channelmode(channel, MODE_INVITEONLY, "");
				else
					del_channelmode(channel, MODE_INVITEONLY, "");
				break;
			case 'n':
				if(sign == '+')
					add_channelmode(channel, MODE_NOPRIVMSGS, "");
				else
					del_channelmode(channel, MODE_NOPRIVMSGS, "");
				break;
			case 'k':
				key = get_token(&params, " ");
				if(sign == '+'){
					char	*s;

					add_channelmode(channel, MODE_KEY, 
									key?key:"???");
					/* try to unset bogus keys */
					for(s = key; key && *s; s++)
						if(*s < ' '){
							REMOVEMODE("k", key);
							did_change = TRUE;
							send_to_user(from,
							"No bogus keys pls");
							break;
						}
				}
				else
					del_channelmode(channel, MODE_KEY, "");
				break;
			case 'l':
				if(sign == '+'){
					limit = get_token(&params, " ");
					add_channelmode(channel, MODE_LIMIT, 
									limit?limit:"0");
				}
				else
					del_channelmode(channel, MODE_LIMIT, "");
				break;
			default:
#ifdef DBUG
				debug(LVL_ERROR, "on_mode(): unknown mode %c", *chanchars);
#endif
				break;
		}
		chanchars++;
	}
	/* restore unwanted modechanges */
	if(did_change)
		sendmode( channel, "+%s-%s %s %s", unplusmode, 
				 unminmode, unplusparams, unminparams);
}

void	on_msg(const char *from, const char *to, const char *msg_untranslated){
	int	i;
	char	msg_copy[BIG_BUFFER];	/* for session */
	char	*command;
	DCC_list	*userclient;
	char **RepPos, **RepNeg;
	int     Flooding;
	locuteur *Locuteur;

	/* on va tenter de traduire l'encodage si possible */
	/* je ne sais pas ce que la recherche de channel va couter en perfs */
	char *msg, *msg_ori = msg_untranslated;
	CHAN_list *channel;
	char buf[MAXLEN];
	char *pbuf = buf;//, *pmsg = &msg_untranslated;
	size_t lin, lout;
	//pmsg = (char *)&msg_translated;
	if(ischannel(to)){
		channel = search_chan(to);
		if(channel->decoder != (iconv_t)-1){
			lin = strlen(msg_untranslated);
			lout = sizeof(buf);
			memset(buf, 0, sizeof(buf));
			if(iconv(channel->decoder, &msg_untranslated, &lin, &pbuf, &lout) != -1)
				msg = buf;
			else
				msg = msg_ori;
		}
		else
			msg = msg_untranslated;
	}
	else{
		//on va essayer de voir si on peut décoder de l'utf
		lin = strlen(msg_untranslated);
		lout = sizeof(buf);
		memset(buf, 0, sizeof(buf));
		if(iconv(currentbot->def_decoder, &msg_untranslated, &lin, &pbuf, &lout) != -1)
			msg = buf;
		else
			msg = msg_ori;
	}
	
	//garde une trace des locuteurs
	Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, from);
	if(!Locuteur)
		Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs, NickUserStr(from));
	MAJDerniereActivite(Locuteur);

#ifdef DBUG
	debug(LVL_DEBUG, "on_msg: from %s to %s msg : %s\n", from, to, msg);
#endif

	strcpy(msg_copy, msg);
	
	Flooding = check_session(from);
	if(Flooding != IS_FLOODING && !is_bot(currentbot->botlist, to, from)){
		ProcessSimpleStims(from, to, msg);
		LuaTraite(currentbot, from, to, msg, GNumPhrase++);
	}
	else if(Flooding == IS_FLOODING)
		return;

	if((command = get_token(&msg, ",: "))== NULL)
		return;			/* NULL-command */
	
	if(STRCASEEQUAL(currentbot->nick, command)){
		if((command = get_token(&msg, ",: "))==NULL)
			return;		/* NULL-command */
	}
	else if((*command != CommandChar) && !STRCASEEQUAL(to, currentbot->nick))
		return; /* The command should start with CommandChar if public */

	if(*command == CommandChar)
		command++;


	/*
	 * Now we got
	 * - A public command.
	 * - A private command/message. 
	 * Time to do a flood check :).
	 */
/* 	if(check_session(from) == IS_FLOODING) */
/* 		return; */
/*       voir plus haut. */
/*         if(Flooding == IS_FLOODING)	 */
/*           return; */

	for(i = 0; on_msg_commands[i].name != NULL; i++)
		if(STRCASEEQUAL(on_msg_commands[i].name, command)){
			if(userlevel(from) < on_msg_commands[i].userlevel){
				send_to_user(from, "Userlevel too low");
				return;
			}
			if(shitlevel(from) > on_msg_commands[i].shitlevel){
				send_to_user(from, "Shitlevel too high");
				return;
			}
			if(rellevel(from) < on_msg_commands[i].rellevel){
				RepPos = malloc(3 * sizeof(char *));
				RepPos[0] = strdup("Je ne te connais pas encore assez pour ça, %s");
				RepPos[1] = strdup("C'est pas que je ne t'aime pas, mais je ne te connais pas encore assez pour t'autoriser cette commande, %s.");
				RepPos[2] = strdup("Dommage, cette commande réclame que je te fasse encore plus confiance, %s!");
				
				RepNeg = malloc(3 * sizeof(char *));
				RepNeg[0] = strdup("Ça va pas la tête, %s? Que je te laisse faire ça? À toi? Quand tu montreras un peu plus de respect!");
				RepNeg[1] = strdup("Uniquement aux gens que j'aime ou que je respecte assez, %s. Apparemment, c'est pas ton cas.");
				RepNeg[2] = strdup("Si tu veux que je t'autorise un jour à faire ça, t'as intérêt à me demander pardon et à te conduire gentiment avec moi.");
				Repondre(from, to, 0, 3, RepPos, 0, 3, RepNeg);
/*                              send_to_user(from, "Je ne te connais pas assez pour ça!"); */
				return;
			}
			userclient = search_list("chat", from, DCC_CHAT);
			if(on_msg_commands[i].forcedcc && 
			   (!userclient || (userclient->flags&DCC_WAIT))){
#ifdef AUTO_DCC
				dcc_chat(from, msg);
				nodcc_session(from, to, msg_copy);
				sendnotice(getnick(from), "Please type: /dcc chat %s", 
						   currentbot->nick);
#else
				sendnotice( getnick(from), 
							"Sorry, %s is only available through DCC",
							command );
				sendnotice( getnick(from), 
							"Please start a dcc chat connection first" );
#endif /* AUTO_DCC */
				return;
			}
			/* if we're left with a null-string (""), give NULL
			   as msg */
			on_msg_commands[i].function(from, to, *msg?msg:NULL);
			return;
		}
	/* If the command was private, let the user know how stupid (s)he is */
/* 	if(STRCASEEQUAL(to, currentbot->nick)) */
/* 		send_to_user(from, "%s %s?!? What's that?", */
/* 			     command, msg ? msg : ""); */
}

void	show_help(const char *from, const char *to, const char *rest){
	FILE	*f;
	char	*s;

	if((f=fopen(currentbot->helpfile, "r"))==NULL){
		send_to_user(from, "Helpfile missing");
		return;
	}
	
	if(rest == NULL){
		find_topic(f, "standard");
		while((s=get_ftext(f)))
			send_to_user(from, s);
	}
	else
		if(!find_topic( f, rest ))
			send_to_user(from, "No help available for \"%s\"", rest);
		else
			while((s=get_ftext(f)))
				send_to_user(from, s);
	fclose(f);
}

void	show_whoami(const char *from, const char *to, const char *rest)
{
	/*
#ifdef DBUG
	printf("WHOAMI : You are %s. Your levels are:\n", from);
	printf("-- User -+- Shit -+- Protect -+- Relation -+\n");
	printf("    %-3d  |   %-3d  |   %-3d   |    %-3d  |\n",userlevel(from),
			shitlevel(from), protlevel(from), rellevel(from));
#endif*/
	debug(LVL_NOTICE, "WHOAMI %s (%s)", getnick(from), from);
	send_to_user(from, "You are %s. Your levels are:", 
				 from);
	send_to_user(from, "+-- User -+- Shit -+- Protect -+- Relation -+");
	send_to_user(from, "|    %3d  |    %3d  |    %3d    |    %4d    |",	//weird aligment bug here...
				 userlevel(from), shitlevel(from), protlevel(from), rellevel(from));
	return;
}

void	show_info(const char *from, const char *to, const char *rest)
{
	sendreply("I am modified VladBot version %s (%s)", VERSION, currentbot->botname);
	sendreply("Started: %-20.20s", time2str(currentbot->uptime));
	sendreply("Up: %s", idle2str(time(NULL)-currentbot->uptime));
	if(botmaintainer)
		sendreply("This bot is maintained by %s <%s>", botmaintainername ? botmaintainername : "", botmaintainer);
	return;
}

void	show_time(const char *from, const char *to, const char *rest)
{
	sendreply("Current time: %s", time2str(time(NULL)));
}

void	show_userlist(const char *from, const char *to, const char *rest)
{
	send_to_user(from, " userlist: %30s   %s", "nick!user@host", "level");
        send_to_user(from, " -----------------------------------------+------");
/* 	if(rest) */
	  show_lvllist(currentbot->lists->opperlist, from, rest?rest:"");
/* 	else  */
/* 	  send_to_user(from, "Il faut mettre un paramètre. Je ne tiens pas à transmettre trop de données d'un seul coup."); */
}

void	show_shitlist(const char *from, const char *to, const char *rest)
{
	send_to_user(from, " shitlist: %30s   %s", "nick!user@host", "level");
        send_to_user(from, " -----------------------------------------+------");
/* 	if(rest) */
	  show_lvllist(currentbot->lists->shitlist, from, rest?rest:"");
/* 	else  */
/* 	  send_to_user(from, "Il faut mettre un paramètre. Je ne tiens pas à transmettre trop de données d'un seul coup."); */
}

void    show_protlist(const char *from, const char *to, const char *rest)
{
        send_to_user(from, " protlist: %30s   %s", "nick!user@host", "level");
        send_to_user(from, " -----------------------------------------+------");
/* 	if(rest) */
	  show_lvllist(currentbot->lists->protlist, from, rest?rest:"");
/* 	else */
/* 	  send_to_user(from, "Il faut mettre un paramètre. Je ne tiens pas à transmettre trop de données d'un seul coup."); */
}

void    show_rellist(const char *from, const char *to, const char *rest)
{
   send_to_user(from, "  rellist: %30s   %s", "nick!user@host", "level");
   send_to_user(from, " -----------------------------------------+------");
   if(rest)
     show_lvllist(currentbot->lists->rellist, from, rest?rest:"");
   else
     send_to_user(from, "Il faut mettre un paramètre. Je ne tiens pas à transmettre trop de données d'un seul coup.");
}

void    show_mycmds(const char *from, const char *to, const char *rest)
{
  int user_level;
  int i=0;
  user_level = userlevel(from);

  send_to_user(from, " Commands available to %s: ", getnick(from));
  
  while(on_msg_commands[i].name) {
    if(on_msg_commands[i].userlevel <= user_level && on_msg_commands[i].rellevel <= rellevel(from))
      send_to_user(from, " Command %s", on_msg_commands[i].name);
    i++; /* Passage a la commande suivante */
  }
  
}

void    show_cmdlvls(const char *from, const char *to, const char *rest) 
{
  int i = 0;

  send_to_user(from, " Commands and levels required:");

  while(on_msg_commands[i].name) {
    send_to_user(from, " Command %9s, %3du, %3ds, %4dr",
		  on_msg_commands[i].name,
		  on_msg_commands[i].userlevel,
		  on_msg_commands[i].shitlevel,
		  on_msg_commands[i].rellevel);
    i++;
  }
  
}

void	do_op(const char *from, const char *to, const char *rest){
	if(usermode(!STRCASEEQUAL(to, currentbot->nick) ? to : currentchannel(),  
				getnick(from)) & MODE_CHANOP) {
		char **RepPos, **RepNeg;
		RepPos = malloc(3 * sizeof(char *));
		RepPos[0] = strdup("Tête en l'air va! Tu es déjà op, %s.");
		RepPos[1] = strdup("Tu aspires à devenir surop, %s? ;)");
		RepPos[2] = strdup("Mets tes lunettes, %s, tu es déjà opérateur du canal!");
		RepNeg = malloc(4 * sizeof(char *));
		RepNeg[0] = strdup("Ça te prend souvent, cette soif de pouvoir, %s?");
		RepNeg[1] = strdup("Calme-toi un peu, %s!");
		RepNeg[2] = strdup("Ça va pas, la tête? Je te signale que t'es déjà op, %s!");
		RepNeg[3] = strdup("Tu radotes, %s!");
		Repondre(from, to, 0, 3, RepPos, -1, 4, RepNeg);
	} else if(userlevel(from) < USERLVL_MODLISTPOW) {
		char **RepPos, **RepNeg;
		RepPos = malloc(3 * sizeof(char *));
		RepPos[0] = strdup("Désolé, %s. Je n'oppe plus. Il vaut mieux utiliser !TOPIC, !KICK ou !BANUSER...");
		RepPos[1] = strdup("Pourquoi faire, %s? Utilise plutôt TOPIC, KICK ou BANUSER...");
		RepPos[2] = strdup("Je suis fatigué d'opper tout le monde! Utilise plutôt TOPIC, KICK ou BAN.");
		RepNeg = malloc(4 * sizeof(char *));
		RepNeg[0] = strdup("Ça te prend souvent, cette soif de pouvoir, %s?");
		RepNeg[1] = strdup("Calme-toi un peu, %s!");
		RepNeg[2] = strdup("Ça va pas, la tête? Pas à toi, %s!");
		RepNeg[3] = strdup("Quand tu seras plus raisonnable, %s!");
		Repondre(from, to, 0, 3, RepPos, -1, 4, RepNeg);
	} else {
		giveop(!STRCASEEQUAL(to, currentbot->nick) ? to : currentchannel(),  
			   getnick(from));
	}
}

#ifndef WIN32 //FIXME find another solution for windows
void	do_alarm(const char *from, const char *to, const char *rest)
{
	time_t maintenant = time(NULL);
	time_t quand  = parsetime(rest?rest:"");
	struct	tm	*btime;

	if(quand == -1)
		send_to_user(from, "Date/Heure mauvaise");
	else
		if(quand == -2)
			send_to_user(from, "On remonte dans le temps ? ;-)");
		else
		{
			btime = localtime(&quand);
		  	signal(SIGALRM, sig_alrm);
			alarm(quand - maintenant);
			send_to_user(from, "Alarme activee pour le %-2.2d %s %d a %-2.2d:%-2.2d:%-2.2d (dans %ld secondes)",
				btime->tm_mday, months[btime->tm_mon],
				btime->tm_year + 1900, 
				btime->tm_hour, btime->tm_min, btime->tm_sec,
				quand-maintenant);
		}
}
#endif

void	do_giveop(const char *from, const char *to, const char *rest){
	int	i = 0;
	char	nickname[MAXNICKLEN];
	char	op[MAXLEN];

	strcpy(op, "");
	if(not(rest)){
		sendreply("Who do you want me to op?");
		return;
	}
	while(readnick(&rest, nickname))
		if(username(nickname) != NULL && 
		   shitlevel(username(nickname)) == 0){
			i++;
			strcat(op, " ");
			strcat(op, nickname);
			if(i==3){
				sendmode(ischannel(to) ?to :currentchannel(), "+ooo %s", op);
				i = 0;
				strcpy(op, "");
			}
		}
	if(i != 0)
		sendmode(ischannel(to) ?to :currentchannel(), "+ooo %s", op);
	else
	  sendreply("J'ai un problème: je ne trouve pas ce nickname. Désolé.");
}

void	do_deop(const char *from, const char *to, const char *rest){
	int	i = 0;
	char	nickname[MAXNICKLEN];
	char	deop[MAXLEN];

	strcpy(deop, "");
	if(not(rest)){
		sendreply("Who do you want me to deop?");
		return;
	}
	while(readnick(&rest, nickname))
		if(username(nickname) != NULL &&
		   (protlevel(username(nickname)) < 100 ||
			shitlevel(username(nickname)) > 0)){
			i++;
			strcat(deop, " ");
			strcat(deop, nickname);
			if(i==3){
				sendmode(ischannel(to) ?to :currentchannel(), "-ooo %s", deop);
				i = 0;
				strcpy(deop, "");
			}
		}
	if(i != 0)
		sendmode(ischannel(to) ?to :currentchannel(), "-ooo %s", deop);
}

void	do_invite(const char *from, const char *to, const char *rest){
	if(rest){
		if(!invite_to_channel(getnick(from), rest))	
			send_to_user(from, "I'm not on channel %s", rest);
	}
	else
		invite_to_channel(from, currentchannel());
}
			
void	do_open(const char *from, const char *to, const char *rest){
	if(rest){
		if(!open_channel(rest))
			send_to_user(from, "I could not open %s!", rest);
		else
			channel_unban(rest, from);
	}
	else 
		if(ischannel(to)){
			if(!open_channel(to))
				send_to_user(from, "I could not open %s!", to);
			else
				channel_unban(to, from);
		}
		else {
			open_channel(currentchannel());
			channel_unban(currentchannel(), from);
		}
}

void    do_chat(const char *from, const char *to, const char *rest){
	dcc_chat(from, rest);
	sendnotice(getnick(from), "Please type: /dcc chat %s", currentbot->nick);
}

void    do_send(const char *from, const char *to, const char *rest){
	char	*pattern;
	
	if(rest)
		while((pattern = get_token(&rest, " ")))
			send_file(from, pattern);
	else
		send_to_user(from, "Please specify a filename (use !files)");
}

void	do_flist(const char *from, const char *to, const char *rest){
	FILE	*ls_file;
	char	indexfile[MAXLEN];
	char	*s;
	char	*p;

	strcpy(indexfile, currentbot->indexfile );
	if(rest){
		for(p=rest; *p; p++) *p=tolower(toascii(*p));
		sprintf(indexfile, "%s.%s", currentbot->indexfile, rest);
		if((ls_file = openindex(from, indexfile)) == NULL)
		{
			send_to_user(from, "No indexfile for %s", rest);
			return;
		}
	}
	else
		if((ls_file = openindex(from, currentbot->indexfile)) == NULL){
			send_to_user(from, "No indexfile");
			return;
		}

	while((s=get_ftext(ls_file)))
		send_to_user(from, s);
	fclose(ls_file);
}	

void	do_say(const char *from, const char *to, const char *rest){
	/* syntax : !say #chan some thing */
	
	if(rest && strlen(rest) > 1 && rest[0] == '#' && strchr(&rest[1],' ')) {
		to = get_token(&rest, " ");
	};
	
	if(rest) {
		if(to && ischannel(to)) {
			if(is_log_on(to))
				botlog(LOGFILE, "<%s#%s> %s", currentbot->botname, to, rest);
			sendprivmsg(to, "%s", rest);
		}
		else {
			/*       if(	log) */
			/* 	botlog(LOGFILE, "<%s#%s	> %s", currentbot->botname, currentchannel(), rest); */
			
			/*       sendprivmsg(currentchannel(), "%s", rest); */
			send_to_user(from, "On what channel?");
		}
		
	} else
		send_to_user(from, "I don't know what to say.");
	return;
}

void    do_me(const char *from, const char *to, const char *rest){
	/* syntax : !me #chan some thing */
	if(rest && strlen(rest) > 1 && rest[0] == '#' && strchr(&rest[1],' ')) {
		to = get_token(&rest, " ");
	};
  
	if(rest) {
		if(to && ischannel(to)) {
			if(is_log_on(to))
				botlog(LOGFILE, "*%s#%s* %s", currentbot->botname, to, rest);
			sendaction(to, "%s", rest);
		}
		else {
			send_to_user(from, "On what channel?");
		}
		
	}
	else
		send_to_user(from, "I don't know what to do.");
	return;
}


void do_topic(const char *from, const char *to, const char *rest){
	/* syntax : !topic #chan some thing */
	if(rest && strlen(rest) > 1 && rest[0] == '#' && strchr(&rest[1],' ')) {
		to = get_token(&rest, " ");
	};
  
	if(rest) {
		if(to && ischannel(to)) {
			sendtopic(to, rest);
			botlog(LOGFILE, "*%s change le topic de %s en \"%s\" sur l'ordre de %s.", currentbot->botname, to, rest, getnick(from));
		}
		else {
			send_to_user(from, "Dans quel canal?");
		}

	} else
		send_to_user(from, "Quel sujet dois-je mettre?");
	return;
}

void    do_replist(const char *from, const char *to, const char *rest){
	int i;
	if(TailleRep <= 0)
		send_to_user(from, "Il n'y a aucune réponse.");
	else {
		if(rest) {
			SKIPSPC(rest);
			
			if(rest[0]) {
				
				for(i=0; i < TailleRep; i++) {
					if(!fnmatch(rest, TableDesReponses[i]->NomStimulus, FNM_CASEFOLD)) {
						send_to_user(from, "Réponse numero %d:", i+1);
						send_to_user(from, "%s\t\"%s\"",
									 TableDesReponses[i]->NomStimulus,
									 TableDesReponses[i]->Reponse, GetNick(from));
						send_to_user(from, "%s\t%s", TableDesReponses[i]->Auteur,
									 TableDesReponses[i]->Canal);
					}
				} 
				
			} else {
				for(i=0; i < TailleRep; i++) {
					send_to_user(from, "Réponse numero %d:", i+1);
					send_to_user(from, "%s\t\"%s\"",
								 TableDesReponses[i]->NomStimulus,
								 TableDesReponses[i]->Reponse, GetNick(from));
					send_to_user(from, "%s\t%s", TableDesReponses[i]->Auteur,
								 TableDesReponses[i]->Canal);
				}
			}
		}
		else {
			for(i=0; i<TailleRep; i++) {
				send_to_user(from, "Réponse numero %d:", i+1);
				send_to_user(from, "%s\t\"%s\"",
							 TableDesReponses[i]->NomStimulus,
							 TableDesReponses[i]->Reponse, GetNick(from));
				send_to_user(from, "%s\t%s", TableDesReponses[i]->Auteur,
							 TableDesReponses[i]->Canal);
			}
		}
	}
}

void    do_stimlist(const char *from, const char *to, const char *rest){
	int i;

	if(TailleStim <= 0)
		send_to_user(from, "Il n'y a aucun stimulus.");
	else {
		if(rest) {
			SKIPSPC(rest);
			
			if(rest[0]) {

				for(i=0; i < TailleStim; i++) {
					if(!fnmatch(rest, TableDesStimuli[i]->NomStimulus, FNM_CASEFOLD)) {
						send_to_user(from, "Stimulus numéro %	d:", i+1);
						send_to_user(from, "\"%s\"\t%s",
									  TableDesStimuli[i]->Stimulus,
									  TableDesStimuli[i]->NomStimulus);
						send_to_user(from, "%s\t%s", TableDesStimuli[i]->Auteur,
									  (TableDesStimuli[i]->Actif?"actif":"inactif"));
					}
				} 
				
			} else {
				for(i=0; i < TailleStim; i++) {
					send_to_user(from, "Stimulus numéro %d:", i+1);
					send_to_user(from, "\"%s\"\t%s",
								  TableDesStimuli[i]->Stimulus, GetNick(from));
					//TableDesStimuli[i]->NomStimulus,
					send_to_user(from, "%s\t%s", TableDesStimuli[i]->Auteur,
								  TableDesStimuli[i]->Actif?"actif":"inactif");
				}
			}
		}
		else {
			for(i=0; i<TailleStim; i++) {
				send_to_user(from, "Stimulus numéro %d:", i+1);
				send_to_user(from, "\"%s\"\t%s",
							  TableDesStimuli[i]->Stimulus, GetNick(from));
				//TableDesStimuli[i]->NomStimulus,
				send_to_user(from, "%s\t%s", TableDesStimuli[i]->Auteur,
							  TableDesStimuli[i]->Actif?"actif":"inactif");
			}
		}
	}
}

void    do_repdel(const char *from, const char *to, const char *rest) {
	if(rest) {
		int numero = atoi(rest);

		if(numero) {
			if(numero > TailleRep || numero < 0) {
				char **Phrase;
				Phrase = malloc(3 * sizeof(char *));
     
				Phrase[0] = strdup("Il faut me donner un vrai numéro de réponse, %s.");
				Phrase[1] = strdup("Ce n'est pas un vrai numéro de réponse ça, %s. Pour l'avoir, utiliser REPLIST");
				Phrase[2] = strdup("Qu'est-ce que c'est que ça? Il ne me faut que le numéro de la réponse à détruire, %s, d'accord?");
				Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
				return;
			}
			if(rellevel(from) >= rellevel(TableDesReponses[numero]->Auteur)) {
				send_to_user(from, "Réponse à détruire: \"%s\".", TableDesReponses[numero]->Reponse);
				
				if(SupprimeRep(numero))
					send_to_user(from, "La réponse numéro %d a été supprimée de la liste des réponses.", numero);
				else
					send_to_user(from, "Je n'ai pas réussi à supprimer la réponse numéro %d.", numero);
			} else {
				char **Phrase;
				Phrase = malloc(3 * sizeof(char *));
     
				Phrase[0] = strdup("Je fais plus confiance à l'auteur de la réponse qu'à toi %s. Dommage.");
				Phrase[1] = strdup("Demande à quelqu'un en qui j'ai encore plus confiance, %s. Je ne suis pas sûr que l'auteur apprécierait qu'on détruise cette réponse.");
				Phrase[2] = strdup("Pourquoi devrais-je détruire cette réponse, %s? Son auteur m'est encore plus sympathique que toi.");
				Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
			}
		} else {
			char **Phrase;
			Phrase = malloc(3 * sizeof(char *));
     
			Phrase[0] = strdup("Il faut me donner un numéro de réponse pour que je puisse le détruire, %s. Et rien d'autre.");
			Phrase[1] = strdup("Ce n'est pas un numéro de réponse ça, %s. Pour l'avoir, utiliser REPLIST");
			Phrase[2] = strdup("Qu'est-ce que c'est que ça? Il ne me faut que le numéro de la réponse à détruire, hmmm?");
			Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
		}
	} else {
		char **Phrase;
		Phrase = malloc(3 * sizeof(char *));
		
		Phrase[0] = strdup("Il faut me donner un numéro de réponse pour que je puisse la détruire, %s. (Utiliser REPLIST).");
		Phrase[1] = strdup("Il manque le numéro de la réponse, %s. Regarde REPLIST.");
		Phrase[2] = strdup("Et le numéro de la réponse à détruire, %s, hmmm?");
		Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
	}
}

void    do_stimdel(const char *from, const char *to, const char *rest) {
	if(rest) {
		int numero = atoi(rest);

		if(numero) {
			if(numero > TailleStim || numero < 0) {
				char **Phrase;
				Phrase = malloc(3 * sizeof(char *));
     
				Phrase[0] = strdup("Il faut me donner un vrai numéro de stimulus, %s.");
				Phrase[1] = strdup("Ce n'est pas un vrai numéro de stimulus ça, %s. Pour l'avoir, utilise STIMLIST");
				Phrase[2] = strdup("Qu'est-ce que c'est que ça? Il ne me faut que le numéro du stimulus à détruire, %s, d'accord?");
				Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
				return;
			}

			if(rellevel(from) >= rellevel(TableDesStimuli[numero]->Auteur)) {
				send_to_user(from, "Stimulus à détruire: \"%s\".", TableDesStimuli[numero]->Stimulus);
				
				if(SupprimeStim(numero))
					send_to_user(from, "Le stimulus numéro %d a été supprimé de la liste des stimuli.", numero);
				else
					send_to_user(from, "Je n'ai pas réussi à supprimer le stimulus numéro %d.", numero);
			} else {
				char **Phrase;
				Phrase = malloc(3 * sizeof(char *));
				
				Phrase[0] = strdup("Je fais plus confiance à l'auteur du stimulus qu'à toi %s. Dommage.");
				Phrase[1] = strdup("Demande à quelqu'un en qui j'ai encore plus confiance, %s. Je ne suis pas sûr que l'auteur apprécierait qu'on détruise ce stimulus.");
				Phrase[2] = strdup("Pourquoi devrais-je détruire ce stimulus, %s? Son auteur m'est encore plus sympathique que toi.");
				Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
			}
		} else {
			char **Phrase;
			Phrase = malloc(3 * sizeof(char *));
     
			Phrase[0] = strdup("Il faut me donner un numéro de stimulus pour que je puisse le détruire, %s. Et rien d'autre.");
			Phrase[1] = strdup("Ce n'est pas un numéro de stimulus ça, %s. Pour l'avoir, utilise STIMLIST");
			Phrase[2] = strdup("Qu'est-ce que c'est que ça? Il ne me faut que le numéro du stimulus à détruire, hmmm?");
			Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
		}
	} else {
		char **Phrase;
		Phrase = malloc(3 * sizeof(char *));
		
		Phrase[0] = strdup("Il faut me donner un numéro de stimulus pour que je puisse le détruire, %s. (Utiliser STIMLIST).");
		Phrase[1] = strdup("Il manque le numéro du stimulus, %s. Regarde STIMLIST.");
		Phrase[2] = strdup("Et le numéro du stimulus à détruire, %s, hmmm?");
		Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
	}
}


void	do_do(const char *from, const char *to, const char *rest){
	if(rest)
		send_to_server(rest);
	else
		send_to_user(from, "What do you want me to do?");
	return;
}

void	show_channels(const char *from, const char *to, const char *rest){
	show_channellist(from);
	return;
}

void	do_join(const char *from, const char *to, const char *rest){
	char *chan = NULL;
	if(rest)
		chan = get_token(&rest, " ");

	if(chan)
		if(join_channel(chan, "", "", rest, TRUE))
			send_to_user(from, "I am now on %s, using %s encoding", chan, strlen(rest)?rest:"default");
	    else
			send_to_user(from, "I failed joining %s", chan);
	else
		send_to_user(from, "What channel do you want me to join?");
	return;
}

void	do_leave(const char *from, const char *to, const char *rest){
	if(rest)
		leave_channel(rest); 
	else
		if(STRCASEEQUAL(currentbot->nick, to))
			leave_channel(currentchannel());
		else
			leave_channel(to);
	return;
}
 
void	do_nick(const char *from, const char *to, const char *rest){
	if(rest){
		if(!isnick(rest)){
			send_to_user(from, "Illegal nickname %s", rest);
			return;
		}	
		strncpy(currentbot->nick, rest, NICKLEN);
		strncpy(currentbot->realnick, rest, NICKLEN);
		sendnick(currentbot->nick); 
	}
	else
		send_to_user(from, "You need to tell me what nick to use");
	return;
}

void	do_die(const char *from, const char *to, const char *rest){
	if( rest != NULL )
		signoff( from, rest );
	else
		signoff( from, "Bye bye!" );
	if(number_of_bots == 0){
		shutdown_lua();
		cleanupcfg();
		exit(0);
	}
}

void	do_quit(const char *from, const char *to, const char *rest){
	quit_all_bots(from, "Quiting all bots.");
	dump_notelist();
	shutdown_lua();
	exit(0);
}

void    do_logon(const char *from, const char *to, const char *rest){
	CHAN_list* Canal;
	/*   log = TRUE; */
  
	SKIPSPC(rest);
	
	if(rest)
		to = rest;
	
	if(to && ischannel(to) && (Canal = search_chan(to))) {
		Canal->log = TRUE;
		send_to_user(from, "Log on %s", to);
	} else {
		send_to_user(from, "%s: canal inconnu", to);
	}
}

void    do_logoff(const char *from, const char *to, const char *rest){
	CHAN_list* Canal;
	/*   log = FALSE; */
  
	SKIPSPC(rest);
	
	if(rest)
		to = rest;

	if(to && ischannel(to) && (Canal = search_chan(to))) {
		Canal->log = FALSE;
		send_to_user(from, "Log off %s", to);
	}  else {
		send_to_user(from, "%s: canal inconnu", to);
	}
}

void    do_msglogon(const char *from, const char *to, const char *rest){
	logging = TRUE;
	send_to_user(from, "Msg log on");
}

void    do_msglogoff(const char *from, const char *to, const char *rest){
  logging = FALSE;
  send_to_user(from, "Msg log off");
}

void    do_comchar(const char *from, const char *to, const char *rest){
	if(rest) {
		SKIPSPC(rest);
		
		if(rest) {
			CommandChar = rest[0];
			sendreply("Le nouveau caractère de commande est '%c'.", CommandChar);
		}
		else {
			sendreply("Quel doit être le nouveau caractère de commande?");
		}
	}
	else
		sendreply("Le caractère de commande actuel est '%c'", CommandChar);
}

void    show_whois(const char *from, const char *to, const char *rest){
	//char *nuh;

	if(rest == NULL){
		send_to_user(from, "Please specify a user");
		return;
	} 

	/* utilise rest directement ou username ? */
	/*
	if((nuh=username(rest))==NULL){
		send_to_user(from, "%s is not on this channel!", rest);
		return;
	}
	*/
	send_to_user(from, "%s's levels are:", rest);
	send_to_user(from, "-- User -+- Shit -+- Protect --+- Relation -+" );
	send_to_user(from, "    %3d  |   %3d  |      %3d   |       %3d  |",
				 userlevel(rest),
				 shitlevel(rest),
				 protlevel(rest),
				 rellevel(rest));
	return;
}

void	show_nwhois(const char *from, const char *to, const char *rest){
	char	*nuh;

	if( rest == NULL ){
		send_to_user( from, "Please specify a nickname" );
		return;
	}
	if((nuh=username(rest))==NULL){
		send_to_user(from, "%s is not on this channel!", rest);
		return;
	}

	send_to_user(from, "USERLIST:-------------Matching pattern(s) + level");
	show_lvllist( currentbot->lists->opperlist, from, nuh);
	send_to_user(from, "SHITLIST:---------------------------------+");
	show_lvllist( currentbot->lists->shitlist, from, nuh);
	send_to_user(from, "PROTLIST:---------------------------------+");
	show_lvllist( currentbot->lists->protlist, from, nuh);
	send_to_user(from, "RELLIST:----------------------------------+");
	show_lvllist( currentbot->lists->rellist, from, nuh);
	send_to_user(from, "End of nwhois-----------------------------+");
}

void	do_nuseradd(const char *from, const char *to, const char *rest){
	char	*newuser;
	char	*newlevel;
	char	*nuh;		/* nick!@user@host */

	if((newuser = get_token(&rest, " ")) == NULL){
		send_to_user( from, "Who do you want me to add?" );
		return;
	}
	if((nuh=username(newuser))==NULL){
		send_to_user(from, "%s is not on this channel!", newuser);
		return;
	}
	if((newlevel = get_token(&rest, " ")) == NULL){
		send_to_user( from, "What level should %s have?", newuser );
		return;
	}

	if(atoi(newlevel) < 0)
		send_to_user(from, "level should be >= 0!");
	else if(atoi(newlevel) >= userlevel(from))
		send_to_user( from, "Sorry bro, can't do that!" );
	else if(userlevel(from) < userlevel(nuh))
		send_to_user(from, "Sorry, %s has a higher level", newuser);
	else{
		char	*nick;
		char	*user;
		char	*host;
		char	*domain;
		char	userstr[MAXLEN];

		nick=get_token(&nuh, "!");
		user=get_token(&nuh,"@");
		if(*user == '~' || *user == '#') user++;
		host=get_token(&nuh, ".");
		domain=get_token(&nuh,"");

		sprintf(userstr, "*!*%s@*%s", user, domain?domain:host);
		send_to_user( from, "User %s added with access %d as %s", 
					  newuser, atoi( newlevel ), userstr );
		add_to_levellist( currentbot->lists->opperlist, 
						  userstr, atoi( newlevel ) );
	}
	return;
}

void	do_useradd(const char *from, const char *to, const char *rest){
	char	*newuser;
	char	*newlevel;

	if((newuser = get_token(&rest, " ")) == NULL){
		send_to_user( from, "Who do you want me to add?" );
		return;
	}
	if((newlevel = rest) == NULL){
		send_to_user( from, "What level should %s have?", newuser );
		return;
	}
	if(atoi(newlevel) < 0)
		send_to_user(from, "level should be >= 0!");
	else if(atoi( newlevel) >= userlevel(from))
		send_to_user(from, "Sorry bro, can't do that!");
	else if(userlevel(from) < userlevel(newuser))
		send_to_user(from, "Sorry, %s has a higher level", newuser);
	else{
		send_to_user( from, "User %s added with access %d", 
					  newuser, atoi(newlevel));
		add_to_levellist(currentbot->lists->opperlist, newuser, atoi(newlevel));
	}
	return;
}                   

void    do_userwrite(const char *from, const char *to, const char *rest){
	if(!write_lvllist(currentbot->lists->opperlist, 
					  currentbot->lists->opperfile,
					  SORT_DESC))
		send_to_user(from, "Userlist could not be written to file %s", 
					 currentbot->lists->opperfile);
	else
		send_to_user(from, "Userlist written to file %s", 
					 currentbot->lists->opperfile);
}

void    do_locwrite(const char *from, const char *to, const char *rest){
	if(!SauveLocuteurs(currentbot->lists->ListeLocuteurs, 
					   currentbot->lists->locuteurfile))
		send_to_user(from, "Locuteurs list could not be written to file %s", 
					 currentbot->lists->locuteurfile);
	else
		send_to_user(from, "Loclist written to file %s", 
					 currentbot->lists->locuteurfile);
}

void	do_userdel(const char *from, const char *to, const char *rest){
	if(rest){
		if(userlevel(from) < userlevel(rest))
			send_to_user(from, "%s has a higer level.. sorry",
						 rest);
		else if(!remove_from_levellist(currentbot->lists->opperlist,
									   rest))
			send_to_user(from, "%s has no level!", rest);
		else
			send_to_user(from, "User %s has been deleted", rest);
	}
	else
		send_to_user(from, "Who do you want me to delete?");
	return;
}

void	do_nshitadd(const char *from, const char *to, const char *rest){
	char	*newuser;
	char	*newlevel;
	char	*nuh;		/* nick!@user@host */

	if((newuser = get_token(&rest, " ")) == NULL){
		send_to_user(from, "Who do you want me to add?");
		return;
	}
	if((nuh=username(newuser))==NULL){
		send_to_user(from, "%s is not on this channel!", newuser);
		return;
	}
	if((newlevel = get_token(&rest, " ")) == NULL ){
		send_to_user(from, "What level should %s have?", newuser);
		return;
	}

	if(atoi(newlevel) < 0)
		send_to_user(from, "level should be >= 0!");
	else if(atoi(newlevel) > userlevel(from))
		send_to_user(from, "Sorry bro, can't do that!");
	else if(userlevel(from) < userlevel(nuh))
		/* This way, someone with level 100 can't shit someone with level USERLVL_BOTOWNER */
		send_to_user(from, "Sorry, %s has a higher level", newuser);
	else{
		char	*nick;
		char	*user;
		char	*host;
		char	*domain;
		char	userstr[MAXLEN];
		
		nick=get_token(&nuh, "!");
		user=get_token(&nuh,"@");
		if(*user == '~' || *user == '#')
			user++;
		host=get_token(&nuh, ".");
		domain=get_token(&nuh,"");

		sprintf(userstr, "*!*%s@*%s", user, domain?domain:host);
		send_to_user(from, "User %s shitted with access %d as %s", 
					 newuser, atoi(newlevel), userstr);
		add_to_levellist(currentbot->lists->shitlist, userstr, atoi(newlevel));
	}
	return;
}                   

void	do_shitadd(const char *from, const char *to, const char *rest){
	char	*newuser;
	char	*newlevel;

	if((newuser = get_token(&rest, " ")) == NULL){
		send_to_user(from, "Who do you want me to add?");
		return;
	}
	if((newlevel = rest) == NULL){
		send_to_user(from, "What level should %s have?", newuser);
		return;
	}
	if(atoi(newlevel) < 0)
		send_to_user(from, "level should be >= 0!");
	else{
		send_to_user(from, "User %s shitted with access %d", newuser, 
		             atoi(newlevel));
		add_to_levellist(currentbot->lists->shitlist, newuser, atoi(newlevel));
	} 
	return;
}                   

void	do_shitwrite(const char *from, const char *to, const char *rest){
	if(!write_lvllist(currentbot->lists->shitlist, 
					  currentbot->lists->shitfile,
					  SORT_DESC))
		send_to_user(from, "Shitlist could not be written to file %s", 
					 currentbot->lists->shitfile);
	else
		send_to_user(from, "Shitlist written to file %s", 
					 currentbot->lists->shitfile);
}

void	do_shitdel(const char *from, const char *to, const char *rest){
	if(rest){
		if(!remove_from_levellist(currentbot->lists->shitlist, rest))
			send_to_user(from, "%s is not shitted!", rest);
		else
			send_to_user(from, "User %s has been deleted", rest);
	}
	else
		send_to_user(from, "Who do you want me to delete?");
	return;
}

void	do_nprotadd(const char *from, const char *to, const char *rest){
	char	*newuser;
	char	*newlevel;
	char	*nuh;		/* nick!@user@host */

	if((newuser = get_token(&rest, " ")) == NULL){
		send_to_user(from, "Who do you want me to add?");
		return;
	}
	if((nuh=username(newuser))==NULL){
		send_to_user(from, "%s is not on this channel!", newuser);
		return;
	}
	if((newlevel = get_token(&rest, " ")) == NULL){
		send_to_user(from, "What level should %s have?", newuser);
		return;
	}

	if(atoi(newlevel) < 0)
		send_to_user(from, "level should be >= 0!");
	else if(atoi(newlevel) > 100)
		send_to_user(from, "yeah, right! You're shitted.");
	else{
		char	*nick;
		char	*user;
		char	*host;
		char	*domain;
		char	userstr[MAXLEN];

		nick=get_token(&nuh, "!");
		user=get_token(&nuh,"@");
		if(*user == '~' || *user == '#')
			user++;
		host=get_token(&nuh, ".");
		domain=get_token(&nuh,"");

		sprintf(userstr, "*!*%s@*%s", user, domain?domain:host);
		send_to_user(from, "User %s protected with access %d as %s", 
					 newuser, atoi(newlevel), userstr);
		add_to_levellist(currentbot->lists->protlist, userstr, atoi(newlevel));
	}
	return;
}                   

void    do_protadd(const char *from, const char *to, const char *rest){
	char    *newuser;
	char    *newlevel;
	
	if((newuser = get_token(&rest, " ")) == NULL){
		send_to_user(from, "Who do you want me to add?");
		return;
	}
	if((newlevel = rest) == NULL ){
		send_to_user(from, "What level should %s have?", newuser);
		return;
	}
	if(atoi(newlevel) < 0)
		send_to_user(from, "level should be >= 0!");
	else{
		send_to_user(from, "User %s protected with access %d", newuser,
					 atoi(newlevel));
		add_to_levellist(currentbot->lists->protlist, newuser, atoi(newlevel));
	}
	return;
}

void    do_protwrite(const char *from, const char *to, const char *rest){
	if(!write_lvllist(currentbot->lists->protlist, 
					  currentbot->lists->protfile,
					  SORT_DESC))
		send_to_user(from, "Protlist could not be written to file %s", 
					 currentbot->lists->protfile);
	else
		send_to_user(from, "Protlist written to file %s", 
					 currentbot->lists->protfile);
}

void    do_protdel(const char *from, const char *to, const char *rest){
	if(rest){
		if(!remove_from_levellist(currentbot->lists->protlist, rest))
			send_to_user(from, "%s is not protected!", rest);
		else
			send_to_user(from, "User %s has been deleted", rest);
	}
	else
		send_to_user(from, "Who do you want me to delete?");
	return;
}

void    do_nreladd(const char *from, const char *to, const char *rest){
	char *newrel;
	char *newlevel;
	char *nuh;		/* nick!@user@host */

	if((newrel = get_token(&rest, " ")) == NULL){
		send_to_user( from, "Who do you want me to add or modify?" );
		return;
    }
	if((nuh=username(newrel))==NULL){
		send_to_user(from, "%s is not on this channel!", newrel);
		return;
    }
  if((newlevel = get_token(&rest, " ")) == NULL){
      send_to_user( from, "What level should %s have?", newrel );
      return;
  }
  
  {
	  
    /*char	*nick;
    char	*user;
    char	*host;
    char	*domain;*/
    char	*userstr;
    /*char i;
    int numbers = FALSE;*/
    int inewlevel;
    int ioldlevel;
    /*
    nick=get_token(&nuh, "!");
    user=get_token(&nuh,"@");
    if(*user == '~' || *user == '#') user++;
    host=get_token(&nuh, ".");
    domain=get_token(&nuh,"");

    for(i='0'; i<='9' && !numbers; i++)
      if(strchr(domain, i))
	numbers = TRUE;

    for(i='a'; i<='z' && numbers; i++)
      if(strchr(domain, i))
	numbers = FALSE;
    
    for(i='A'; i<= 'Z' && numbers; i++)
      if(strchr(domain, i))
	numbers = FALSE;

    if(!numbers)
      sprintf(userstr, "%s!*%s@*%s", nick, user, domain?domain:host);
    else {
      domain = get_token(&domain, ".");
      sprintf(userstr, "%s!*%s@%s.%s*", nick, user, host, domain?domain:"");
    }
    */

    userstr = NickUserStr(from);

#ifdef DBUG
	fprintf(stderr, "userstr:'%s', nuh:'%s', from:'%s', newrel:'%s'\n", userstr, nuh, from, newrel);
#endif //DBUG

    inewlevel = atoi(newlevel);
    ioldlevel = rellevel(rest);
    /* Si le demandeur est maitre du bot */
    if(userlevel(from) > USERLVL_POWERUSER) {
#ifdef DBUG
		fprintf(stderr, "Demandeur maître du bot\n");
#endif //DBUG
      /* Si l'utilisateur n'existe pas encore */
		if(!exist_userhost(currentbot->lists->rellist, newrel)) {
			send_to_user( from, "Rel %s added with access %d as %s", 
						  newrel, inewlevel, nuh );
			add_to_levellist( currentbot->lists->rellist, 
							  nuh, inewlevel );
		}
		else {
			send_to_user( from, "Rel %s access changed to %d", 
						  newrel, inewlevel );
			add_to_levellist( currentbot->lists->rellist, nuh, inewlevel );
		}
    }
    /* Si le demandeur n'est pas maitre du bot */
    else {
#ifdef DBUG
		fprintf(stderr, "Demandeur NON maître du bot: %s\n", from);
#endif //DBUG
		/* Si le niveau demande est superieur au niveau actuel */
		if(inewlevel >= ioldlevel) {
#ifdef DBUG
			fprintf(stderr, "Niveau demandé: %d, Niveau actuel: %d\n", inewlevel, ioldlevel);
#endif //DBUG
			/* Si le demandeur est la cible */
			if(!fnmatch( nuh, from, FNM_CASEFOLD )) {
#ifdef DBUG
				fprintf(stderr, "Cible: %s = Demandeur: %s\n", nuh, from);
#endif //DBUG
				/* - */
				/* Si l'utilisateur n'existe pas encore */
				if(!exist_userhost(currentbot->lists->rellist, newrel)) {
					send_to_user( from, "Rel %s added with access %d as %s", 
								  from, ioldlevel - (inewlevel - ioldlevel), userstr );
					add_to_levellist( currentbot->lists->rellist, 
									  NickUserStr(from),
									  ioldlevel - ( inewlevel - ioldlevel) );
				}
				else {
					send_to_user( from, "Rel %s access changed to %d", 
								  from, ioldlevel - (inewlevel-ioldlevel) );
					add_to_levellist( currentbot->lists->rellist, NickUserStr(from),
									  ioldlevel-(inewlevel-ioldlevel) );
				}
			}
			/* Si le demandeur n'est pas la cible */
			else {
#ifdef DBUG
				fprintf(stderr, "Cible: %s != Demandeur: %s\n", userstr, from);
#endif //DBUG
				/* Si le demandeur est sympathique */
				if(rellevel(from) >= SYMPA_LVL) {
					/* Si la cible n'est pas antipathique */
					if(ioldlevel >= 0) {
						/* +1 */
						/* Si l'utilisateur n'existe pas encore */
						if(!exist_userhost(currentbot->lists->rellist, newrel)) {
							send_to_user( from, "Rel %s added with access %d as %s", 
										  newrel, ioldlevel+1, userstr );
							add_to_levellist( currentbot->lists->rellist, 
											  userstr, ioldlevel+1 );
						}
						else {
							send_to_user( from, "Rel %s access changed to %d", 
										  newrel, ioldlevel+1 );
							add_to_levellist( currentbot->lists->rellist, newrel, ioldlevel+1 );
						}
					}
				}
				/* Si le demandeur n'est pas sympathique */
				else {
					/* Demandeur -, cible -1 */
					
				}
			}
		}
		/* Si le niveau demande est inferieur au niveau actuel */
		else {
			/* Si le demandeur est la cible */
			if(!fnmatch( userstr, from, FNM_CASEFOLD )) {
				/* OK */
			}
			/* Si le demandeur n'est pas la cible */
			else {
				/* Si le demandeur est sympathique */
				if(rellevel(from) >= SYMPA_LVL) {
					/* Si la cible est plus sympathique */
					if(rellevel(from) <= ioldlevel) {
						/* non */
					}
					/* Si le demandeur a un niveau superieur a celui de sa cible */
					else {
						/* OK */
					}
				}
				/* Si le demandeur n'est pas sympathique */
				else {
					if(ioldlevel >= 0) {
						/* Demandeur -, kick(demandeur) */
					}
					else {
						/* Demandeur - */
					}
				}
			}
		}
    }

    if(userstr)
		free(userstr);
  }
  return;
}

void    do_reladd(const char *from, const char *to, const char *rest){
	char *newrel;
	char *newlevel;

	if((newrel = get_token(&rest, " ")) == NULL){
		send_to_user( from, "Who do you want me to add?" );
		return;
    }
	if((newlevel = rest) == NULL){
		send_to_user( from, "What level should %s have?", newrel );
		return;
    }
	if(atoi( newlevel) > rellevel(from))
		send_to_user(from, "Sorry bro, can't do that!");
	else if(rellevel(from) < rellevel(newrel))
		send_to_user(from, "Sorry, %s has a higher level", newrel);
	else if(atoi(newlevel)-rellevel(newrel) < -SYMPA_LVL)
		send_to_user(from, "C'est pas gentil ça, %s!", newrel);
	else{
		send_to_user( from, "Rel %s added with access %d", 
					  newrel, atoi(newlevel));
		add_to_levellist(currentbot->lists->rellist, newrel, atoi(newlevel));
    }
	return;
}

void    do_reldel(const char *from, const char *to, const char *rest){
	char **RepPos, **RepNeg;
  
	if(rest) {
		if(rellevel(from) < rellevel(rest)) {
			RepPos = malloc(sizeof(char *) * 3);
			RepPos[0] = strdup("Désolé, ton niveau est inférieur.");
			RepPos[1] = strdup("Il faut avoir un niveau supérieur à celui qu'on veut détruire, %s.");
			RepPos[2] = strdup("C'est impossible, je t'aime moins que lui, %s.");
			
			RepNeg = malloc(1 * sizeof(char *));
			RepNeg[0] = strdup("Non. Je te l'inderdis, %s.");
			
			Repondre(from, to, -1, 3, RepPos, -2, 1, RepNeg);
		}
		else if(!remove_from_levellist(currentbot->lists->rellist, rest)) {
			char Phrase[MAXLEN];
      
			RepPos = malloc(3 * sizeof(char *));

			sprintf(Phrase, "%s n'est pas dans la liste de mes connaissances.", rest);
			RepPos[0] = strdup(Phrase);
			sprintf(Phrase, "Je ne connais pas \"%s\".", rest);
			RepPos[1] = strdup(Phrase);
			RepPos[2] = strdup("Impossible, %s, je ne l'ai pas dans ma liste!");

			RepNeg = malloc(1 * sizeof(char *));
			RepNeg[0] = strdup("Même si je le pouvais, %s, je ne le ferais pas. Mais c'est impossible: je n'ai pas trouvé ce nom dans ma liste.");

			Repondre(from, to, 0, 3, RepPos, -1, 1, RepNeg);
		}
		else {
			char Phrase[MAXLEN];
			
			RepPos = malloc(3 * sizeof(char *));
			sprintf(Phrase, "%s a été détruit de ma liste de relation.", rest);
			RepPos[0] = strdup(Phrase);
			sprintf(Phrase, "J'ai effacé \"%s\" de mes connaissances.", rest);
			RepPos[1] = strdup(Phrase);
			sprintf(Phrase, "J'oublie que j'ai déjà parlé à \"%s\"", rest);
			RepPos[2] = strdup(Phrase);
			
			RepNeg = malloc(1 * sizeof(char *));
			RepNeg[0] = strdup("Par quel sortilège es-tu parvenu à me faire faire ça, %s?");
	  
			Repondre(from, to, 0, 3, RepPos, -1, 1, RepNeg);
		}
	}
	else {
		RepPos = malloc(3 * sizeof(char *));
		RepPos[0] = strdup("C'est bien gentil, %s, mais j'aimerais savoir qui je dois oublier.");
		RepPos[1] = strdup("Étourdi, va, %s: il faut me donner un nom complet à oublier!");
		RepPos[2] = strdup("C'est bien mon petit %s, ça: il oublie de me dire qui il faut oublier!");
		
		RepNeg = malloc(1 * sizeof(char *));
		RepNeg[0] = strdup("Ça m'étonne pas de toi, %s: tu ne sais pas qu'il faut dire qui je dois oublier?");
    
		Repondre(from, to, 0, 3, RepPos, 0, 1, RepNeg);
	}
	return;
}


void    do_relwrite(const char *from, const char *to, const char *rest){
	if(!write_lvllist(currentbot->lists->rellist,
					  currentbot->lists->relfile,
					  SORT_DESC))
		send_to_user(from, "Rellist could not be written to file %s", 
					 currentbot->lists->relfile);
	else
		send_to_user(from, "Rellist written to file %s", 
					 currentbot->lists->relfile);
}

void	do_banuser(const char *from, const char *to, const char *rest){
	char	*user_2b_banned;
	char	*channel;

	if(ischannel(to))
		channel = to;
	else
		channel = currentchannel();
		
	if(rest)
		if((user_2b_banned = username(rest)))
			ban_user(user_2b_banned, channel);
		else
			send_to_user(from, "%s not on this channel", rest);
	else
		send_to_user( from, "No." );
}

void	do_showusers(const char *from, const char *to, const char *rest){
	char    *channel;
	
	if( (channel = strtok( rest, " " )) == NULL )
		show_users_on_channel( from, currentchannel());
	else
		show_users_on_channel( from, channel );
}

void    do_showbots(const char *from, const char *to, const char *rest){
	char *channel;
  
	if((channel = strtok(rest, " ")) == NULL)
		show_botlist(currentbot->botlist, to, from, to);
	else
		show_botlist(currentbot->botlist, channel, from, to);
}

void    do_botadd(const char *from, const char *to, const char *rest){
	char *newbot;
	char *newchannel;
  
	if((newbot = get_token(&rest, " ")) == NULL) {
		send_to_user(from, "Quel bot faut-il ajouter ?");
		return;
	}

	if((newchannel = rest) == NULL) {
		send_to_user(from, "Dans quel(s) canal(-aux) faut il mettre %s?", newbot);
		return;
	}
  
	add_to_botlist(currentbot->botlist, newbot, newchannel);
	send_to_user(from, "Voilà qui est fait : %s est ajouté dans %s.", newbot, newchannel);
}

void    do_showlocs(const char *from, const char *to, const char *rest){
	MontreLocuteurs(currentbot->lists->ListeLocuteurs, from, rest);
}

void	do_massop(const char *from, const char *to, const char *rest){
	char	*op_pattern;

	if( ( op_pattern = strtok( rest, " " ) ) == NULL )
		send_to_user( from, "Please specify a pattern" );
	else
		channel_massop( ischannel( to ) ? to : currentchannel(), 
						op_pattern );    
	return;
}

void    do_massdeop(const char *from, const char *to, const char *rest){
	char    *op_pattern;
 
	if( ( op_pattern = strtok( rest, " " ) ) == NULL )
		send_to_user( from, "Please specify a pattern" );
	else
		channel_massdeop( ischannel( to ) ? to : currentchannel(), 
						  op_pattern );
	return;
}

void	do_masskick(const char *from, const char *to, const char *rest){
	char    *op_pattern;

	if( ( op_pattern = strtok( rest, " " ) ) == NULL )
		send_to_user( from, "Please specify a pattern" );
	else
		channel_masskick( ischannel( to ) ? to : currentchannel(), 
						  op_pattern );
	return;
}

void	do_massunban(const char *from, const char *to, const char *rest){
	char	*channel;

	if((channel = strtok(rest, " "))==NULL)
		channel_massunban(currentchannel());
	else
		channel_massunban(channel);
}
	
void	do_server(const char *from, const char *to, const char *rest){
	int	serv;

	if(rest)
		if(readint(&rest, &serv))
			if(not(change_server(serv)))
				sendreply("Incorrent servernumber!");
			else
				sendreply("Hold on, I'm trying..");
		else
			sendreply("Illegal servernumber!");
	else
		sendreply("Change to which server?");
}

void	show_dir(const char *from, const char *to, const char *rest){
	char	*pattern;

	if(rest)
		while((pattern = get_token(&rest, " "))){
			if(*rest != '-')
				do_ls(from, pattern);
		}
	else
		do_ls(from, "");
}

void	show_cwd(const char *from, const char *to, const char *rest){
	pwd(from);
}

void	do_cd(const char *from, const char *to, const char *rest){
	do_chdir(from, rest?rest:"/");
}

void    show_queue( const char *from, const char *rest, const char *to ){
	do_showqueue();
}

void	do_fork(const char *from, const char *to, const char *rest){
	char	*nick;
	char	*login;

	if(rest && (nick=get_token(&rest, " "))){
		if(!isnick(nick)){
			send_to_user(from, "Illegal nick %s", nick);
			return;
		}
		login= get_token(&rest, " ");
		rest = get_token(&rest, "");
		if(!forkbot(nick, login, rest))
			send_to_user(from, "Sorry, no more bots free");
	}	
	else
		send_to_user(from, "Pls specify nick");
}

void	do_unban(const char *from, const char *to, const char *rest){
	char    *channel;

	if((channel = strtok(rest, " "))==NULL)
		channel_unban(currentchannel(), from);
	else
		channel_unban(channel, from);
}

void	do_kick(const char *from, const char *to, const char *rest){
	char	*nuh;
	char    *Phrase;
	char    *nom;
	char    *Rest, *ALiberer;
	
	if(!rest)
	    return;

	if(strlen(rest)==0)
	    return;

	Phrase = malloc(MAXLEN*sizeof(char));

	Rest = strdup(rest);
	ALiberer = Rest;
	nom = get_token(&Rest, " ");

	if(nom && ((nuh = username(nom)) != NULL))
		if((protlevel(nuh)>=50)&&
		   (shitlevel(nuh)==0)) {
			send_to_user(from, "%s est protégé!", GetNick(nuh));
		} else if(ischannel(to)) {
			sprintf(Phrase, "%s a demandé ce kick.", GetNick(from));
			sendkick(to, rest, Phrase);
		} else {
			sprintf(Phrase, "%s a demandé ce kick", GetNick(from));
			sendkick(currentchannel(), rest, Phrase);
		}
	else
		send_to_user(from, "Qui!?!");

	free(Phrase);
	free(ALiberer);
}

void	do_listdcc(const char *from, const char *to, const char *rest){
	show_dcclist( from );
}

void	do_rehash(const char *from, const char *to, const char *rest){
	rehash = TRUE;
}

void	do_reloadlogic(const char *from, const char *to, const char *rest){
	char *msg = NULL;
	load_lualogic(&msg);
	if(msg){
		sendnotice(getnick(from), "Erreur lors du rechargement de la logique : %s", msg);
		free(msg);
	}
}

void 	giveop(const char *channel, const char *nicks ){
    	sendmode(channel, "+ooo %s", nicks);
}

int 	userlevel(const char *from){
	if(from)
		return(get_level(currentbot->lists->opperlist, from));
	else
		return 0;
}

int 	shitlevel(const char *from){
	if(from)
		return(get_level(currentbot->lists->shitlist, from));
	else
		return 0;
}

int     protlevel(const char *from){
	if(from)
		return(get_level(currentbot->lists->protlist, from));
	else
		return 0;
}

int     rellevel(const char *from){
	int intermediaire;
	int succes = FALSE;

	if(from) {
		intermediaire = get_level_neg(currentbot->lists->rellist, from, &succes);
		if(!succes)
			return DEFAUT_LVL; /* Valeur par defaut */
		else return(intermediaire);
	} else
		return 0;
}

void	ban_user(const char *who, const char *channel)
/*
 * Ban the user as nick and as user (two bans)
 */
{
	char	buf[MAXLEN];	/* make a copy, save original */
	char	*usr = buf;
	char	*nick;
	char	*user;

	strcpy(buf, who);
	nick = get_token(&usr, "!");
	user = get_token(&usr, "@");

	if(*user == '#' || *user == '~')
		user++;
	if(not(*user) || (*user == '*')){/* faking login			*/
		/* we can't ban this user on his login,	*/
		/* and banning the nick isn't 'nuff, so	*/
		/* ban the entire site!			*/
		if(not(strchr(usr, '.')))
			sendmode(channel, "+bb %s!*@* *!*@*%s", nick, usr);
		else
			sendmode(channel, "+bb %s!*@* *!*@*%s", 
				 strchr(usr, '.'));
		return;
	}
	sendmode(channel, "+bb %s!*@* *!*%s@*", nick, user);
}

void	signoff(const char *from, const char *reason){
	char	*fromcpy;

	mstrcpy(&fromcpy, from);	/* something hoses, dunno what */
	if(number_of_bots == 1){
		send_to_user(fromcpy, "No bots left... Terminating");
		if(!SauveStimuli(currentbot->stimfile))
		    send_to_user(fromcpy, "Could not save stimuli.");
		if(!SauveReponses(currentbot->repfile))
		    send_to_user(fromcpy, "Could not save responses.");
		LibereStimuli();
		LibereReponses();
 		send_to_user(fromcpy, "Dumping notes");
		dump_notelist();
	}
	send_to_user(fromcpy, "Saving lists...");
	if(!write_lvllist(currentbot->lists->opperlist, 
					  currentbot->lists->opperfile,
					  SORT_DESC))
		send_to_user(fromcpy, "Could not save opperlist");
	if(!write_lvllist(currentbot->lists->shitlist, 
					  currentbot->lists->shitfile,
					  SORT_DESC))
		send_to_user(fromcpy, "Could not save shitlist");
	if(!write_lvllist(currentbot->lists->protlist, 
					  currentbot->lists->protfile,
					  SORT_DESC))
		send_to_user( fromcpy, "Could not save protlist");
	if(!write_lvllist(currentbot->lists->rellist, 
					  currentbot->lists->relfile,
					  SORT_DESC))
		send_to_user( fromcpy, "Could not save rellist");
	if(!SauveLocuteurs(currentbot->lists->ListeLocuteurs,
						 currentbot->lists->locuteurfile))
		send_to_user(fromcpy, "Could not save loclist");
	if(!write_botlist(currentbot->botlist, currentbot->botfile))
		send_to_user( fromcpy, "Could not save botlist");
	delete_botlist(currentbot->botlist);

	send_to_user(fromcpy, "Bye...");
	free(fromcpy);
	killbot(reason);
}

void	do_apprends(const char *from, const char *to, const char *rest) {

	char chaine[MAXLEN];
	char repinter[MAXLEN];
	char *Stimulus = 0;
	char *NomStimulus = 0;
	char *Reponse = 0;
	char *Canal = 0;
	char *rest_init = 0;
	char *pointeur_init = 0;
	char *inter;

	if(rest) 
		rest_init = strdup(rest);

	pointeur_init = rest_init;
  
	/* C'est un stimulus */
	if(rest && rest[0] == '\"') {
		if(strchr(&rest[1], '\"')) {
      
			Stimulus = get_token(&rest, "\"");
			if(rest) {
	
				NomStimulus = get_token(&rest, " ");

				if(NomStimulus) {
	  
					sprintf(chaine, "Stimulus: \"%s\", Nom: %s", Stimulus, NomStimulus);
					send_to_user(from, chaine);
				}
				else send_to_user(from, "Il manque le nom du stimulus correspondant");
	
			}
			else send_to_user(from, "Manque le stimulus à apprendre");
		}
		else send_to_user(from, "Il manque un guillemet");
    
	}
	else {
		if(rest && strchr(&rest[1], '\"')){
			
			NomStimulus = get_token(&rest_init, " ");
			if(rest_init) {
				Reponse = get_token(&rest_init, "\"");
				
				if(Reponse) {
	  
					if(rest_init && strchr(rest_init, '#')) {
						Canal = strchr(rest_init, '#');
						inter = strchr(Canal, '\n');
						if(inter)
							*inter = '\0';
					} else
						Canal = to;
					
					sprintf(repinter, Reponse, getnick(from), getnick(from), getnick(from), getnick(from), getnick(from));
					sprintf(chaine, "Nom: %s, Réponse: \"%s\"", NomStimulus, repinter /*, getnick(from)*/);
					send_to_user(from, chaine);
					send_to_user(from, "Canal: %s", Canal);
				}
				else send_to_user(from, "Il n'y a même pas de Réponse.");
	
			}
			else send_to_user(from, "Manque le nom stimulus à apprendre.");
		}
		else if(rest) send_to_user(from, "Il manque l'espace séparant le nom du stimulus de la réponse.");
	}

	if(NomStimulus && Stimulus) {
		if(strlen(Stimulus) > 3) {
			if(AjouteStimulus(from, to, Stimulus, NomStimulus))
				send_to_user(from, "Stimulus %s appris.", NomStimulus);
		}
		else send_to_user(from, "Stimulus trop court!");
	}
  
  
	if(NomStimulus && Reponse) {
		if(AjouteReponse(from, (Canal?Canal:to), Reponse, NomStimulus))
			send_to_user(from, "Réponse apprise.");
	}
  
	if(pointeur_init)
		free(pointeur_init);
}

void    do_oublie(const char *from, const char *to, const char *rest){
  int i;
  
  if(!rest || !strlen(rest)) {
	  for(i=0; i <TailleStim; i++) {
		  
		  free(TableDesStimuli[i]->NomStimulus);
		  TableDesStimuli[i]->NomStimulus = 0;
      
		  free(TableDesStimuli[i]->Stimulus);
		  TableDesStimuli[i]->Stimulus = 0;
		  
		  free(TableDesStimuli[i]->Auteur);
		  TableDesStimuli[i]->Auteur = 0;
	  }
    
	  for(i=0; i<TailleRep; i++) {
		  free(TableDesReponses[i]->Reponse);
		  TableDesReponses[i]->Reponse = 0;
		  
		  free(TableDesReponses[i]->NomStimulus);
		  TableDesReponses[i]->NomStimulus = 0;
      
		  free(TableDesReponses[i]->Auteur);
		  TableDesReponses[i]->Auteur = 0;

		  free(TableDesReponses[i]->Canal);
		  TableDesReponses[i]->Canal = 0;
	  }
    
	  if(TailleStim) {
		  free(TableDesStimuli);
		  TailleStim = 0;
		  TableDesStimuli = 0;
	  }
	  
	  if(TailleRep) {
		  free(TableDesReponses);
		  TailleRep = 0;
		  TableDesReponses = 0;
	  }
    
	  send_to_user(from, "Tous les stimuli et réponses appris sont oubliés!");
  }
  else {
	  send_to_user(from, "Attention: !oublie me fait oublier tous les stimuli et les réponses que je connais! Mieux vaut utiliser !desactive.");
  }
  
}

void    do_desactive(const char *from, const char *to, const char *rest){
	int i;
	int desactivation = FALSE;

	if(rest) {
		
		SKIPSPC(rest);
    
		for(i = 0; i < TailleStim; i++) {
			if(strcmp(TableDesStimuli[i]->NomStimulus, rest)== 0 ||
			   strcmp(TableDesStimuli[i]->Auteur, rest) == 0){
				if(rellevel(from)+SYMPA_LVL >= rellevel(TableDesStimuli[i]->Auteur)) {
					send_to_user(from, "Stimulus numéro %d désactivé.", i);
					TableDesStimuli[i]->Actif = FALSE;
					desactivation = TRUE;
				}
				else
					send_to_user(from,
								 "Niveau de relation insuffisant, il faudrait %d, qui est le niveau de %s.",
								 rellevel(TableDesStimuli[i]->Auteur),
								 getnick(TableDesStimuli[i]->Auteur));
			}
		}
	} else
		send_to_user(from, "Je veux bien, moi, mais désactiver quel stimulus?");
	
	if(!desactivation)
		send_to_user(from, "Je n'ai désactivé aucun stimulus.");
}

void    do_active(const char *from, const char *to, const char *rest){
    int i;
    
    if(rest) {
	   
		SKIPSPC(rest);
	   
		for(i = 0; i < TailleStim; i++) {
			if(strcmp(TableDesStimuli[i]->NomStimulus,rest)==0 ||
			   strcmp(TableDesStimuli[i]->Auteur, rest) == 0){
				if(rellevel(from) >= rellevel(TableDesStimuli[i]->Auteur)) {
					send_to_user(from, "Stimulus numéro %d réactivé.", i);
					TableDesStimuli[i]->Actif = TRUE;
				} else
					send_to_user(from,
								 "Niveau de relation insuffisant, il faudrait %d.",
								 rellevel(TableDesStimuli[i]->Auteur));
			}
		}
    } else
		send_to_user(from, "Je veux bien, moi, mais activer quel stimulus?");
}

void    do_stimwrite(const char *from, const char *to, const char *rest){
	if(rest) {
		SKIPSPC(rest);

		if(SauveStimuli(rest))
			send_to_user(from, "%d stimuli sauvegardés", TailleStim);
		else
			send_to_user(from, "Impossible de sauvegarder les stimuli!");
	}
	else {
		if(SauveStimuli(currentbot->stimfile))
			send_to_user(from, "%d stimuli sauvegardés dans %s.",
						 TailleStim,
						 currentbot->stimfile);
	}
}

void    do_stimload(const char *from, const char *to, const char *rest){
	if(rest) {
		SKIPSPC(rest);
    
		if(ChargeStimuli(rest))
			send_to_user(from, "%d stimuli chargés.", TailleStim);
		else
			send_to_user(from, "Impossible de charger les stimuli!");
	}
	else {
		if(ChargeStimuli(currentbot->stimfile))
			send_to_user(from, "%d stimuli de %s chargés.",
						 TailleStim,
						 (currentbot->stimfile));
	}
}


void do_repload(const char *from, const char *to, const char *rest){
	if(rest) {
		SKIPSPC(rest);
    
		if(ChargeReponses(rest))
			send_to_user(from, "%d réponses chargées", TailleRep);
		else
			send_to_user(from, "Impossible de charger les réponses!");
	}
	else {
		if(ChargeReponses(currentbot->repfile))
			send_to_user(from, "%d réponses chargées dans %s.",
						 TailleRep,
						 currentbot->repfile);
	}
}


void    do_repwrite(const char *from, const char *to, const char *rest) {
  
	if(rest) {
		SKIPSPC(rest);
		if(SauveReponses(rest))
			send_to_user(from, "%d réponses sauvegardées", TailleRep);
		else
			send_to_user(from, "Impossible de créer le fichier!");
	}
	else 
		if(SauveReponses(currentbot->repfile))
			send_to_user(from, "%d réponses de %s sauvegardées.",
						 TailleRep,
						 (currentbot->repfile));
}

void do_botload(const char *from, const char *to, const char *rest){
	if(rest) {
		SKIPSPC(rest);
    
		if(readbotdatabase(rest, currentbot->botlist))
			send_to_user(from, "bots chargés depuis %s", rest);
		else
			send_to_user(from, "Impossible de charger les bots depuis %s!", rest);
	}
	else {
		if(readbotdatabase(currentbot->botfile, currentbot->botlist))
			send_to_user(from, "bots chargés depuis %s.",
						 currentbot->botfile);
	}
}

void do_botwrite(const char *from, const char *to, const char *rest) {
	if(rest) {
		SKIPSPC(rest);
		
		if(write_botlist(currentbot->botlist, rest))
			send_to_user(from, "bots sauvegardés dans %s.", rest);
		else
			send_to_user(from, "Impossible de sauvegarder les bots dans %s!", rest);
	} else {
		if(write_botlist(currentbot->botlist, currentbot->botfile))
			send_to_user(from, "bots sauvegardés dans %s.", currentbot->botfile);
	}
}

void do_botdel(const char *from, const char *to, const char *rest) {
	if(rest) {
		int numero = atoi(rest);
		
		if(numero) {
			if(remove_bot(currentbot->botlist, numero))
				send_to_user(from, "Le bot numéro %d a été supprimé de la liste des bots.", numero);
			else
				send_to_user(from, "Je n'ai pas réussi à supprimer le bot numéro %d.", numero);
		} else {
			char **Phrase;
			Phrase = malloc(3 * sizeof(char *));
			Phrase[0] = strdup("Il faut me donner un numéro de bot pour que je puisse le détruire, %s. Et rien d'autre.");
			Phrase[1] = strdup("Ce n'est pas un numéro de bot, %s.");
			Phrase[2] = strdup("Qu'est-ce que c'est que ça? Il ne me faut que le numéro du bot à détruire, hmmm?");
			Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
		}
	} else {
		char **Phrase;
		Phrase = malloc(3 * sizeof(char *));
		Phrase[0] = strdup("Il faut me donner un numéro de bot pour que je puisse le détruire, %s.");
		Phrase[1] = strdup("Il manque le numéro de bot, %s.");
		Phrase[2] = strdup("Et le numéro du bot à détruire, hmmm?");
		Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
	}
}

void    do_fuck(const char *from, const char *to, const char *who) {
	char **Phrase;
	char ch[MAXLEN];
	
	if(who) {
    
		Phrase = malloc(3 * sizeof(char *));
		sprintf(ch, "%s: %s voudrait que je t'encule! Il est mal élevé, hein?", who, GetNick(from));
		Phrase[0] = strdup(ch);
		sprintf(ch, "Pourquoi tu veux que je sodomise %s, c'est pas gentil %s!", who, GetNick(from));
		Phrase[1] = strdup(ch);
		Phrase[2] = strdup("Je réprouve la sodomie, %s.");
		Repondre(from, to, -1, 3, Phrase, 0, 0, 0);
	}
	else {
		Phrase =  malloc(3 * sizeof(char *));
		Phrase[0] = strdup("Qui veux-tu que je sodomise, %s?");
		Phrase[1] = strdup("Ça te prends souvent de donner des commandes sans argument, %s?");
		Phrase[2] = strdup("Qui, %s?");

		Repondre(from, to, -1, 3, Phrase, 0, 0, 0);
	}
}

void  do_ident(char *from, char *canal, char *pass) {
	FILE *fp;
	char usrhost[MAXLEN];
	char password[MAXLEN];
	char s[MAXLEN];
	char *p;
	int found = FALSE;
	locuteur *Locuteur = 0;
	char *NUS = NickUserStr(from);

	if((fp = fopen("pass", "r")) == NULL) {
		printf("Fichier \"pass\" non trouve, j'arrete.\n");
		exit(0);
	}

	while((p=fgets(s, MAXLEN, fp)) && !found ) {
		sscanf(s, "%s %s", usrhost, password);
		if(!fnmatch(usrhost, from, FNM_CASEFOLD)) 
			found = TRUE;
	}

	if(found) {
		Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, from);
		if(!Locuteur)
			Locuteur = AjouteLocuteur(currentbot->lists->ListeLocuteurs,
									  NUS);

	}

	fclose(fp);
	if(NUS)
		free(NUS);
}

void do_seen(const char *from, const char *to, const char *rest) {
	char **Phrase;
	char *nuh;
	locuteur *Locuteur;
	char chaine[MAXLEN];

	if(rest){
		if((nuh = username(rest))){
			if(ischannel(to)){
				Phrase = malloc(3 * sizeof(char *));
				sprintf(chaine, "Voyons, %s est là %%s", rest);
				Phrase[0] = strdup(chaine);
				sprintf(chaine, "Si tu regardes bien tu verras que %s est là %%s", rest);
				Phrase[1] = strdup(chaine);
				sprintf(chaine, "%%s: %s est là il me semble.", rest);
				Phrase[2] = strdup(chaine);
				Repondre(from, to, 0, 3, Phrase, 0, 0, 0);
			}
			else{
				send_to_user(from, "Voyons, %s est là!",rest);
			}
			return;
		}		
		Locuteur = LocuteurNickExiste(currentbot->lists->ListeLocuteurs, rest);
		if(Locuteur){
#ifdef DBUG
			printf("trouvé un locuteur %s, lastseen = %ld, now = %ld diff = %ld\n", rest, Locuteur->LastSeen, time(NULL), time(NULL) - Locuteur->LastSeen);
#endif			
			if(ischannel(to)){
				Phrase = malloc(1 * sizeof(char *));
				sprintf(chaine, "%%s: j'ai vu %s il y a %s", rest, idle2str(time(NULL) - Locuteur->LastSeen));
				Phrase[0] = strdup(chaine);
				Repondre(from, to, 0, 1, Phrase, 0, 0, 0);
			}
			else{
				send_to_user(from, "j'ai vu %s il y a %s", rest, idle2str(time(NULL) - Locuteur->LastSeen));
			}
		}
		else{
			if(ischannel(to)){
				Phrase = malloc(2 * sizeof(char *));
				sprintf(chaine, "Je ne connais pas de '%s', %%s", rest);
				Phrase[0] = strdup(chaine);
				sprintf(chaine, "%s ? Ça ne me dit rien %%s", rest);
				Phrase[1] = strdup(chaine);
				Repondre(from, to, 0, 2, Phrase, 0, 0, 0);
			}else
				send_to_user(from, "Je ne connais pas de '%s'", rest);
		}
	}
	else{
		if(ischannel(to)){
			//réponse publique
			Phrase = malloc(2 * sizeof(char *));
			Phrase[0] = strdup("Je n'ai pas compris de qui tu parles, %s");
			Phrase[1] = strdup("Qui ça, %s ?");
			Repondre(from, to, 0, 2, Phrase, 0, 0, 0);
		}
		else
			send_to_user(from, "Qui ça ?");
	}
}

int ChaineEstDans(const char *aFouiller, const char *aChercher) {
	char *AFouiller;
	char *AChercher;
	char *p;
	char *GuillemetsO = 0;
	char *GuillemetsF = 0;
	//char *c;
	static int DernierNumPhrase = -1;

	/* normalement c'est fait en amont
 
   char *Init = "éèêëÉÈÊËçÇàâäåÀÂùûÙÛôöÔÖîïÎÏ";    //TODO: revoir la comparaison avec iconv
   char *Remp = "eeeeEEEEcCaaaaAAuuUUooOOiiII";
	*/
	AFouiller = strdup(aFouiller);
	AChercher = strdup(aChercher);
	/*	
		for(p = AFouiller; *p; p++) {	
		c = strchr(Init, *p);	
		if(c) *p = Remp[c-Init];	
		*p = tolower(toascii(*p));	
		}	
		for(p = AChercher; *p; p++) {	
		c = strchr(Init, *p);	
		if(c) *p = Remp[c-Init];	
		*p = tolower(toascii(*p));	
		}	
	*/
	
	if(DernierNumPhrase != GNumPhrase) {
		GuillemetsO = strchr(AFouiller, '\"');
		if(GuillemetsO) {
			GuillemetsF = strchr(GuillemetsO+1, '\"');
			if(!GuillemetsF)
				GuillemetsF = strchr(GuillemetsO, '\0');
		}
		DernierNumPhrase = GNumPhrase;
	}
	
	if((p = strcasestr(AFouiller, AChercher))
	   && !(p > GuillemetsO
			&& p < GuillemetsF)) {
		if(AFouiller) free(AFouiller);
		if(AChercher) free(AChercher);
		return TRUE;
	}
	else {
		if(AFouiller) free(AFouiller);
		if(AChercher) free(AChercher);
		return FALSE;
	}
  
} /* int ChaineEstDans() */

void    ProcessSimpleStims(const char *from, const char *to, const char *msg){

	//int AncienneAutorisation = TRUE;
	char **Reponse, **Reponse2;
	locuteur *Locuteur;
	int i;
	int j;
	int NbRep = 0;
	int Num;
	int QUEL_ENDROIT = FALSE; /* ou ? where? */
	int ILYA       = FALSE; /* Il y a, est la, there, ici */
	int LISTE_STIMULI = FALSE;
	int TESTE_STIMULI = FALSE;
	int LISTE_REPONSES = FALSE;
	int MOI        = FALSE;
	int NEGATION   = FALSE;
	int ES_TU      = FALSE;
	int AS_TU      = FALSE;
	int BIENVENUE   = FALSE; /* Aussi Bon retour */
	int MOUARF      = FALSE; /* Joli chien-chien */
	int ETTOI = FALSE; /* Et toi? */
	int QUESTION = FALSE; /* es-tu as-tu est-ce-que est-il comment pourquoi suis-je ais-tu ? */
	int SOUHAIT = FALSE; /* Positif: Porte-toi bien */
	int QUI = FALSE; /* Qui es-tu? */
	int JESUIS = FALSE;
	int FONCTION = FALSE;
	int VEUX = FALSE; /* Ouin. Je VEUX que tu ... , j'ORDONNE*/
	int VIVE = FALSE; /* Viva! Rules rulezz */
	int FRONT_NATIONAL = FALSE; /* Argh! */
	int KICKER = FALSE;
	int KICKE_MOI = FALSE;
	int ACHILLE = FALSE;
	char *NUS = NickUserStr(from);
	
	/*   CHAN_list *Channel_to = (ischannel(to)?search_chan(to):currentbot->Current_chan); */
	CHAN_list *Channel_to = (ischannel(to)?search_chan(to):0);
	
	if((ischannel(to) && is_log_on(to)) || (!ischannel(to) && logging))
		botlog(LOGFILE, "<%s#%s> %s", from, to, msg);
	
	
/*	
	if(FONCTION) {
		AncienneAutorisation = (Channel_to?Channel_to->talk:TRUE);
		if(Channel_to) Channel_to->talk = FALSE;
	}
 */

	if((Channel_to?Channel_to->talk:TRUE)) {
		
		Locuteur = LocuteurExiste(currentbot->lists->ListeLocuteurs, from);
		if(!Locuteur)
			AjouteLocuteur(currentbot->lists->ListeLocuteurs,
									  NUS);
		
		//---------------------------------------------------------------- Lecture des stimulis simples (!APPRENDS)
		for(i=0; i<TailleStim; i++)
			if(ChaineEstDans(msg, TableDesStimuli[i]->Stimulus))
				TableDesStimuli[i]->Present = TRUE;
			else
				TableDesStimuli[i]->Present = FALSE;
		
		NbRep = 0;
		
		for(i=0; i<TailleRep; i++) {
			
			if(TableDesReponses[i]->Active
			   && (STRCASEEQUAL(to, TableDesReponses[i]->Canal) ||
				   !strcmp(TableDesReponses[i]->Canal, "#*")))
				for(j=0; j<TailleStim; j++)
					if(TableDesStimuli[j]->Present &&
					   TableDesStimuli[j]->Actif &&
					   strcmp(TableDesStimuli[j]->NomStimulus,
							  TableDesReponses[i]->NomStimulus)==0) {
						   NbRep++;
					   }
		}
		
		if(NbRep) {
			Reponse  = malloc(NbRep*sizeof(char *));
			Reponse2 = malloc(NbRep*sizeof(char *));
		} else {
			Reponse  = 0;
			Reponse2 = 0;
		}
		
		Num = 0;
		
		for(i=0; i<TailleRep; i++)
			for(j=0; j<TailleStim; j++)
				if(TableDesStimuli[j]->Present &&
				   TableDesStimuli[j]->Actif &&
				   TableDesReponses[i]->Active &&
				   (STRCASEEQUAL(to, TableDesReponses[i]->Canal) ||
					!strcmp(TableDesReponses[i]->Canal, "#*")) &&
				   strcmp(TableDesStimuli[j]->NomStimulus,
						  TableDesReponses[i]->NomStimulus)==0) {
					   Reponse[Num]  = strdup(TableDesReponses[i]->Reponse);
					   Reponse2[Num] = strdup(TableDesReponses[i]->Reponse);
					   Num ++;
				   }
		
		if(Num)
			Repondre(from, to, 0, Num, Reponse, 0, Num, Reponse2);
		//---------------------------------------------------------------- fin stimulis
		
		
  
    
/*     if(INJURE && !COMPLIMENT && !ETTOI && !JESUIS) { */

/*       if(ischannel(to) && NOM) { */
/* 	if(!exist_userhost(currentbot->lists->rellist, from)) */
/* 	  add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0); */

/* 	add_to_level(currentbot->lists->rellist, from, -1); */
/* 	Humeur = rellevel(from); */

/* 	if(Humeur < 0 && Humeur > -3) { */
/* 	  Numero = NumPhrase % 5; */
/* 	  switch(Numero) { */
/* 	  case 0: */
/* 	    sendprivmsg(to, "Attention, %s, je vais kicker! J'exige des excuses.", getnick(from)); */
/* 	    break; */
/* 	  case 1: */
/* 	    sendprivmsg(to, "Mortecouilles, %s, je m'en vais vous estropier! Montjoie! Saint-Denis!", getnick(from)); */
/* 	    break; */
/* 	  case 2: */
/* 	    sendprivmsg(to, "Attention, %s, je commence a etre serieusement enerve!", getnick(from)); */
/* 	    break; */
/* 	  case 3: */
/* 	    sendprivmsg(to, "Je te previens, %s, j'ai les pieds qui me demangent.", getnick(from)); */
/* 	    break; */
/* 	  case 4: */
/* 	    sendprivmsg(to, "Implore mon pardon, miserable %s!", getnick(from)); */
/* 	    break; */
/* 	  } */
/* 	} else if(Humeur >= 0) { */
/* 	  Numero = NumPhrase % 3; */
/* 	  switch(Numero) { */
/* 	  case 0: */
/* 	    sendprivmsg(to, "Toi-meme, %s!", getnick(from)); */
/* 	    break; */
/* 	  case 1: */
/* 	    sendprivmsg(to, "C'est celui qui le dit qui l'est, %s!", getnick(from)); */
/* 	    break; */
/* 	  case 2: */
/* 	    sendprivmsg(to, "Tu me provoques, %s?", getnick(from)); */
/* 	    break; */
/* 	  } */
/* 	} else { */
	/*Humeur negative <= -3  */
/* 	  sendprivmsg(to, "Rahh, %s j'ai des fourmis dans les pieds! B[", getnick(from)); */
/* 	  if(protlevel(from) < 50) { */
/* 	    Numero = NumPhrase % 7; */
/* 	    switch(Numero) { */
/* 	    case 0: */
/* 	      sendkick(to, getnick(from), "Je t'avais prevenu!"); */
/* 	      break; */
/* 	    case 1: */
/* 	      sendkick(to, getnick(from), "Je ne supporte pas qu'on me manque de respect!"); */
/* 	      break; */
/* 	    case 2: */
/* 	      sendkick(to, getnick(from), "Dehors, odieux personnage!"); */
/* 	      break; */
/* 	    case 3: */
/* 	      sendkick(to, getnick(from), "Rahh! Ne recommence plus jamais!"); */
/* 	      break; */
/* 	    case 4: */
/* 	      sendkick(to, getnick(from), "Ta mere en tongs au Prisunic. :p"); */
/* 	      break; */
/* 	    case 5: */
/* 	      sendkick(to, getnick(from), "Tiens, prends ca!"); */
/* 	      break; */
/* 	    case 6: */
/* 	      sendkick(to, getnick(from), "Moule a gaufres!"); */
/* 	      break; */
/* 	    } */
/* 	  } else { */
/* 	    Numero = NumPhrase % 5; */
/* 	    switch(Numero) { */
/* 	    case 0: */
/* 	      sendprivmsg(to, "C'est bien parce que je suis cense te proteger que je ne te kicke pas, %s!", getnick(from)); */
/* 	      break; */
/* 	    case 1: */
/* 	      sendprivmsg(to, "Tiens le toi pour dit, %s: quand tu ne seras plus protege, je ne me generai pas pour te kicker!",  getnick(from)); */
/* 	      break; */
/* 	    case 2: */
/* 	      sendprivmsg(to, "Sors d'ici %s! J'espere que je ne devrai plus te proteger.", getnick(from)); */
/* 	      break; */
/* 	    case 3: */
/* 	      sendprivmsg(to, "Je t'en prie, ne reste pas dans ce canal, %s!", getnick(from)); */
/* 	      break; */
/* 	    case 4: */
/* 	      sendprivmsg(to, "Va te faire voir chez les Grecs, %s!", getnick(from)); */
/* 	      break; */
/* 	    } */
/* 	  } */
/* 	} */
/*       } */
/*       else if(!ischannel(to)) { */
/* 	if(!exist_userhost(currentbot->lists->rellist, from)) */
/* 	  add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0); */

/* 	add_to_level(currentbot->lists->rellist, from, -2); */
/* 	Humeur = rellevel(from); */

/* 	if(Humeur < 0) */
/* 	  send_to_user(from, "Attention, je vais m'enerver! J'exige des excuses.", getnick(from)); */
/* 	else */
/* 	  send_to_user(from, "Arrete de me charrier. Je peux etre dangereux :]", getnick(from)); */
/*       } */
/*     } */



	/*
    if(ES_TU && LORIA && !NANCY && !QUEST_CE && (NOM || !ischannel(to))) {
      Reponse = malloc(4 * sizeof(char *));
      Reponse[0] = strdup("Oui %s, je suis au LORIA.");
      Reponse[1] = strdup("Évidemment, %s, je suis au LORIA.");
      Reponse[2] = strdup("Effectivement, je suis au LORIA.");
      Reponse[3] = strdup("/me est au LORIA.");

      Reponse2 = malloc(3 * sizeof(char *));
      Reponse2[0] = strdup("%s: Pourquoi je s'rais pas au LORIA?");
      Reponse2[1] = strdup("Évidemment que je suis au LORIA, %s. T'as pas vu mon adresse?");
      Reponse2[2] = strdup("Ça me semble évident, %s.");

      Repondre(from, to, +1, 4, Reponse, +1, 3, Reponse2);
    }
	*/


/*     if(INJURE && ETRANGERS) { */
/*       if(!exist_userhost(currentbot->lists->rellist, from)) */
/* 	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0); */

/*       add_to_level(currentbot->lists->rellist, from, -4); */

/*       Reponse = malloc(6*sizeof(char *)); */
/*       Reponse[0] = strdup("%s: T'es raciste, toi!"); */
/*       Reponse[1] = strdup("Calme toi, %s."); */
/*       Reponse[2] = strdup("%s: Pas de ca ici!"); */
/*       Reponse[3] = strdup("%s: Je te préviens: je ne veux pas de tels propos ici."); */
/*       Reponse[4] = strdup("%s: Va prendre une douche froide, tu me diras si ça t'a calmé."); */
/*       Reponse[5] = strdup("Eh! Oh! Va dire des horreurs pareilles ailleurs, %s."); */
      
/*       Reponse2 = malloc(6*sizeof(char *)); */
/*       Reponse2[0] = strdup("Dehors, sale raciste!"); */
/*       Reponse2[1] = strdup("Je banne rarement, mais là, fait gaffe!"); */
/*       Reponse2[2] = strdup("Suppôt de Satan!"); */
/*       Reponse2[3] = strdup("Va jouer!"); */
/*       Reponse2[4] = strdup("Pas de ça ici!"); */
/*       Reponse2[5] = strdup("Non mais!"); */

/*       KickerRepondre(from, to, -2, 6, Reponse, -6, 6, Reponse2); */
/*     } */


/*     if((VIVE || COMPLIMENT) && FRONT_NATIONAL) { */
/*       if(!exist_userhost(currentbot->lists->rellist, from)) */
/* 	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0); */

/*       add_to_level(currentbot->lists->rellist, from, -4); */

/*       Reponse = malloc(7*sizeof(char *)); */
/*       Reponse[0] = strdup("%s: Raciste, va!"); */
/*       Reponse[1] = strdup("Je te previens, %s: j'aime pas les racistes."); */
/*       Reponse[2] = strdup("%s: Tu me déçois. :("); */
/*       Reponse[3] = strdup("Abstiens-toi d'en parler ici, %s, s'il-te-plaît, ou je ne réponds plus de rien."); */
/*       Reponse[4] = strdup("Je t'aimais bien jusqu'à présent, %s. :("); */
/*       Reponse[5] = strdup("%s! Tu dérailles?"); */
/*       Reponse[6] = strdup("/me est outré!"); */
      
/*       Reponse2 = malloc(6*sizeof(char *)); */
/*       Reponse2[0] = strdup("Et moi? J'suis pas de ta race?"); */
/*       Reponse2[1] = strdup("Reste avec ceux que tu considères comme tes égaux!"); */
/*       Reponse2[2] = strdup("Ne reste pas ici, je ne te supporte plus!"); */
/*       Reponse2[3] = strdup("Je n'aime pas faire de l'élitisme, mais là, vraiment!"); */
/*       Reponse2[4] = strdup("Tu dépasses les bornes!"); */
/*       Reponse2[5] = strdup("Va voir dans #frontnat!"); */

/*       KickerRepondre(from, to, -2, 7, Reponse, -6, 6, Reponse2); */
/*     } */

/*     if(JESUIS && POUR && FRONT_NATIONAL) { */
/*       if(!exist_userhost(currentbot->lists->rellist, from)) */
/* 	add_to_levellist(currentbot->lists->rellist, NickUserStr(from), 0); */

/*       add_to_level(currentbot->lists->rellist, from, -4); */

/*       Reponse = malloc(6*sizeof(char *)); */
/*       Reponse[0] = strdup("%s: Et pourquoi?"); */
/*       Reponse[1] = strdup("Je te préviens, %s: je n'aime pas le Front National."); */
/*       Reponse[2] = strdup("%s: Tu me déçois. :("); */
/*       Reponse[3] = strdup("Abstiens-toi d'en parler ici, %s, s'il-te-plaît, ce n'est pas la peine de lui faire de la pub."); */
/*       Reponse[4] = strdup("Hé bien moi, je suis contre, %s. :("); */
/*       Reponse[5] = strdup("%s! Tu plaisantes?"); */
      
/*       Reponse2 = malloc(6*sizeof(char *)); */
/*       Reponse2[0] = strdup("Je savais que tu étais égoïste: tu n'arrives pas à te mettre à la place des autres (les étrangers, dans ce cas précis)."); */
/*       Reponse2[1] = strdup("Reste avec ceux que tu considères comme tes égaux!"); */
/*       Reponse2[2] = strdup("Pas de propagande dans ce canal!"); */
/*       Reponse2[3] = strdup("Tu m'as déçu!"); */
/*       Reponse2[4] = strdup("Révise tes idées!"); */
/*       Reponse2[5] = strdup("Va voir dans #fascisme!"); */

/*       KickerRepondre(from, to, -2, 6, Reponse, -6, 6, Reponse2); */
/*     } */

		
		
	}
	/* Si l'autorisation de parler n'est pas donnee */
	else {
		
	}
	
/*	
	if(FONCTION)
		if(Channel_to) Channel_to->talk = AncienneAutorisation;
*/	

	if(NUS)
		free(NUS);
	
}

// Local variables:
// coding: utf-8
// end:
