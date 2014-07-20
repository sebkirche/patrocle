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

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "config.h"
#define _GNU_SOURCE
#include "fnmatch.h"
#include "misc.h"
#include "send.h"
#include "userlist.h"


USERLVL_list *exist_userhost(USERLVL_list **l_list, const char *userhost)
{
	USERLVL_list	* User;

	for (User = *l_list; User; User = User->next)
		if (!fnmatch(User->userhost, userhost, FNM_CASEFOLD))
			return (User);

	return (NULL);
}

USERLVL_list	*find_userhost(USERLVL_list **l_list, const char *userhost)
{
	USERLVL_list	* User;

	for (User = *l_list; User; User = User->next)
		if (STRCASEEQUAL(userhost, User->userhost))
			return (User);

	return (NULL);
}

USERLVL_list	**init_levellist()
{
	USERLVL_list	**l_list;

	l_list = (USERLVL_list **) malloc(sizeof(*l_list));
	*l_list = NULL;

	return (l_list);
}

void	add_to_levellist(USERLVL_list **l_list, const char *userhost, int level)
{
	/*
	 * adds a user to the list... 
	 * should we check here for double entries?
	 * Check for level (adding higher level etc)
	 */
	USERLVL_list	* New_user;

	if ((New_user = find_userhost(l_list, userhost)) != NULL) {
		New_user->access = level;
		return ;
	}

	if ((New_user = (USERLVL_list*) malloc(sizeof(*New_user))) == NULL)
		return ;

	mstrcpy(&New_user->userhost, userhost);
	New_user->access = level;
	New_user->next = *l_list;
	*l_list = New_user;
}

int	remove_from_levellist(USERLVL_list **l_list, const char *userhost)
{
	/*
	 * removes the first occurrence of userhost from l_list 
	 */

	USERLVL_list	**old;
	USERLVL_list	*dummy;

	if ((dummy = find_userhost(l_list, userhost)) == NULL)
		return (FALSE);

	for (old = l_list; *old; old = &(*old) ->next)
		if (*old == dummy) {
			*old = dummy->next;
			free(dummy->userhost);
			free(dummy);
			return (TRUE);
		}

	return (FALSE);
}

void	delete_levellist(USERLVL_list **l_list)
{
	USERLVL_list	* dummy;
	USERLVL_list	*next;

	dummy = *l_list;

	while (dummy) {
		next = dummy->next;
		remove_from_levellist(l_list, dummy->userhost);
		dummy = next;
	}
	free(l_list);
}

int	get_level(USERLVL_list **l_list, const char *userhost)
{
	USERLVL_list	* dummy;
	int	access = 0;

	for (dummy = *l_list; dummy; dummy = dummy->next)
		if (!fnmatch(dummy->userhost, userhost, FNM_CASEFOLD))
			access = (dummy->access > access) ?
			         dummy->access : access;

	return (access);
}

int	get_level_neg(USERLVL_list **l_list, const char *userhost , int *success)
{
	USERLVL_list	* dummy;
	int	access = -500;

	*success = FALSE;

	for (dummy = *l_list; dummy; dummy = dummy->next)
		if (!fnmatch(dummy->userhost, userhost, FNM_CASEFOLD)) {
			access = (dummy->access > access) ?
			         dummy->access : access;
			*success = TRUE;
		}

	return (access);
}

void add_to_level(USERLVL_list **l_list, const char *userhost, int humeur)
{
	USERLVL_list	* dummy;

	for (dummy = *l_list; dummy; dummy = dummy->next)
		if (!fnmatch(dummy->userhost, userhost, FNM_CASEFOLD))
			dummy->access += humeur;
}

void	show_lvllist(USERLVL_list **l_list, const char *from, const char *userhost)
{
	USERLVL_list	* dummy;

	for (dummy = *l_list; dummy; dummy = dummy->next)
		if (!fnmatch(dummy->userhost, userhost, FNM_CASEFOLD) || !*userhost)
			send_to_user(from, " %40s | %-4d",
			              dummy->userhost, dummy->access);
}

void cancel_level(USERLVL_list **l_list, int level)
{
	USERLVL_list * dummy;
	USERLVL_list *next;

	dummy = *l_list;

	while (dummy) {
		next = dummy->next;

		if (dummy->access == level)
			remove_from_levellist(l_list, dummy->userhost);

		dummy = next;
	}
}

int	write_lvllist(USERLVL_list **l_list, const char *filename, sort_type sort)
{
	USERLVL_list	* dummy;
	time_t	T;
	FILE	*list_file;

	if ((list_file = fopen(filename, "w")) == NULL)
		return (FALSE);

	sort_list(l_list, sort);
	
	T = time((time_t *) NULL);

	fprintf(list_file, "#############################################\n");
	fprintf(list_file, "## %s\n", filename);
	fprintf(list_file, "## Created: %s", ctime(&T));
	fprintf(list_file, "## (c) 2010  seki (sebastien.kirche@free.fr)\n");
	fprintf(list_file, "#############################################\n");

	for (dummy = *l_list; dummy; dummy = dummy->next)
		fprintf(list_file,
		         " %45s %d\n", dummy->userhost, dummy->access);

	fprintf(list_file, "# End of %s\n", filename);
	fclose(list_file);

	return (TRUE);
}

/* implementation of a linked list sort, 
   from "Mergesort For Linked Lists" by Simon Tatham
   http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 
   the sort consist of a elt_cmp() to compare 2 elements
   and sort_list() that sort the list
*/
int elt_cmp(USERLVL_list *a, USERLVL_list *b, sort_type sort)
{
	/*
	 * Compare two list elements
	 *
	 * This is a 2-stages comparison :
	 * 1) compare the user level
	 * 2) if the levels are equal, compare the user ident
	 *    but keep the alphabetical order 
	 */

	int ret;
	
	ret = a->access - b->access;
	
	if (!ret){
		if (sort == SORT_ASC)
			//TODO: compare the nicknames ?
			ret = strcasecmp(a->userhost, b->userhost);
		else
			ret = strcasecmp(b->userhost, a->userhost);
	}
	
	return ret;
}

void sort_list(USERLVL_list **list, sort_type sort)
{
	/*
	 * Sort of a user list
	 */
	
	USERLVL_list *p, *q, *e, *tail;
	int insize, nmerges, psize, qsize, i, cmp;
	
	if (!list || !*list)
		return;
	
	insize = 1;
	
	while (1) {
		p = *list;
		*list = NULL;
		tail = NULL;
		
		nmerges = 0;  /* count number of merges we do in this pass */
		
		while (p) {
			nmerges++;  /* there exists a merge to be done */
			/* step `insize' places along from p */
			q = p;
			psize = 0;
			for (i = 0; i < insize; i++) {
				psize++;
				q = q->next;
				if (!q)
					break;
			}
			
			/* if q hasn't fallen off end, we have two lists to merge */
			qsize = insize;
			
			/* now we have two lists; merge them */
			while (psize > 0 || (qsize > 0 && q)) {
				
				/* decide whether next element of merge comes from p or q */
				if (psize == 0) {
					/* p is empty; e must come from q. */
					e = q;
					q = q->next;
					qsize--;
				} else if (qsize == 0 || !q) {
					/* q is empty; e must come from p. */
					e = p;
					p = p->next;
					psize--;
				} else {
					cmp = elt_cmp(p, q, sort);
					if ((cmp <= 0 && sort == SORT_ASC) || (cmp >= 0 && sort == SORT_DESC)) {
						/* First element of p is lower (or same);
						 * e must come from p. */
						e = p;
						p = p->next;
						psize--;
					} else {
						/* First element of q is lower; e must come from q. */
						e = q;
						q = q->next;
						qsize--;
					}
				}
				
				/* add the next element to the merged list */
				if (tail) {
					tail->next = e;
				} else {
					*list = e;
				}
				tail = e;
			}
			
			/* now p has stepped `insize' places along, and q has too */
			p = q;
		}
		tail->next = NULL;
		
		/* If we have done only one merge, we're finished. */
		if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
			return;
		
		/* Otherwise repeat, merging lists twice the size */
		insize *= 2;
	}
}


// Local variables:
// coding: utf-8
// end: