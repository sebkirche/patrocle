/*
 log.c - implements logging to file
 Copyright (C) 1993 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996,1997 François Parmentier (H_I)
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

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>//seki
#include "dcc.h"
#include "vladbot.h"
#include "config.h"
#include "log.h"
#include "debug.h"

extern	botinfo	*currentbot;

int	loglevel = LVL_QUIET;

void 	botlog(const char *logfile,const char *logfmt, ...)
{
	char 	buf[WAYTOBIG];
	va_list	msg;
	FILE	*flog;
	time_t	T;
	char	*time_string = NULL;


	if((T = time((time_t *)NULL)) != -1){
		time_string = ctime(&T);	
		*(time_string + strlen(time_string) - 1) = '\0';
	}	
	if((flog = fopen( logfile, "a")) == NULL)
		return;
	va_start(msg, logfmt);
	vsprintf(buf, logfmt, msg);
#ifdef DBUG
	debug(LVL_DEBUG, "%s", buf);
#endif
	fprintf(flog, "[%s] %s: %s\n", currentbot->botname, time_string, buf);
	va_end(msg);
	fclose(flog);
}

void 	globallog( char *logfile, char *logfmt, ...)
{
	char 	buf[WAYTOBIG];
	va_list	msg;
	FILE	*flog;
	time_t	T;
	char	*time_string = NULL;


	if((T = time((time_t *)NULL)) != -1){
		time_string = ctime(&T);	
		*(time_string + strlen(time_string) - 1) = '\0';
	}	
	if((flog = fopen( logfile, "a")) == NULL)
		return;
	va_start(msg, logfmt);
	vsprintf(buf, logfmt, msg);
#ifdef DBUG
	global_dbg(LVL_DEBUG, "%s", buf);
#endif
	fprintf(flog, "[-GLOBAL-] %s: %s\n", time_string, buf);
	va_end(msg);
	fclose(flog);
}

// Local variables:
// coding: utf-8
// end:
