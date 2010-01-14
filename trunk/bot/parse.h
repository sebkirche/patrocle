/*
 parse.h - server input parsing
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
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
#ifndef PARSE_H
#define PARSE_H

void 	parse_privmsg(char *from, char *rest);
void	parse_notice(char *from, char *rest);
void	parse_join(char *from, char *rest);
void	parse_part(char *from, char *rest);
void	parse_mode(char *from, char *rest);
void	parse_nick(char *from, char *rest);
void	parse_kick(char *from, char *rest);
void	parse_ping(char *from, char *rest);
void	parse_pong(char *from, char *rest);
void	parse_quit(char *from, char *rest);
void	parse_error(char *from, char *rest);
void	parse_001(char *from, char *rest);
void	parse_324(char *from, char *rest);
void	parse_352( char *from, char *rest);
void	parse_367(char *from, char *rest);
void	parse_433(char *from, char *rest);
void	parse_451(char *from, char *rest);
void	parse_471(char *from, char *rest);
void 	parseline(char *line);

#endif /* PARSE_H */

// Local variables:
// coding: utf-8
// end:
