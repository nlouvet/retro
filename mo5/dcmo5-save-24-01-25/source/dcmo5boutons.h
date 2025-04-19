#ifndef __DCMO5BOUTONS__
#define __DCMO5BOUTONS__

#include "dcmo5global.h"

// surfaces des boutons ([0]=relache, [1]=enfonce)
extern SDL_Surface *buttonsurface[KEYBUTTON_MAX + JOYBUTTON_MAX + OTHERBUTTON_MAX][2];

// Initialisation des surfaces des touches
void Initkeysurfaces();

// Initialisation des surfaces des boutons
void Initbuttonsurfaces();

#endif
