/*
 channel.c - a better implementation to handle channels on IRC
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

#include <stddef.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <iconv.h>

#include "channel.h"
#include "chanuser.h"
#include "config.h"
#define _GNU_SOURCE
#include "fnmatch.h"
#include "misc.h"
#include "send.h"
#include "userlist.h"
#include "vladbot.h"
#include "vlad-ons.h"

//FIXME : weird, but fixes a link problem with my OSX
//#ifdef __APPLE__
//#define iconv_open iconv_open
//#define iconv iconv
//#define iconv_close iconv_close
//#endif

extern	botinfo	*currentbot;
extern	int	userlevel(const char *from);
extern	int	protlevel(const char *from);
extern	int	shitlevel(const char *from);
extern char *utf8;
extern char *latin1;

static	char	channel_buf[MAXLEN];

CHAN_list	*search_chan(const char *name )
{
	CHAN_list	*Channel;

	for( Channel = currentbot->Channel_list; Channel; Channel = Channel->next )
		if( STRCASEEQUAL( name, Channel->name ) )
			return(Channel);
	return(NULL);
}

void	add_channel( CHAN_list *Channel )
/*
 * adds Channel to the list... 
 */
{
	Channel->next = currentbot->Channel_list;
	currentbot->Channel_list = Channel;
}

int	delete_channel( CHAN_list *Channel )
/*
 * removes channel from list 
 */
{
	CHAN_list	**old;

	for( old = &(currentbot->Channel_list); *old; old = &(**old).next )
		if( *old == Channel ){
			*old = Channel->next;
			clear_all_users(&(Channel->users));
			free(Channel->name);
			free(Channel->key);
			free(Channel->topic);
			free(Channel->mod);
			free(Channel->encoding);
			if(Channel->encoder != (iconv_t)-1)
				iconv_close(Channel->encoder);
			if(Channel->decoder != (iconv_t)-1)
				iconv_close(Channel->decoder);
			free(Channel);
			return(TRUE);
		}
	return( FALSE );
}

void	delete_all_channels()
{
	CHAN_list	*Channel;

	for(;(Channel = currentbot->Channel_list);)
		delete_channel(Channel);
}

void	copy_channellist(CHAN_list *c_list)
{
	CHAN_list *tmp;

	for(tmp = c_list; tmp; tmp = tmp->next)
		join_channel(tmp->name, tmp->topic, tmp->mod, tmp->encoding, FALSE);
}

int	join_channel(const char *name, const char *topic, const char *mode, const char *encoding, int dojoin)
/*
 * Tries to join channel "name" and adds it to the channellist.
 * If the join is not succesfull, it'll be noticed by parseline
 * and active can be marked FALSE
 *
 * It could very well be that the userlist is not empty (after a kick,
 * kill). So clean it up first. Same for modes.
 */
{
	CHAN_list	*Channel;

	if(!ischannel(name))
		return(FALSE);

	if( (Channel = search_chan( name )) == NULL ){
/*
 * A completely new channel 
 */
		if( (Channel = (CHAN_list*)malloc( sizeof( *Channel ) ) )
			== NULL )
			return(FALSE);
		mstrcpy(&Channel->name, name);
		Channel->users = NULL;
		Channel->banned = NULL;
		Channel->mode = 0;
		mstrcpy(&Channel->key,"");
		mstrcpy(&Channel->topic, topic?topic:"");
		mstrcpy(&Channel->mod, mode?mode:"");
		mstrcpy(&Channel->encoding, encoding?encoding:"");
		Channel->encoder = Channel->decoder = (iconv_t)-1;
		printf("join channel %s in encoding %s\n", name, encoding);
		if(STRCASEEQUAL(encoding, "utf-8")){
			Channel->utf8 = TRUE;
		}else if((STRCASEEQUAL(encoding, "latin-1")) || (STRCASEEQUAL(encoding, "iso-8859-1"))){
			Channel->encoder = iconv_open(latin1, utf8);
			Channel->decoder = iconv_open(utf8, latin1);
		}else{
			Channel->encoder = iconv_open(encoding, utf8);
			Channel->decoder = iconv_open(utf8, encoding);			
		}

		Channel->talk = TRUE;
#ifdef LOG
		Channel->log  = TRUE;
#else
		Channel->log  = FALSE;
#endif
		add_channel(Channel);
		if(dojoin){
			sendjoin(name);
			Channel->active = TRUE;		/* assume it went ok */	
		}
		else
			Channel->active = FALSE;
	}
	else{	/* rejoin */
		clear_all_users(&(Channel->users));
		clear_all_bans(&(Channel->banned));
		Channel->mode = 0;
		/* sendjoin doesn't work with keys (fix this) */
		send_to_server("JOIN %s %s", name, 
					   *(Channel->key)?Channel->key:"");
		Channel->active = TRUE;		/* assume it went ok */	
	}	
	/*
	 * here we should send something like sendwho channel );
	 * Yup..., also send something to get the channelmodes
	 * -no, do this when we joined the channel
	 */
 	currentbot->Current_chan = Channel;
	return( TRUE );
}

int	leave_channel(const char *name)
/*
 * removes channel "name" from the list and actually leaves
 * the channel
 */
{
	CHAN_list	*Dummy;

	if((Dummy = search_chan(name)) != NULL){
		sendpart(name);
		if(Dummy == currentbot->Current_chan)
			currentbot->Current_chan = currentbot->Channel_list;
		delete_channel(Dummy);
		/* Channel was found and we left it
		 * (or at least  removed it from the list)
		 */
		return(TRUE);		
	}
	/*
	 * There was obviously no such channel in the list
	 */
	return(FALSE);
}

int	mark_success(const char *name)
/*
 * Marks channel "name" active
 */
{
	CHAN_list	*Dummy;

	if((Dummy = search_chan(name)) != NULL){
		Dummy->active = TRUE;
		/* Perhaps this shouldn't be here, but it makes things 
		   alot easier */
		if(*Dummy->topic)
			sendtopic(name, Dummy->topic);
		sendmode(name, "%s", Dummy->mod);
		return(TRUE);
	}
	return(FALSE);
}

int	mark_failed( const char *name )
/*
 * Marks channel "name" not active
 */
{
	CHAN_list	*Dummy;

	if( (Dummy = search_chan( name ) ) != NULL ){
		Dummy->active = FALSE;
		return(TRUE);
	}
	return(FALSE);
}

int     user_nb_on_channel( CHAN_list *Channel ) 
{
	USER_list *User;
	int N;

	N = 0;
	
	for(User = Channel->users; User; User = User->next)
		N++;
	
	return(N);
}


void	show_channellist(const char *user)
{
	CHAN_list	*Channel;
	unsigned int	mode;
	char		modestr[MAXLEN];

	if( currentbot->Channel_list == NULL ){
		send_to_user( user, "I'm not active on any channels" );
		return;
	}

	send_to_user( user, "Active on: %s", currentbot->Channel_list->name );
	for( Channel = currentbot->Channel_list; Channel; Channel = Channel->next ){
		mode = Channel->mode;
		strcpy( modestr, "" );
		if( mode&MODE_PRIVATE ) strcat( modestr, "p" );
		if( mode&MODE_SECRET ) strcat( modestr, "s" );
		if( mode&MODE_MODERATED ) strcat( modestr, "m" );
		if( mode&MODE_TOPICLIMIT ) strcat( modestr, "t" );
		if( mode&MODE_INVITEONLY ) strcat( modestr, "i" );
		if( mode&MODE_NOPRIVMSGS ) strcat( modestr, "n" );
		if( mode&MODE_KEY ) {
			strcat( modestr, "k " );
			strcat( modestr, Channel->key?Channel->key:"???" );
			strcat( modestr, " " );
		}
		if( mode&MODE_LIMIT ){
			strcat( modestr, "l " );
			strcat( modestr, Channel->limit?Channel->limit:"???" );
		}
		send_to_user( user, "   -  %10s, mode: +%-5s %-9s %-12s%-8s", 
					  Channel->name, modestr,
					  (Channel->talk?", talking":""),
					  (Channel->encoding?Channel->encoding:""),
					  (Channel->log?", log on":""));
	}
	return;
} 

void	reset_channels( int Mode )
/*
 * This function tries to join all inactive channels
 */
{
	CHAN_list	*Channel;

	for(Channel = currentbot->Channel_list; Channel; Channel = Channel->next)
		if((Mode == HARDRESET) || !(Channel->active))
			join_channel(Channel->name, Channel->topic, Channel->mod, Channel->encoding, TRUE);
}

char	*currentchannel()
/*
 * returns name of current channel, i.e. the last joined
 */
{
	if( currentbot->Channel_list )
		strcpy( channel_buf, currentbot->Channel_list->name );
	else
		strcpy( channel_buf, "#0" );
	return(channel_buf);
}

/*
 * Ok, that was the basic channel stuff... Now some functions to
 * add users to a channel, remove them and one to change a nick on ALL
 * channels.
 */

int	add_user_to_channel( const char *channel, const char *nick, 
                             const char *user, const char *host )
{
	CHAN_list	*Dummy;

	if( (Dummy = search_chan( channel )) == NULL )
		return(FALSE);
	add_user( &(Dummy->users), nick, user, host );
	return(TRUE);
}

int	remove_user_from_channel( const char *channel, const char *nick )
{
	CHAN_list	*Dummy;
	char *Topic;
	char *Mode;
	char *Encoding;

	if( (Dummy = search_chan( channel )) == NULL )
		return(FALSE);
	delete_user( &(Dummy->users), nick );

	/* If there is only the bot in this channel and he is not OP,
	   we left the channel and come back. Thus, we can get OP */
	if(user_nb_on_channel(Dummy) == 1
	    && !(get_usermode(Dummy->users)&MODE_CHANOP)) {
		Topic = strdup(Dummy->topic);
		Mode  = strdup(Dummy->mod);
		Encoding = strdup(Dummy->encoding);
		leave_channel(channel);
		join_channel(channel, Topic, Mode, Encoding, TRUE);
	  
		sendmode(channel, Mode);
		sendtopic(channel, Topic);

		free(Topic);
		free(Mode);
		free(Encoding);
		
	}

	return(TRUE);
}

void	change_nick( const char *oldnick, const char *newnick )
/*
 * Searches all channels for oldnick and changes it into newnick
 */
{
	CHAN_list	*Channel;

	for( Channel = currentbot->Channel_list; Channel; Channel = Channel->next )
		change_user_nick( &(Channel->users), oldnick, newnick );
}

void	remove_user( const char *nick )
/*
 * Remove a user from all channels (signoff)
 */
{
	CHAN_list	*Channel;
	CHAN_list       *Next = NULL;
	char *Topic;
	char *Mode;
	char *Name;
	char *Encoding;
	int  Destruction = 0;

	for( Channel = currentbot->Channel_list; Channel;
	     Channel = (Destruction?Next:Channel->next) ){
	    delete_user( &(Channel->users), nick );
	    Destruction = 0;

		/* if the channel is left empty, we get the control */
	    if(user_nb_on_channel(Channel) == 1
			&& !(get_usermode(Channel->users)&MODE_CHANOP)) {
			//strdup the strings because they will be freed in leave_channel/delete channel
			Topic = strdup(Channel->topic);
			Mode  = strdup(Channel->mod);
			Name  = strdup(Channel->name);
			Encoding = strdup(Channel->encoding);
			Next  = Channel->next;
			leave_channel(Name);
			Destruction = 1;
			join_channel(Name, Topic, Mode, Encoding, TRUE);
			//free the strings (join_channel() has made its own copies
			free(Topic);
			free(Mode);
			free(Name);
			free(Encoding);
	    }
	}
}

int	show_users_on_channel( const char *from, const char *channel )
{
	CHAN_list       *Dummy;
	USER_list	*Blah;
	char		modestr[MAXLEN];

	if( (Dummy = search_chan( channel )) == NULL )
		return(FALSE);
	
	Blah = Dummy->users;
	while( Blah ){
		strcpy(modestr, "");
		if( get_usermode(Blah)&MODE_CHANOP ) strcat(modestr, "+o");
		if( get_usermode(Blah)&MODE_VOICE ) strcat(modestr, "+v" );
		send_to_user( from, "%s: %30s, %3du %3ds %3dp %3dr, %s", 
					  channel, get_username(Blah),
					  userlevel(get_username(Blah)),
					  shitlevel(get_username(Blah)),
					  protlevel(get_username(Blah)), 
					  rellevel(get_username(Blah)),
					  modestr );
		Blah = get_nextuser( Blah );
	}
	return(TRUE);
}

/*
 * now some functions to keep track of modes
 */

void	add_channelmode( const char *channel, unsigned int mode, const char *params )
{
	CHAN_list	*Channel;

	if((Channel = search_chan(channel))==NULL)
		return;

	switch(mode){
		case MODE_CHANOP:
		case MODE_VOICE:
			add_mode(&(Channel->users), mode, params);
			break;
		case MODE_KEY:
			Channel->mode |= MODE_KEY;
			free(Channel->key);
			mstrcpy(&Channel->key, params);
			break;
		case MODE_BAN:
			add_ban(&(Channel->banned), params);
			break;
		case MODE_LIMIT:
			Channel->mode |= MODE_LIMIT;
			strcpy(Channel->limit, params);
			break;
		default:
			Channel->mode |= mode;
			break;
	}
}

void	del_channelmode( const char *channel, unsigned int mode, const char *params )
{
	CHAN_list	*Channel;

	if((Channel = search_chan(channel))==NULL)
		return;
	
	switch(mode){
		case MODE_CHANOP:
		case MODE_VOICE:
			del_mode( &(Channel->users), mode, params );
			break;
		case MODE_KEY:
			Channel->mode &= ~MODE_KEY;
			break;
		case MODE_BAN:
			delete_ban(&(Channel->banned), params);
			break;
		case MODE_LIMIT:
			Channel->mode &= ~MODE_LIMIT;
			break;
		default:
			Channel->mode &= ~mode;
			break;
	}
}

void	change_usermode( const char *channel, const char *user, unsigned int mode )
{
	CHAN_list	*Channel;
	
	if( (Channel = search_chan( channel )) == NULL )
		return;		/* should not happen */

	add_mode( &(Channel->users), mode, user );
}
	
int	open_channel( const char *channel )
{
	CHAN_list       *Channel;
	
	if( (Channel = search_chan( channel )) == NULL )
		return(FALSE);
	
	if((Channel->mode)&MODE_KEY)
		sendmode(channel, "-ipslk %s", Channel->key);
	else
		sendmode(channel, "-ipsl");
	return(TRUE);
}

/*
 * Here is some mass-stuff. Perhaps it doesn't belong here (though..
 * I think this is a quite good place...).
 */

void	channel_massop( const char *channel, const char *pattern )
{
	CHAN_list       *Channel;
	USER_list	*Dummy;
	char		opstring[MAXLEN];
	int		i;

	if( (Channel = search_chan( channel )) == NULL )
		return;

	Dummy = Channel->users;
	while(Dummy){
		strcpy(opstring, "+ooo ");
		i=0;
		while(Dummy && (i<3)){
			if(!fnmatch(pattern, get_username(Dummy), FNM_CASEFOLD)&&
			   !(get_usermode(Dummy)&MODE_CHANOP) &&
			   (shitlevel(get_username(Dummy))==0)){
				strcat(opstring, " ");
				strcat(opstring, get_usernick(Dummy));
				i++;
			}
			Dummy=get_nextuser(Dummy);
		}
		sendmode(channel, "%s", opstring);
	}
}

void    channel_massdeop( const char *channel, const char *pattern )
{
	CHAN_list       *Channel;
	USER_list       *Dummy;
	char            deopstring[MAXLEN];
	int             i;
	
	if( (Channel = search_chan( channel )) == NULL )
		return;
	
	Dummy = Channel->users;
	while(Dummy){
		strcpy(deopstring, "-ooo ");
		i=0;
		while(Dummy && (i<3)){
			if(!fnmatch(pattern, get_username(Dummy), FNM_CASEFOLD)&&
			   (get_usermode(Dummy)&MODE_CHANOP)&&
			   (protlevel(get_username(Dummy))<50)){
				strcat(deopstring, " ");
				strcat(deopstring, get_usernick(Dummy));
				i++;
			}
			Dummy=get_nextuser(Dummy);
		}
		sendmode(channel, "%s", deopstring);
	}
}
 
void    channel_masskick( const char *channel, const char *pattern )
{
	CHAN_list       *Channel;
	USER_list       *Dummy;
	
	if( (Channel = search_chan( channel )) == NULL )
		return;
	
	Dummy = Channel->users;
	while(Dummy){
		if(!fnmatch(pattern, get_username(Dummy), FNM_CASEFOLD)&&
		   (protlevel(get_username(Dummy))<50))
			sendkick(channel, get_usernick(Dummy),
					 "**Masskick**");
		Dummy=get_nextuser(Dummy);
	}
}

/*
 * Some misc. function which deal with channels and users
 */

int	invite_to_channel( const char *user, const char *channel )
{
	if(search_chan( channel )){
		send_to_server("INVITE %s :%s", user, channel);
		return(TRUE);
	}
	return(FALSE);
}

char	*username( const char *nick )
/*
 * Searches all lists for nick and if it finds it, returns 
 * nick!user@host
 */
{
	CHAN_list	*Channel;
	USER_list	*User;

	for(Channel = currentbot->Channel_list; Channel; Channel = Channel->next)
		if((User = search_user(&(Channel->users), nick)))
			return(get_username(User));
	return(NULL);
}

char    *channelname(const char *nick)
/*
 * Searches all lists for nick and if it finds it, returns 
 * a name of channel on which he is
 */
{
	CHAN_list	*Channel;
	USER_list	*User;

	for(Channel = currentbot->Channel_list; Channel; Channel = Channel->next)
		if((User = search_user(&(Channel->users), nick)))
			return(Channel->name);
	return(NULL);
}

unsigned int	usermode( const char *channel, const char *nick )
/*
 * returns the usermode of nick on channel
 */
{
	CHAN_list	*Channel;
	USER_list	*Blah;

	if((Channel = search_chan(channel))==NULL)
		return(0);	/* or -1? */

	Blah=Channel->users;
	while( Blah ){
		if(STRCASEEQUAL(nick, get_usernick(Blah)))
			return(get_usermode(Blah));
		Blah=get_nextuser(Blah);
	}
	return(0);
}
	
/*
 * Here are the functions to bookkeep the banlist
 */

BAN_list	*search_ban(BAN_list **b_list, const char *banstring)
{
	BAN_list	*Banned;

	for(Banned = *b_list; Banned; Banned = Banned->next)
		if(STRCASEEQUAL(Banned->banstring, banstring))
			return(Banned);		
	return(NULL);
}

void	add_ban(BAN_list **b_list, const char *banstring)
{
	BAN_list	*new_ban;
	
	if( search_ban(b_list, banstring) != NULL )
		return;
	if( (new_ban = (BAN_list*)malloc(sizeof(*new_ban))) == NULL)
		return;
	mstrcpy(&new_ban->banstring, banstring );
	new_ban->next = *b_list;
	*b_list = new_ban;
}

int	delete_ban(BAN_list **b_list, const char *banstring)
{
	BAN_list	**old;
	BAN_list	*Dummy;
	
	if((Dummy = search_ban(b_list, banstring)) == NULL)
		return(FALSE);
	
	for(old = b_list; *old; old = &(*old)->next)
		if(*old == Dummy){
			*old = Dummy->next;
			free(Dummy->banstring);
			free(Dummy);
			return(TRUE);
		}
	return(FALSE);
}

void    clear_all_bans(BAN_list **b_list)
/*
 * Removes all entries in the list
 */
{
	BAN_list	**old;
	BAN_list	 *del;
	
	for(old = b_list; *old;){
		del = *old;
		*old = (*old)->next;
		free(del->banstring);
		free(del);
	}
}

void    channel_massunban(const char *channel)
{
	CHAN_list   *Channel;
	BAN_list    *Dummy;
	char		 unbanmode[5];
	char         unbanstring[MAXLEN];
	int          i;

	if( (Channel = search_chan( channel )) == NULL )
		return;

	Dummy = Channel->banned;
	while(Dummy){
		/* We can't just -bbb ban1 ban2,
		   because the third b will make the server
		   show the banlist etc. */
		strcpy(unbanmode, "-");
		strcpy(unbanstring, "");
		i=0;
		while(Dummy && (i<3)){
			strcat(unbanmode,"b");
			strcat(unbanstring, " ");
			strcat(unbanstring, Dummy->banstring);
			i++;
			Dummy=Dummy->next;
		}
		sendmode(channel, "%s %s", unbanmode, unbanstring);
	}
}

void	channel_unban(const char *channel, const char *user)
{
	CHAN_list   *Channel;
	BAN_list    *Dummy;
	char		 unbanmode[5];
	char         unbanstring[MAXLEN];
	int          i;

	if( (Channel = search_chan( channel )) == NULL )
		return;
	
	Dummy = Channel->banned;
	while(Dummy){
		strcpy(unbanmode, "-");
		strcpy(unbanstring, "");
		i=0;
		while(Dummy && (i<3)){
			if(!fnmatch(Dummy->banstring, user, FNM_CASEFOLD)){
				strcat(unbanmode, "b");
				strcat(unbanstring, " ");       
				strcat(unbanstring, Dummy->banstring);          
				i++;
			}        
			Dummy=Dummy->next;        
		}
		sendmode(channel, "%s %s", unbanmode, unbanstring);      
	}
}

int	find_highest(const char *channel, const char *pattern)
/*
 * Find user that matches pattern with the highest protlevel,
 * and return this level. Shitlevel must be 0
 *
 * Kinda kludgy, but I have to clean channel.c up anyhow...
 */
{
	CHAN_list	*Channel;
	USER_list   *Dummy;
	int			 highest = 0;

	if((Channel = search_chan(channel)) == NULL)
		return 0;

	Dummy = Channel->users;
	while(Dummy){
		if(!fnmatch(pattern, get_username(Dummy), FNM_CASEFOLD)&&
		   (shitlevel(get_username(Dummy))==0))
			if(protlevel(get_username(Dummy)) > highest)
				highest = protlevel(get_username(Dummy));  
		Dummy=get_nextuser(Dummy);
	}
	return highest;
}

int is_log_on(const char *to)
{
	CHAN_list *Canal;

	/* Si ce n'est pas un canal, on logge */
	if(!(Canal = search_chan(to)))
		return TRUE;
  
	return Canal->log;
}

// Local variables:
// coding: utf-8
// end:
