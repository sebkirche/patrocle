/*
 phrase.h - implementation of userlists
 Copyright (C) 1996, 1997 François Parmentier (H_I)
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

#ifndef PHRASE_H
#define PHRASE_H

typedef struct {
  char *NomStimulus;
  char *Stimulus;
  char *Auteur;
  int Present;
  int Actif;
} phr_tbl;

typedef struct 
{
  char *Reponse;
  char *NomStimulus;
  char *Auteur;
  char *Canal;
  int Active;
} rep_tbl;

extern phr_tbl **TableDesStimuli;
extern rep_tbl **TableDesReponses;
extern int TailleStim;
extern int TailleRep;

int SauveStimuli(char *nom_fichier);
int ChargeStimuli(char *nom_fichier);
int AjouteStimulus(char *from, char *channel, char *Stimulus, char *NomStimulus);
int SauveReponses(char *nom_fichier);
int ChargeReponses(char *nom_fichier);
int AjouteReponse(char *from, char *channel, char *Reponse, char *NomStimulus);
int SupprimeRep(int numero);
int SupprimeStim(int numero);
void LibereStimuli();
void LibereReponses();

#endif

// Local variables:
// coding: utf-8
// end:
