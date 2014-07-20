/*
 ftext.h - formatted text for VladBot (filelist and help)
 Copyright (C) 1993 VladDrac (irvdwijk@cs.vu.nl)
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

#ifndef FTEXT_H
#define FTEXT_H

#include <stdio.h>

char	*read_word( FILE *ffile );
void	 translate_escape( char *s, char c );
void	 skipcomment( FILE *ffile );
int		 find_topic( FILE *ffile, const char *topic );
char	*get_ftext( FILE *ffile );

#endif /* FTEXT_H */

// Local variables:
// coding: utf-8
// end:
