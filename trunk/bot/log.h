/*
 log.h - implements logging to file
 Copyright (C) 1993 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996,1997 François Parmentier (H_I)
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

#ifndef _LOG_H_
#define _LOG_H_

#define	LVL_QUIET	0
#define LVL_ERROR	1
#define LVL_NOTICE	2
#define LVL_DEBUG	3
#define MAX_LVL LVL_DEBUG

extern int loglevel;
extern	void 	botlog(const char *logfile,const char *logfmt, ...);
extern	void 	globallog( char *logfile, char *logfmt, ...);

#endif

// Local variables:
// coding: utf-8
// end:
