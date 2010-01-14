/*
 note.h - implementation of a simple messaging system for vladbot
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

#ifndef NOTE_H
#define NOTE_H

#include "config.h"

#define MAXNOTELEN	20
#define PUBLICADDR	"PUBLIC"
#define	OPERADDR	"OPERATOR"

typedef	char	*message[MAXNOTELEN];	/* 20 should be plenty for a message */

typedef	struct	note_struct
{
	char	*received_by;
	char	*from;		/* sender of note */
	char	*to;		/* pattern to whom note is */
	char	*subject;
	int	note_id;	/* id to identify note */
	int	finished;	/* true is msg finished */
	long	created;
	message	msg;
	struct	note_struct	*prev;
	struct	note_struct	*next;
} notelist;

notelist	*find_unfinished(char *from);
void		 note_create(char *from, char *s);
void		 note_delete(char *from, char *s);
void		 note_show(char *from, char *s);
void		 note_view(char *from, char *s);
void		 note_addline(char *from, char *s);
void		 note_delline(char *from, char *s);
void		 note_replaceline(char *from, char *s);
void		 note_list(char *from, char *s);
void		 note_send(char *from, char *s);
void		 note_dump(char *from, char *s);

void	parse_note(char *from, char *to, char *s);
int		dump_notelist();
int		read_notelist();

void	create_note(char *from, char *to, char *subject);
void	freenote(notelist *note);
void	del_note(char *from, int note_id);
void	show_note(char *from,	int note_id);
void	view_note(char *from);
int		note_exist(char *from);

#endif /* NOTE_H */

// Local variables:
// coding: utf-8
// end:
