/*
 *  userlisttest.c
 *  patrocle
 *
 *  Created by Sébastien Kirche on 01/02/10.
 *  Copyright 2010 - All rights reserved.
 *
 */

#include <stdio.h>
#include "userlist.h"
#include "chanuser.h"

void display_list(USER_list **list);

void display_list(USER_list **list)
{
	USER_list *item;
	int i = 1;
	for (item = *list; item; item = item->next, i++) {
		printf("item %2d : %p : nick=%s / user=%s / host=%s / next=%p\n", 
			   i, item, 
			   item->nick, item->user, item->host,
			   item->next);
	}
}

int main(int argc,char** argv)
{
	USER_list *list = NULL;
	
	printf("Test lists\n");
	printf("Create 1 element\n");
	add_user(&list, "nick1", "user1", "host1");
	printf("Listing of list %p...\n", list);
	display_list(&list);
	printf("Add 2 elements\n");
	add_user(&list, "nick2", "user2", "host2");
	add_user(&list, "nick3", "user3", "host3");
	printf("Listing of list %p...\n", list);
	display_list(&list);
	printf("Remove 1 element\n");
	delete_user(&list, "nick2");
	printf("Listing of list %p...\n", list);
	display_list(&list);
	
	return 0;
}