/*
 userlist.c - implementation of userlists
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

#ifndef USERLIST_H
#define USERLIST_H

typedef struct	USERLVL_struct
{
  char	*userhost;		/* VladDrac!irc@blah */
  int	access;			/* accesslevel */
  struct	USERLVL_struct	*next;	/* next user */
} USERLVL_list;

typedef enum {
	SORT_ASC,
	SORT_DESC
} sort_type;

USERLVL_list    *exist_userhost( USERLVL_list **l_list, const char *userhost );
USERLVL_list	*find_userhost( USERLVL_list **l_list, const char *userhost );
USERLVL_list	**init_levellist();
void			add_to_levellist( USERLVL_list **l_list, const char *userhost, int level );
int				remove_from_levellist( USERLVL_list **l_list, const char *userhost );
void			delete_levellist( USERLVL_list **l_list );
int				get_level( USERLVL_list **l_list, const char *userhost );
int				get_level_neg( USERLVL_list **l_list, const char *userhost , int *success );
void			add_to_level( USERLVL_list **l_list, const char *userhost, int humeur );
void			show_lvllist( USERLVL_list **l_list, const char *from, const char *userhost );
void			cancel_level( USERLVL_list **l_list, int level );
int				write_lvllist( USERLVL_list **l_list, const char *filename, sort_type sort);
void			sort_list( USERLVL_list **list, sort_type sort);

#endif /* USERLIST_H */

// Local variables:
// coding: utf-8
// end:
