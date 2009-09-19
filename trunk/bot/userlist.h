/*
 userlist.c - implementation of userlists
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

#ifndef USERLIST_H
#define USERLIST_H

typedef struct	USERLVL_struct
{
  char	*userhost;		/* VladDrac!irc@blah */
  int	access;			/* accesslevel */
  struct	USERLVL_struct	*next;	/* next user */
} USERLVL_list;

USERLVL_list	 *exist_userhost (USERLVL_list **, const char *);
USERLVL_list	 *find_userhost(USERLVL_list **, char *);
USERLVL_list	**init_levellist();
void			  add_to_levellist(USERLVL_list **, char *, int);
int				  remove_from_levellist(USERLVL_list **, char *);
void			  delete_levellist(USERLVL_list **);
int				  get_level(USERLVL_list **, const char *);
int				  get_level_neg(USERLVL_list **,const char *, int *);
void			  add_to_level (USERLVL_list **, const char *, int);
void			  show_lvllist(USERLVL_list **, char *, char *);
void			  cancel_level(USERLVL_list **, int);
int				  write_lvllist(USERLVL_list **, char *);

#endif /* USERLIST_H */

// Local variables:
// coding: utf-8
// end:
