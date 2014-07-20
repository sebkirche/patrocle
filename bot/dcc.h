/*
 dcc.h - an effort to implement dcc (at least chat) in VladBot
 Copyright (C) 1993 VladDrac (irvdwijk@cs.vu.nl)
 Copyright (C) 1996, 1997 François Parmentier
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

 Thanx to Richie_B for patching this for Ultrix.	
 */

#ifndef _DCC_H_
#define _DCC_H_

#include <sys/types.h>
#ifdef AIX
/* AIX needs <sys/types.h> included before <sys/select.h> */
#	include <sys/select.h>
#endif /* AIX */
#ifdef WIN32
#include <winsock2.h>
#endif

#include "config.h"

#define DCC_CHAT        ((unsigned) 0x0001)
#define DCC_FILEOFFER   ((unsigned) 0x0002)
#define DCC_FILEREAD    ((unsigned) 0x0003)
#define DCC_TALK        ((unsigned) 0x0004)
#define DCC_SUMMON      ((unsigned) 0x0005)
#define DCC_RAW_LISTEN  ((unsigned) 0x0006)
#define DCC_RAW         ((unsigned) 0x0007)
#define DCC_TYPES       ((unsigned) 0x000f)

#define DCC_WAIT        ((unsigned) 0x0010)
#define DCC_ACTIVE      ((unsigned) 0x0020)
#define DCC_OFFER       ((unsigned) 0x0040)
#define DCC_DELETE      ((unsigned) 0x0080)
#define DCC_TWOCLIENTS  ((unsigned) 0x0100)
#define DCC_STATES      ((unsigned) 0xfff0)
 

typedef	struct DCC_struct
{
	unsigned 	flags;
	int	read;
	int	write;
	int	file;
	int	filesize;		
	char	description[MAXLEN];	
	char	user[MAXLEN];
 	char	ip_addr[20];		/* remote in ircII */
	int	port;			/* remport */
	struct  DCC_struct      *next;
	long	bytes_read;
	long	bytes_sent;
	long	lasttime;		/* can be used for idletime */
	long	starttime;
} DCC_list;

DCC_list	*search_list(const char *name, const char *user, unsigned int type);
int		do_dcc( DCC_list *Client );
void	reply_dcc( char *from, char *to, char *rest );
void	show_dcclist( const char *from );
char	*dcc_time( long time );
void	add_client( DCC_list *Client );
int		delete_client( DCC_list *Client );
void	close_all_dcc();
void    register_dcc_offer( char *from, char *type, char *description, 
								char *inetaddr, char *port, char *size );
void	parse_dcc( fd_set *read_fds );
void	process_dcc( char *from, char *rest );
void	dcc_chat( const char *from, const char *rest );
int		send_chat(const char *to,const char *text );
void	dcc_sendfile( char *from, char *rest );
void	dccset_fds(fd_set *rds, fd_set *wds);

#endif /* DCC_H */

// Local variables:
// coding: utf-8
// end:
