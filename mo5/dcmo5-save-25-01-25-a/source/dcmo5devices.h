#ifndef __DCMO5DEVICES__
#define __DCMO5DEVICES__

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

extern DIR *dmemo;  // pointeur directory pour recherche memo5
extern FILE *ffd;   // pointeur fichier disquette
extern FILE *fk7;   // pointeur fichier k7
extern FILE *fprn;  // pointeur fichier imprimante
extern int k7bit;   // puissance de 2 designant le bit k7 en cours
extern int k7octet;        // octet de la cassette en cours de traitement
extern int k7index;        // compteur du lecteur de cassette
extern int k7indexmax;     // compteur du lecteur de cassette en fin de bande
extern int k7protection;   // indicateur lecture seule pour la cassette
extern int fdprotection;   // indicateur lecture seule pour la disquette
extern char k7name[100];   // nom du fichier cassette
extern char fdname[100];   // nom du fichier disquette
extern char memoname[100]; // nom du fichier cartouche

// Emulation imprimante
void Imprime();

// Erreur lecture/ecriture fichier qd ou fd
void Diskerror(int n);

// Lecture d'un secteur
void Readsector();

// Ecriture d'un secteur
void Writesector();

// Formatage d'un disque
void Formatdisk();

void Loadfd(char *name);

// Emulation cassette
void Readoctetk7();

void Readbitk7();

void Writeoctetk7();

void Loadk7(char *name);

void Rewindk7();

// Emulation cartouche memo5
void Loadmemo(char *name);

// Emulation crayon optique
void Readpenxy();

// Initialisation noms de fichiers et pointeur de fonction de chargement
void Initfilenames();

#endif