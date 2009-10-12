/*
 send.h send stuff to the server, but not the basic stuff,
 more specified, like sendprivmsg, sendmode etc...
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

#ifndef	SEND_H
#define SEND_H

int	sendprivmsg(const char *sendto, const char *format, ...);
int sendaction(const char *sendto, const char *format, ...);
int	sendnotice(const char *sendto, const char *format, ...);
int sendregister(const char *nick, const char *login, const char *ircname);
int	sendtopic(const char *channel, const char *topic);
int sendping(const char *to);
int sendpong(const char *to);
int sendnick(const char *nick);
int sendjoin(const char *channel);
int sendpart(const char *channel);
int sendquit(const char *reason);
int	sendmode(const char *to, const char *format, ...);
int sendkick(const char *channel, const char *nick, const char *reason );
int	send_ctcp_reply(const char *to, const char *format, ...);
int	send_ctcp(const char *to, const char *format, ...);
int sendison(const char *nick);
int senduserhost(const char *nick);
int	send_to_user(const char *to, const char *format, ...);

#endif /* SEND_H */

// Local variables:
// coding: utf-8
// end:
