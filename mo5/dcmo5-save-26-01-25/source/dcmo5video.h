#ifndef __DCMO5VIDEO__
#define __DCMO5VIDEO__

#include <SDL.h>

extern SDL_Surface *screen; // surface d'affichage de l'ecran
extern int xbitmap;         // largeur ecran = 320 + 2 bordures de 8
extern int ybitmap;         // hauteur ecran = 200 + 2 bordures de 8
extern int ystatus;         // hauteur status bar
extern int xclient;         // largeur fenetre utilisateur
extern int yclient;         // hauteur ecran MO5 dans fenetre utilisateur
extern int xmouse;          // abscisse souris dans fenetre utilisateur
extern int ymouse;          // ordonn�e souris dans fenetre utilisateur
extern int framedelay;      // nombre de VBL entre deux affichages de l'ecran

void Initpalette();
void Displayscreen();
void ComposeMO5line(int a);
void Displayline(int n);
void Resizescreen(int x, int y);
void Setmouseposition(int x, int y);

#endif