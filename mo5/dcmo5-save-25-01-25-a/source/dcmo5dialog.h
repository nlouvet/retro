#ifndef __DCMO5DIALOG__
#define __DCMO5DIALOG__

#include <SDL.h>
#include <SDL_ttf.h>

#include "dcmo5global.h"
#include "dcmo5boutons.h"

#define DIRLIST_LENGTH 256
#define DIRLIST_NMAX 500

extern const SDL_Color textnoir;  // couleur d'ecriture noir
extern const SDL_Color textblanc; // couleur d'ecriture blanc

extern const int rmask, gmask;
extern const int bmask, amask;
extern const int blanc, noir;
extern const int gris, bleu;
extern const int grisf;

extern button statusbutton[STATUSBUTTON_MAX];
extern button closebutton;

extern SDL_Surface *textbox;       // surface d'affichage de texte
extern SDL_Surface *dialogbox;     // surface d'affichage dialogbox
extern SDL_Surface *statusbar;     // surface de la barre de statut
extern TTF_Font *font11;           // police d'ecriture taille 11
extern TTF_Font *font9;            // police d'ecriture taille 9
extern SDL_Rect dialogrect;        // position dialogbox
extern int dialog;                 // 0=rien 1=message, 2=options 3=clavier 4=menu
extern int mouselastx, mouselasty; // position souris dans dialogbox
extern int dircount;               // nombre de fichiers dans le repertoire
extern char dirlist[DIRLIST_NMAX][DIRLIST_LENGTH]; // liste des fichiers du repertoire
extern int dirin, dirmax;           // plage de numeros de fichiers affiches
extern void (*Load[3])(char *name); // pointeur fonction de chargement de fichier

// Tri du repertoire path avec filtre sur l'extension
void Sortdirectory(char *path, char *extension);

// Draw textbox
void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c);

// Ajout du texte 36 + i sur un bouton
void Buttontext(int i, int x, int y);

// Draw message box
void Drawmessagebox(char *titre, char *text1[], char *text2[]);

// Index cassette
void Drawk7index();

// Draw status bar
void Drawstatusbar();

// Init status bar
void Initstatusbar();

// Create box
void Createbox();

// Create dialog box
void Createdialogbox(int w, int h);

// Traitement des clics souris dans la barre de statut
void Statusclick();

// Traitement des clics souris dans la boite de dialogue
void Dialogclick();

// Traitement des clics souris
void Mouseclick();

// Draw menu box
void Drawmenubox();

// Draw directory listbox
void Drawdirlistbox(int n);

// Traitement des clics dans un menu deroulant
void Menuclick();

#endif