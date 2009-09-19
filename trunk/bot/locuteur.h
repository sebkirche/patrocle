/*
 locuteur.h - implementation of userlists
 Copyright (C) 1996, 1997 François Parmentier (H_I)
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

#ifndef LOCUTEUR_H
#define LOCUTEUR_H

typedef struct locuteur_struct {
	char *nuh;                /* H_I!irc@blah */
	int bonus_malus;          /* Cumul points intermediaires */
	int PremierContact;       /* heure du premier contact */
	int DernierContact;       /* Heure du dernier contact */
	int Bonjours;             /* Nombre de bonjours */
	int Veridique;            /* Booleen disant si le locuteur s'est identifie */
	long LastSeen;			  /* dernier signe de vie du locuteur */
  struct locuteur_struct *suivant; /* locuteur suivant */
} locuteur;

locuteur **InitLocuteurListe ();
locuteur *LocuteurExiste (locuteur **, const char *);
locuteur *LocuteurNickExiste (locuteur **, char *);
locuteur *AjouteLocuteur (locuteur **, const char *);
void      AjoutePoints (locuteur *, int);
void      AnnulePoints (locuteur *);
void      AnnuleBonjours (locuteur *);
void      MAJPremierContact (locuteur *);
void      MAJDernierContact (locuteur *);
void	  MAJDerniereActivite (locuteur *);
void      EnleveLocuteur (locuteur **, locuteur *);
void      MontreLocuteurs (locuteur **, char *, char *);
int       SauveLocuteurs (locuteur **, char *);
void      DetruitListe (locuteur **);
int       ChargeLocuteurs (char *, locuteur **);
void      NettoieListeLocuteurs (locuteur **);
int       loclevel (char *);
int       loctrue (char *);
#endif

// Local variables:
// coding: utf-8
// end:
