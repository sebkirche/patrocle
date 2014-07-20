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

#ifndef VLAD_ONS_H
#define VLAD_ONS_H
/*
 * vlad-ons.h
 * Contains: on msg, on join
 *
 */

typedef	struct
{
	char		*name;			/* name of command 	*/
	void	(*function)(const char *, const char*, const char*);	/* pointer to function 	*/
	int			 userlevel;		/* required userlvl 	*/
	int			 shitlevel;		/* max. allowed shitlvl */
	int			 rellevel;      /* required rellvl      */
	int			 forcedcc;		/* If true,		command requires DCC */
} command_tbl;

extern command_tbl on_msg_commands[];

void	on_msg( const char *from, const char *to, const char *next );
void 	on_mode( const char *from, const char *rest );
void	on_kick(const char *from, const char *channel, const char *nick, const char *reason);
void 	on_join( const char *who, const char *channel );
/* -------- */
void	show_help( const char *from, const char *to, const char *rest );
void	show_whoami( const char *from, const char *to, const char *rest ); 
void	show_info( const char *from, const char *to, const char *rest ); 
void	show_time( const char *from, const char *to, const char *rest); 
void	show_userlist( const char *from, const char *to, const char *rest ); 
void	show_shitlist( const char *from, const char *to, const char *rest ); 
void    show_protlist( const char *from, const char *to, const char *rest );
void    show_rellist( const char *from, const char *to, const char *rest );
void    do_nreladd( const char *from, const char *to, const char *rest );
void    show_mycmds( const char *from, const char *to, const char *rest );
void    show_cmdlvls(const char *from, const char *to, const char *rest);
void	do_op( const char *from, const char *to, const char *rest ); 
void	do_giveop( const char *from, const char *to, const char *rest ); 
void	do_deop( const char *from, const char *to, const char *rest ); 
void	do_invite( const char *from, const char *to, const char *rest );
void	do_open( const char *from, const char *to, const char *rest );
void	do_chat( const char *from, const char *to, const char *rest );
void	do_send( const char *from, const char *to, const char *rest );
void	do_flist( const char *from, const char *to, const char *rest );
void	do_say( const char *from, const char *to, const char *rest ); 
void	do_me( const char *from, const char *to, const char *rest ); 
void	do_do( const char *from, const char *to, const char *rest ); 
void	show_channels( const char *from, const char *to, const char *rest ); 
void	do_join( const char *from, const char *to, const char *rest ); 
void	do_leave( const char *from, const char *to, const char *rest ); 
void	do_nick( const char *from, const char *to, const char *rest ); 
void	do_die( const char *from, const char *to, const char *rest ); 
void	do_quit( const char *from, const char *to, const char *rest );
void    do_logon(const char *from, const char *to, const char *rest);
void    do_logoff(const char *from, const char *to, const char *rest);
void    do_msglogon(const char *from, const char *to, const char *rest);
void    do_msglogoff(const char *from, const char *to, const char *rest);
void    do_comchar(const char *from, const char *to, const char *rest);
void	show_whois( const char *from, const char *to, const char *rest );
void	show_nwhois( const char *from, const char *to, const char *rest );
void	do_nuseradd( const char *from, const char *to, const char *rest );
void	do_useradd( const char *from, const char *to, const char *rest );
void	do_userwrite( const char *from, const char *to, const char *rest );
void    do_locwrite( const char *from, const char *to, const char *rest );
void	do_userdel( const char *from, const char *to, const char *rest );
void	do_nshitadd( const char *from, const char *to, const char *rest );
void	do_shitadd( const char *from, const char *to, const char *rest );
void	do_shitwrite( const char *from, const char *to, const char *rest );
void	do_shitdel( const char *from, const char *to, const char *rest ); 
void    do_nprotadd( const char *from, const char *to, const char *rest );
void    do_protadd( const char *from, const char *to, const char *rest );
void    do_protwrite( const char *from, const char *to, const char *rest );
void    do_protdel( const char *from, const char *to, const char *rest );
void    do_relwrite( const char *from, const char *to, const char *rest );
void    do_reladd(const char *from, const char *to, const char *rest);
void    do_reldel(const char *from, const char *to, const char *rest);
void	do_banuser( const char *from, const char *rest, const char *to );
void	do_showusers( const char *from, const char *rest, const char *to );
void    do_showlocs( const char *from, const char *rest, const char *to );
void	do_massop( const char *from, const char *to, const char *rest ); 
void    do_massdeop( const char *from, const char *to, const char *rest );
void	do_masskick( const char *from, const char *rest, const char *to );
void	do_massunban( const char *from, const char *rest, const char *to );
void	do_server(const char *, const char *, const char *);
	
void	show_dir( const char *from, const char *rest, const char *to );
void	show_cwd( const char *from, const char *rest, const char *to );
void	do_cd( const char *from, const char *rest, const char *to );
void	show_queue( const char *from, const char *rest, const char *to );
void	do_fork( const char *from, const char *rest, const char *to );
void	do_unban( const char *from, const char *rest, const char *to );
void	do_kick(const char *from, const char *to, const char *rest);
void	do_listdcc( const char *from, const char *rest, const char *to );
void	do_rehash( const char *from, const char *rest, const char *to );
void	do_reloadlogic(const char *from, const char *to, const char *rest);
#ifndef WIN32
void	do_alarm( const char *from, const char *rest, const char *to );
#endif
void	giveop( const char *channel, const char *nicks );
int 	userlevel(const char *from );
int 	shitlevel(const char *from );
int 	protlevel(const char *from );
int     rellevel(const char *from );
void	ban_user( const char *who, const char *channel );
void	signoff( const char *from, const char *reason );
void    do_fuck(const char *from, const char *to, const char *who);
void    do_apprends( const char *from, const char *to, const char *rest );
void    do_oublie(const char *from, const char *to, const char *rest );
void    do_desactive(const char *from, const char *to, const char *rest);
void    do_active(const char *from, const char *to, const char *rest);
void    do_stimwrite(const char *from, const char *to, const char *rest);
void    do_stimload(const char *from, const char *to, const char *rest);
void    do_repwrite(const char *from, const char *to, const char *rest);
void    do_repload(const char *from, const char *to, const char *rest);
void    ProcessSimpleStims( const char *from, const char *to, const char *msg);
void    do_showbots(const char *from, const char *to, const char *rest);
void    do_botadd(const char *from, const char *to, const char *rest);
void    do_topic(const char *from, const char *to, const char *rest);
void    do_botload(const char *from, const char *to, const char *rest);
void    do_botwrite(const char *from, const char *to, const char *rest);
void    do_botdel(const char *from, const char *to, const char *rest);
void    do_stimlist(const char *from, const char *to, const char *rest);
void    do_replist(const char *from, const char *to, const char *rest);
void    do_stimdel(const char *from, const char *to, const char *rest);
void    do_repdel(const char *from, const char *to, const char *rest);
void    do_seen(const char *from, const char *to, const char *rest);
char    *UserStr(const char *nuh);
char    *NickUserStr(const char *nuh);
int		ChaineEstDans(const char *aFouiller, const char *aChercher);
void    Repondre(const char *from, const char *to, int AjoutHumeurPos, int NbRepPos, char **RepPos, int AjoutHumeurNeg, int NbRepNeg, char **RepNeg);
void    KickerRepondre(const char *from, const char *to, int AjoutHumeurPos, int NbRepPos, char **RepPos, int AjoutHumeurNeg, int NbRepNeg, char **RepNeg);
void	Ecrire(const char *from, const char *to, const char *phrase);
#endif /* VLAD_ONS_H */

// Local variables:
// coding: utf-8
// end:
