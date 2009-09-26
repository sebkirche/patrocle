/*
 misc.h - misc functions for VladBot
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997 François Parmentier (H_I)
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
 */

#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <stdlib.h>

#define EXTRA_CHAR(q)	q<=1?"":"s"
#define SKIPSPC(s)	for(;s&&(*s==' '||*s=='\t'); s++)
#define not(expr)	(!(expr))

/* The else is needed or things might get wrong in if-constructions */
#define KILLNEWLINE(q)	if(*(q+strlen(q)-1)=='\n') *(q+strlen(q)-1)='\0'; else {};
#define KILLRETURN(q)	if(*(q+strlen(q)-1)=='\r') *(q+strlen(q)-1)='\0'; else {};

/* str(case)cmp's returnvalues are kinda misleading */
#define STRCASEEQUAL(s1, s2)	(strcasecmp((s1), (s2)) == 0)
#define STREQUAL(s1, s2)	(strcmp((s1), (s2)) == 0)

int 	ischannel(const char *channel );
int		isnick(const char *nick);
char	*getnick(const char *nick_userhost);
char    *GetNick (const char *nick_userhost);
char	*time2str(long time);
int      gettimeday(long time);
int      gettimehour(long time);
int      time2hours(long time);
char    *time2heure(long time);
char	*time2small(long time);
char	*idle2str(long time);
char	*mstrcpy(char **dest, const char *src);
char	*mstrncpy(char **dest, const char *src, int len);
char	*get_token(char **src, const char *token_sep);
char	*get_string(char **src);
char	*expand_twiddle(char *s);
#ifndef __APPLE__
char	*strcasestr(char *s1, char *s2);
#endif

#endif /* MISC_H */

// Local variables:
// coding: utf-8
// end:
