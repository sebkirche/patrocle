/*
 session.c - manages sessions between IRC users and the bot
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
 
 A session is created:
 - When a user starts a DCC chat connection
 - When a user sends a legal command (public or private)
 - When a user sends an illegal private message (for floodcontrol)
 
 A session is deleted when the user has been idle for a defined
 amount of time.
 
 Sessions can be used for "remembering" the cwd (with filetransfer),
 last used command, floodingcontrol etc.
 
 */
#ifndef SESSION_H
#define SESSION_H

#include "config.h"

#define IS_FLOODING	1
#define IS_OK		2

typedef struct	session_struct
{
	char	*user;			/* nick!user@host.domain 		*/
	long	last_received;		/* (info)  when did we receive last cmd */
	char	*nodcc_to;		/* "to" when there was no dcc		*/
	char	*nodcc_rest;		/* "rest" when there was no dcc		*/
	char	*cwd;			/* (dcc)   current working directory 	*/
	int	currentfile;		/* (dcc)   current file to send		*/
	int	maxfile;		/* (dcc)   total number of files	*/
	struct
	{
		char	*path;
		char	*name;
	} file_list[LISTSIZE];		/* (dcc)   list of files to send 	*/
	long	flood_start;		/* (flood) used for timing-stuff 	*/
	int	flood_cnt;		/* (flood) Number of lines received	*/
	int	flooding;		/* (flood) Marked true if flooding	*/
	struct	session_struct	*next;	/* next session in list 		*/
} SESSION_list;	
	
SESSION_list	*find_session(char *user);
SESSION_list	*create_session(char *user);
int	delete_session(char *user);
void	cleanup_sessions();
int	check_session(char *user);
int	check_flood(char *user);
void	nodcc_session(char *from, char *to, char *rest);
void	hasdcc_session(char *from);
char	*get_cwd(char *user);
void	change_dir(char *user, char *new_dir);
void    dcc_sendnext(char *user);
int     dcc_sendlist(char *user, char *path, char *file_list[], int n);
void	do_showqueue();
void	sendreply(char *s, ...);

#endif
// Local variables:
// coding: utf-8
// end:
