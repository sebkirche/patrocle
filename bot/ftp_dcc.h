/*
 ftp-dcc.h - ftp-interface for dcc
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
 
 Provide a ftp-ish interface for dcc
 */

#ifndef _FTP_DCC_H_
#define _FTP_DCC_H_

#ifdef NEXT
#  include <sys/types.h>
#  include <sys/dir.h>
#else
#  include <dirent.h>
#endif /* NEXT */
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

/* 
 * I hate this.. 
 * Thanx for fixing Mozgy
 */
#ifndef _POSIX_SOURCE
#  ifndef _S_IFMT
#    define _S_IFMT 0170000
#  endif /* _S_IFMT */
#  ifndef _S_IFLNK
#    define _S_IFLNK 0120000
#  endif /* _S_IFLNK */
#  ifndef _S_IFSOCK
#    define _S_IFSOCK 0140000
#  endif /* _S_IFSOCK */
#  ifndef S_ISLNK
#    define S_ISLNK(mode)	(((mode) & (_S_IFMT)) == (_S_IFLNK))
#  endif /* S_ISLNK */
#  ifndef S_ISSOCK 
#    define S_ISSOCK(mode)	(((mode) & (_S_IFMT)) == (_S_IFSOCK))
#  endif /* S_ISSOCK */
#endif /* NOT _POSIX_SOURCE */

void	 pwd(char *from);
char	*real_root(char *path, char *buf);
int		 rr_stat(char *name, struct stat *buf);
int		 rr_access(char *name, int mode);
DIR		*rr_opendir(char *dirname);
char	*make_dir(char *old, char *new, char *dirbuffer);
void	 sort_dir(char *list[], int n);
void	 output_dir(char *from, char *path, char *list[], int n);
int		 read_dir(char *list[], char *path, char *pattern);
void	 close_dir(char *list[], int n);
void	 do_ls(char *from, char *path);
void	 send_file(char *from, char *name);
void	 do_chdir(char *from, char *new_dir);
FILE    *openindex(char *from,	char *name);

#endif

// Local variables:
// coding: utf-8
// end:

