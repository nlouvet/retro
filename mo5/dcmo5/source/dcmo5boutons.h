#ifndef __DCMO5BOUTONS__
#define __DCMO5BOUTONS__

#include "dcmo5global.h"

typedef struct {
    char name[12]; // nom du bouton
    int x;         // ordonnee du coin haut-gauche
    int y;         // abscisse du coin haut-gauche
    int w;         // largeur en pixels
    int h;         // hauteur en pixels
    int n;         // numero de la surface SDL
} button;

// surfaces des boutons ([0]=relache, [1]=enfonce)
extern SDL_Surface *buttonsurface[KEYBUTTON_MAX + JOYBUTTON_MAX + OTHERBUTTON_MAX][2];

// Initialisation des surfaces des touches
void Initkeysurfaces();

// Initialisation des surfaces des boutons
void Initbuttonsurfaces();

#endif
