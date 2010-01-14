/*
 misc.c - misc functions for VladBot
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997 François Parmentier (H_I)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
//#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#else
#include <pwd.h>
#endif

#include "config.h"
#include "misc.h"

static	char	usernick[MAXLEN];
static	char	timebuf[MAXLEN];
static 	char	idlestr[MAXLEN];
static	char	path_buf[MAXLEN];

char	*months[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

int 	ischannel(const char *channel)
{
	return(*channel == '#' || *channel == '&');
}

int	isnick(const char *nick)
{
	int	i;

	for(i=0; nick[i]; i++)
		if((i==0 && !isalpha(nick[i])) || !isalnum(nick[i]))
			if(!strrchr("|_-[]\\{}^", nick[i]))
				return FALSE;
	return TRUE;
}

char *getnick(const char *nick_userhost)
{
	strcpy(usernick, nick_userhost);
	return(strtok( usernick, "!" ));
}

char    *GetNick(const char *nick_userhost) 
{
	char *pointeur;
	char *fin;
  
	strcpy(usernick, nick_userhost);
	pointeur = strtok(usernick, "!");
	while(*pointeur=='_') pointeur++;

	fin = &pointeur[strlen(pointeur)-1];
	while(*fin =='_' && fin > pointeur) {
		*fin = '\0';
		fin--;
	}
  
	return(pointeur);
}


char	*time2str(long time)
{
	struct	tm	*btime;

	btime = localtime(&time);
	if(time && (sprintf(timebuf, "%-2.2d:%-2.2d:%-2.2d %s %-2.2d %d", 
						btime->tm_hour, btime->tm_min, btime->tm_sec, 
						months[btime->tm_mon], btime->tm_mday, 
						btime->tm_year + 1900)))
		return timebuf;
	return(NULL);
} 

int     gettimeday(long time)
{
	struct	tm	*btime;

	btime = localtime(&time);

	return(btime->tm_mday);
}

int     gettimehour(long time)
{
	struct tm *btime;

	btime = localtime(&time);

	return(btime->tm_hour);
}

int     time2hours(long time)
{
	struct tm *btime;

	btime = localtime(&time);

	return(  btime->tm_year * 12 * 31 * 24
			  + btime->tm_mon * 31 * 24
			  + btime->tm_mday*24
			+ btime->tm_hour);	//FIXME: this does not take seconds into account; so between 2 contacts at 17:58 and 18:02 we get 1 hour of delay...
								//maybe that we should use the seconds too, to compute more accurately the contacts ?
}

char    *time2heure(long time)
{
	struct tm  *btime;

	btime = localtime(&time);
	/*   if(time && (sprintf(timebuf, "%d heure%s %d minute%s et %d seconde%s", */
	/* 			btime->tm_hour, (btime->tm_hour>1?"s":""), */
	/* 			btime->tm_min, (btime->tm_min>1?"s":""), */
	/* 			btime->tm_sec, (btime->tm_sec>1?"s":"")))) */
	if(time && btime->tm_min && sprintf(timebuf, "%d heure%s %d minute%s",
										  btime->tm_hour, (btime->tm_hour>1?"s":""),
										  btime->tm_min, (btime->tm_min>1?"s":"")))
		return timebuf;

	if(time && (sprintf(timebuf, "%d heure%s %d minute%s",
						  btime->tm_hour, (btime->tm_hour>1?"s":""),
						  btime->tm_min, (btime->tm_min>1?"s":""))))
		return timebuf;

	return(NULL);
}

char	*time2small(long time)
{
	struct tm *btime;

	btime = localtime( &time );
	if(time && (sprintf(timebuf, "%s %-2.2d", 
						months[btime->tm_mon], btime->tm_mday)))
		return timebuf;
	return(NULL);
}

char	*idle2str(long time)
{
	int	days,
		hours,
		mins,
		secs;
	
	days  = (time/86400);
	hours = ((time - (days*86400))/3600);
	mins  = ((time - (days*86400) - (hours*3600))/60);
	secs  = (time - (days*86400) - (hours*3600) - (mins*60));

	sprintf(idlestr, "%d day%s, %d hour%s, %d minute%s and %d second%s",
			days, EXTRA_CHAR(days), hours, EXTRA_CHAR(hours),
			mins, EXTRA_CHAR(mins), secs, EXTRA_CHAR(secs)); 

	return idlestr;
}

char	*mstrcpy(char **dest, const char *src)
{
	if(src){
		*dest = (char *) malloc(strlen(src)+1);
		strcpy(*dest, src);
	}
	else
		*dest = NULL;

	return *dest;
}

char	*mstrncpy(char **dest, const char *src, int len)
{
	if(src){
		*dest = (char *) malloc(len+1);
		strncpy(*dest, src, len);
	}
	else
		*dest = NULL;

	return *dest;
}

char	*get_token(char **src, const char *token_sep)
/*
 * Just a little more convenient than strtok()
 * This function returns a pointer to the first token
 * in src, and makes src point to the "new string".
 *
 */
{
	char	*tok;

	if(!(src && *src && **src))
		return NULL;

	/* first, removes leading token_sep's */
	while(**src && strchr(token_sep, **src))
		(*src)++;
	
	/* first non token_sep */
	if(**src)
		tok = *src;
	else
		return NULL;

	/* Make *src point after token */
	*src = strpbrk(*src, token_sep);
	if(*src){
		**src = '\0';
		(*src)++;
		while(**src && strchr(token_sep, **src))
			(*src)++;
	}
	else
		*src = "";
	return tok;
}
	
char	*get_string(char **src)
/* 
 * Put's a \0 after the first non-escaped " in the string,
 * returns NULL on error (no closing ") and translates escapes.
 */
{
	char	*s;
	int	p=0;
	
	if(!(src && *src && **src && **src == '"'))
		return NULL;

	s = ++(*src);
	while(**src){
		if(**src == '"'){
			s[p] = '\0';
			(*src)++;
			return s;
		}
		if(**src == '\\'){
			(*src)++;
			if(!**src)
				return NULL;
			switch(**src){
				case 'b': 	
					s[p++] = '\002';
					break;
				case 'u':	
					s[p++] = ''; /* '\031'; */
					break;
				case 'i':	
					s[p++] = ''; /* '\022'; */
					break;
				case '"':	
					s[p++] = '"';
					break;
				case '\\':
					s[p++] = '\\';
					break;
				default:
					s[p++] = **src;
					break;
			}
		}
		else
			s[p++] = **src;
		(*src)++;
	}
	return NULL;				
}


char	*expand_twiddle(char *s)
/*
 * Expands:
 * ~username	into username's homedir
 * ~/		into the owners homedir.
 * Trailing path will be added
 */
{
#ifdef WIN32
    char entry[MAXLEN];
#else
    struct passwd *entry;
#endif

    if(!s)
	return NULL;
    
    if(*s == '~'){
	s++;
	if(*s == '/'){
	    /* my homedir */
	    s++;
#ifdef WIN32
	    GetEnvironmentVariable("USERPROFILE", entry, sizeof(entry));
#else
	    entry = getpwuid(getuid());
#endif
	}
#ifndef WIN32 //FIXME : find another solution in WIN32
	else{
	    char	*user;
	    
	    if((user = get_token(&s, "/")))
		entry = getpwnam(user);
	    else
		entry = NULL;
	}
#endif
	if(!entry)
	    return NULL;
#ifdef WIN32
	sprintf(path_buf, "%s/%s", entry, s);
#else
	sprintf(path_buf, "%s/%s", entry->pw_dir, s);		
	
#endif
	return path_buf;
    }
    else
	return s;
}

#ifndef __APPLE__
char 	*strcasestr(char *s1, char *s2)
{
    char n1[256], n2[256];
    int i;

    for( i = 0; s1[i] != '\0'; i++ )
        n1[i] = toupper( s1[i] );
    n1[i] = '\0';

    for( i = 0; s2[i] != '\0'; i++ )
        n2[i] = toupper( s2[i] );
    n2[i] = '\0';
    return( strstr( n1,n2 ) );
}
#endif

// Local variables:
// coding: utf-8
// end:
