/*
 chanuser.c - register users on a channel
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

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "chanuser.h"
#include "misc.h"
#include "config.h"

static	char	buffer[MAXLEN];

USER_list	*search_user( USER_list **u_list, const char *nick )
{
	USER_list	*User;

	for( User = *u_list; User; User = User->next )
		if( STRCASEEQUAL( nick, User->nick ) )
			return(User);		
	return(NULL);
}

void	add_user( USER_list **u_list, const char *nick, 
                  const char *user, const char *host )
/*
 * adds a user to the list... 
 */
{
	USER_list	*New_user;

	if( (New_user = (USER_list*)malloc(sizeof(*New_user))) == NULL)
		return;
	if( search_user(u_list, nick) != NULL ) {
	  free(New_user);
	  return;
	}
	strncpy(New_user->nick, nick, sizeof(New_user->nick));
	mstrcpy(&New_user->user, user );	// do not forget to free
	mstrcpy(&New_user->host, host );	// these 2 strings !!!
	New_user->mode = 0;
	New_user->next = *u_list;
	*u_list = New_user;
}

int	delete_user( USER_list **u_list, const char *nick )
/*
 * removes a user from u_list 
 */
{
	USER_list	**old;
	USER_list	*Dummy;

	if( (Dummy = search_user( u_list, nick )) == NULL )
		return(FALSE);

	for( old = u_list; *old; old = &(*old)->next )
		if( *old == Dummy ){
			*old = Dummy->next;
			free_user(Dummy);
			return(TRUE);
		}
	return(FALSE);
}

int	change_user_nick( USER_list **u_list, 
                          const char *oldnick, const char *newnick )
{
	USER_list	*Dummy;

	if( (Dummy = search_user( u_list, oldnick )) != NULL ){
		strncpy( Dummy->nick, newnick, sizeof(Dummy->nick));
		return(TRUE);
	}
	return(FALSE);
}

void free_user(USER_list *item)
/*
 * Frees the memory of a USER_list item
 */
{
	if(item){
		free(item->user);
		free(item->host);
		free(item);
	}

}
void	clear_all_users( USER_list **u_list )
/*
 * Removes all entries in the list
 */
{
	USER_list	**old;
	USER_list       *next;

	for( old = u_list; *old; *old = next){
		next = (*old)->next;
		free_user(*old);
	}
}

/*
 * There need to be some functions which return 
 * elements (next elements). This way the information
 * in the userlists can be used to mass-do things,
 * show a list etc.
 *
 * Actually, to make it a little "opaque", these functions
 * should only be used by channel.c, i.e. everything goes
 * through channel.c...
 */

/* This one is needed to get the next user. 
 */

USER_list	*get_nextuser( USER_list *Old )
{
	return( Old->next );
}

unsigned	int	get_usermode( USER_list *User )
{
	return(User->mode);
}

char	*get_username( USER_list *User )
{
	sprintf(buffer, "%s!%s@%s", User->nick, 
			User->user, User->host );
	return(buffer);
}

char	*get_usernick( USER_list *User )
{
	return(User->nick);
}

/*
 * Some stuff to keep track of usermodes (not swi)
 */
void	add_mode( USER_list **u_list, 
                  unsigned int mode, const char *param )
{
	USER_list	*Dummy;

	if( (Dummy = search_user( u_list, param )) == NULL )	
		return;
	Dummy->mode |= mode;
}

void	del_mode( USER_list **u_list, 
                  unsigned int mode, const char *param )
{
	USER_list	*Dummy;

	if( (Dummy = search_user( u_list, param )) == NULL )	
		return;
	Dummy->mode &= ~mode;
}

// Local variables:
// coding: utf-8
// end:
