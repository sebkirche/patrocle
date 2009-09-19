/*
 parsing.c - some general string parsing routines
 Copyright (C) 1993, 1994 VladDrac (irvdwijk@cs.vu.nl)
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

#ifndef PARSING_H
#define PARSING_H

#define	skipspc(str)	readspaces(str)

int		readint(char **src, int *dest);
int		readstring(char **src, char *dest);
int		readident(char **src, char *dest);
int		readboolean(char **src, int *dest);
void	readspaces(char **src);
int		readnick(char **src, char *dest);
int		readchannel(char **src, char *dest);

#endif /* PARSING_H */

// Local variables:
// coding: utf-8
// end:
