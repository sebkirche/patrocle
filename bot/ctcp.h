/*
 ctcp.h - deals with most of the ctcp stuff (except for DCC).
 Copyright (C) 1993 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997 François Parmentier
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
#ifndef _CTCP_H_
#define _CTCP_H_

void 	on_ctcp( char *from, char *to, char *ctcp_msg );
void	ctcp_finger( char *from, char *to, char *rest );
void	ctcp_version( char *from, char *to, char *rest );
void	ctcp_clientinfo( char *from, char *to, char *rest );
void	ctcp_dcc( char *from, char *to, char *rest );
void	ctcp_ping( char *from, char *to, char *rest );
void	ctcp_source( char *from, char *to, char *rest );
void	ctcp_ignore( char *from, char *to, char *rest );
void	ctcp_time( char *from, char *to, char *rest );
void	ctcp_unknown( char *from, char *to, char *ctcp_command );

#endif

// Local variables:
// coding: utf-8
// end:
