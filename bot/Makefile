#
# Patrocle's Makefile
# Hacked from the original Achille's Makefile
# that is itself picked from VladBot - v2.1g
#


# These defines are supposed to work on Linux, Ultrix and SunOs 4.1.*
# with a good ansi-C compiler (i.e. gcc)
#
#CFLAGS = -g -Wall # for debugging with gdb/dbx
#CFLAGS = -O2 -s # -Wall  #for compiling with GNU/Linux

#for compiling with my iMac ia64 
CFLAGS = -arch x86_64 -g -Wall -I/usr/include #-I/opt/local/include
LDFLAGS = -arch x86_64 -L/usr/lib #-L/opt/local/lib DYLD_INSERT_LIBRARIES=/usr/lib/libMallocDebug.A.dylib

CC = gcc
RM = rm -f
CP = cp -r
INSTALL = install -s -m 700
DEFINES = 

# Use these for AIX-3.2.5:
# CC = cc
# CFLAGS = -O
# INSTALL = /usr/ucb/install -s -m 700
# DEFINES = -DAIX

# And these should make vladbot compile cleanly on Next
#CC = cc
#CFLAGS = -O2 -s -Wall
#DEFINES = -DNEXT -D_POSIX_SOURCE

#
# It seems that on Solaris some libraries have to be linked with the bot.
# If anyone knows which, pls mail me
#
# Supprimer les 2 lignes INCLUDES et LIBS pour Linux
# Perhaps something like
#INCLUDES = -I/usr/ucbinclude -L/usr/ucblib
#LIBS = -lucb -lsocket -lnsl
# seki: nouvelle ligne pour linux 2.6
LIBS = -lc -liconv -llua 

INSTALL_DIR = ..
INSTALL_NAME = patrocle

#
# Don't change anything below here...
#

OBJECTS = main.o vladbot.o cfgfile.o parse.o vlad-ons.o file.o misc.o send.o \
          ctcp.o server.o channel.o chanuser.o botlist.o userlist.o phrase.o \
          locuteur.o dcc.o ftext.o log.o note.o debug.o session.o ftp_dcc.o \
          fnmatch.o parsing.o parsetime.o luainterface.o
SOURCES = main.c vladbot.c cfgfile.c parse.c vlad-ons.c file.c misc.c send.c \
	  ctcp.c server.c channel.c chanuser.c botlist.c userlist.c phrase.c \
          locuteur.c dcc.c ftext.c log.c note.c debug.c session.c ftp_dcc.c \
          fnmatch.c parsing.c parsetime.c luainterface.c

all: bot help Makefile

.c.o: ; ${CC} ${CFLAGS} ${DEFINES} -c `pwd`/$<
hr_ftext.o: ; ${CC} ${CFLAGS} ${DEFINES} ${INCLUDES} -DSTAND_ALONE -c -o hr_ftext.o ftext.c

# Enlever le "purify" de la ligne suivante s'il y en a un
bot: ${OBJECTS} Makefile
	${CC} ${LDFLAGS} ${DEFINES} ${INCLUDES} -o ${INSTALL_NAME} ${OBJECTS} ${LIBS}

help:   helpread.o hr_ftext.o 
	${CC} ${LDFLAGS} ${DEFINES} -DSTAND_ALONE -o helpread helpread.o hr_ftext.o



install: all
	${INSTALL} ${INSTALL_NAME}  ${INSTALL_DIR}/${INSTALL_NAME}
	cp help.bot ${INSTALL_DIR}/help.bot
	@echo Don\'t forget to create/copy the \*.list, help.bot and infile.cfg files!

distribution:
	tar -cf vladbot.tar *.c *.h *.list Makefile README vladbot.1
	gzip -9 vladbot.tar

clean:
	${RM} bot ${INSTALL_NAME} helpread *.o *.bak core 

# Dependencies

cfgfile.o : cfgfile.c cfgfile.h vladbot.h session.h config.h userlist.h channel.h \
  chanuser.h dcc.h debug.h misc.h parsing.h botlist.h Makefile
channel.o : channel.c channel.h chanuser.h config.h fnmatch.h misc.h send.h \
  userlist.h vladbot.h session.h dcc.h  Makefile
chanuser.o : chanuser.c chanuser.h config.h misc.h  Makefile
ctcp.o : ctcp.c config.h ctcp.h debug.h misc.h send.h vladbot.h session.h userlist.h \
  channel.h chanuser.h dcc.h  Makefile
dcc.o : dcc.c config.h dcc.h debug.h log.h misc.h send.h server.h session.h \
  vladbot.h userlist.h channel.h chanuser.h  Makefile
debug.o : debug.c vladbot.h session.h config.h userlist.h channel.h chanuser.h \
  dcc.h debug.h  Makefile
file.o : file.c file.h userlist.h config.h  Makefile
fnmatch.o : fnmatch.c fnmatch.h  Makefile
ftext.o : ftext.c vladbot.h session.h config.h userlist.h channel.h chanuser.h \
  dcc.h misc.h  Makefile
ftp_dcc.o : ftp_dcc.c config.h debug.h fnmatch.h ftp_dcc.h log.h misc.h session.h \
  vladbot.h userlist.h channel.h chanuser.h dcc.h  Makefile
helpread.o : helpread.c ftext.h config.h  Makefile
log.o : log.c dcc.h config.h vladbot.h session.h userlist.h channel.h chanuser.h \
  log.h  Makefile
main.o : main.c channel.h chanuser.h config.h debug.h log.h note.h parse.h session.h \
  userlist.h vladbot.h dcc.h  Makefile
misc.o : misc.c config.h misc.h  Makefile
note.o : note.c config.h fnmatch.h note.h misc.h parsing.h session.h vladbot.h \
  userlist.h channel.h chanuser.h dcc.h  Makefile
parse.o : parse.c channel.h chanuser.h config.h ctcp.h debug.h log.h misc.h \
  parse.h send.h vladbot.h session.h userlist.h dcc.h vlad-ons.h  Makefile
parsing.o : parsing.c parsing.h misc.h config.h  Makefile
send.o : send.c dcc.h config.h send.h debug.h  Makefile
server.o : server.c config.h debug.h vladbot.h session.h userlist.h channel.h \
  chanuser.h dcc.h  Makefile
session.o : session.c config.h misc.h send.h session.h vladbot.h userlist.h \
  channel.h chanuser.h dcc.h vlad-ons.h  Makefile
userlist.o : userlist.c config.h fnmatch.h misc.h send.h userlist.h  Makefile
userlist.o : locuteur.c config.h fnmatch.h misc.h send.h locuteur.h file.h Makefile
botlist.o : botlist.c config.h fnmatch.h misc.h send.h botlist.h Makefile
vlad-ons.o : vlad-ons.c channel.h chanuser.h config.h dcc.h debug.h ftext.h \
  ftp_dcc.h misc.h note.h parsing.h send.h session.h userlist.h vladbot.h vlad-ons.h locuteur.h phrase.h botlist.h Makefile
vladbot.o : vladbot.c channel.h chanuser.h config.h dcc.h debug.h file.h userlist.h \
  misc.h parse.h send.h server.h vladbot.h session.h botlist.h Makefile
locuteur.o : locuteur.c locuteur.h config.h fnmatch.h misc.h send.h file.h \
             vladbot.h Makefile
phrase.o : phrase.c phrase.h config.h Makefile
luainterface.o : luainterface.c log.h cfgfile.h debug.h misc.h parsing.h vlad-ons.h send.h vladbot.h phrase.h
