#ifndef CFGFILE_H
#define CFGFILE_H

#include "vladbot.h"

#define	QUIET	0
#define ERROR	1
#define NOTICE	2

void	set_debug(char *s);
void	create_bot(char *s);
void	create_userlist(char *s);
void	set_default(char *s);
void	set_var(char *s);

void	set_listprot();
void	set_listshit();
void	set_listuser();
void    set_listrel ();
void	set_dcc();
void	set_nick(botinfo *bot, char *s);
void	set_login(botinfo *bot, char *s);
void	set_name(botinfo *bot, char *s);
void	set_uploaddir(botinfo *bot, char *s);
void	set_downloaddir(botinfo *bot, char *s);
void	set_indexfile(botinfo *bot, char *s);
void	set_helpfile(botinfo *bot, char *s);
void    set_stimfile (botinfo *bot, char *s);
void    set_repfile (botinfo *bot, char *s);
void    set_botfile (botinfo *bot, char *s);
void	add_server(botinfo *bot, char *s);
void	add_to_channellist(botinfo *bot);
void	set_listset(botinfo *bot, char *s);

void	get_idletimeout();
void	get_waittimeout();
void	get_maxuploadsize();
void	get_notefile();
void	get_maintainer();
void	get_globaldebug();

void	readcfg();
void	cleanupcfg();

#endif
