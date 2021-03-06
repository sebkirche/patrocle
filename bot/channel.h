/*
 channel.h - a better implementation to handle channels on IRC
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
 */
#ifndef CHANNEL_H
#define CHANNEL_H

#include <iconv.h>
#include "chanuser.h"
#include "config.h"

/*
 * SOFTRESET means: only join "failed" channels
 * HARDRESET: delete all users and join all channels (i.e. after a kill)
 */
#define SOFTRESET 0
#define HARDRESET 1

typedef struct	BAN_struct
{
	char	*banstring;		/* banned nick!user@host */
	struct	BAN_struct	*next;	/* Next ban */
} BAN_list;

typedef	struct	CHAN_struct
{
	char	*name;			/* channel's name */
	int	active;				/* are we there? */
	USER_list	*users;		/* user on that channel */
	BAN_list	*banned;	/* banned user on channel */
	unsigned	int	mode;	/* private etc. */
	char	*key;			/* usefull if +k */
	char	limit[20];		/* if +l (why char? shut up! :) */
	char	*topic;			/* Startup-topic */
	char	*mod;			/* Startup-mode */
	char	*encoding;		/* encoding used to talk in the channel */
	int		utf8;			/* utf-8 flag, faster than testing encoding every time */
	iconv_t encoder;		/* iconv descriptor for encoding if channel is not ascii/latin */
	iconv_t decoder;		/* iconv descriptor for decoding if channel is not ascii/latin */
	int     talk;           /* If the bot can talk in the channel */
	int     log;            /* can we log this channel? */
	struct	CHAN_struct	*next;	/* ptr to next channel */
} CHAN_list;

#define MODE_CHANOP	CHFL_CHANOP
#define MODE_VOICE	CHFL_VOICE
#define MODE_PRIVATE	0x0004
#define MODE_SECRET	0x0008
#define MODE_MODERATED	0x0010
#define MODE_TOPICLIMIT	0x0020
#define MODE_INVITEONLY 0x0040
#define MODE_NOPRIVMSGS	0x0080
#define MODE_KEY	0x0100
#define MODE_BAN	0x0200	
#define MODE_LIMIT	0x0400	
#define MODE_FLAGS	0x07ff


CHAN_list		*search_chan(const char *);
void			 add_channel(CHAN_list *);
int				 delete_channel(CHAN_list *);
void			 delete_all_channels();
void			 copy_channellist(CHAN_list *);
int				 join_channel(const char *, const char *, const char *, const char *, int);
int				 leave_channel(const char *);
int				 mark_success(const char *);
int				 mark_failed(const char *);
int				 user_nb_on_channel(CHAN_list *);
void			 show_channellist(const char *);
void			 reset_channels(int);
char			*currentchannel();
int				 add_user_to_channel(const char *, const char *, const char *, const char *);
int				 remove_user_from_channel( const char *, const char *);
void			 change_nick(const char *, const char *);
void			 remove_user(const char *);
int				 show_users_on_channel(const char *, const char *);
void			 add_channelmode(const char *, unsigned int, const char *);
void			 del_channelmode(const char *, unsigned int, const char *);
void			 change_usermode(const char *, const char *, unsigned int);
int				 open_channel(const char *);
void			 channel_massop(const char *, const char *);
void			 channel_massdeop(const char *, const char *);
void			 channel_masskick(const char *, const char *);
int				 invite_to_channel(const char *, const char *);
char			*username(const char *);
char			*channelname(const char *);
unsigned int	 usermode(const char *, const char *);
BAN_list		*search_ban(BAN_list **, const char *);
void			 add_ban(BAN_list **, const char *);
int				 delete_ban(BAN_list **, const char *);
void			 clear_all_bans(BAN_list **);
void			 channel_massunban(const char *);
void			 channel_unban(const char *,	const char *);
int				 find_highest(const char *,	const char *);
int				 is_log_on(const char *);

#endif /* CHANNEL_H */

// Local variables:
// coding: utf-8
// end:

