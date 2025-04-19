#ifndef __DCMO5KEYB__
#define __DCMO5KEYB__

#include "dcmo5global.h"

extern button mo5key[MO5KEY_MAX];
extern button joykey[JOYKEY_MAX];
extern button keyboardbutton[KEYBOARDBUTTON_MAX];
extern button joystickbutton[JOYSTICKBUTTON_MAX];

extern char mo5keycode[256]; // scancode mo5 en fonction du scancode pc
extern char mo5joycode[256]; // numero bouton joystick en fonction du scancode pc

extern int lastkeycode;      // keycode derniere touche enfoncee
extern int lastkeysym;       // keysym derniere touche enfoncee

// Emulation clavier : affichage du nom de la derniere touche pressee
void Displaykey();

// Emulation manettes : affichage du nom de la derniere touche pressee
void Displayjoy();

// Création de la boite de dialogue du clavier
void Clavier();

// Création de la boite de dialogue des manettes
void Manettes();

// Restauration de la configuration par defaut des touches
void Restorekeydefault();

// Restauration de la configuration par defaut des manettes
void Restorejoydefault();

// Sauvegarde de la configuration du clavier
void Savekeyfile();

// Sauvegarde de la configuration des manettes
void Savejoyfile();

// Traitement des clics boite de dialogue clavier
void Keyclick();

// Traitement des clics boite de dialogue manettes
void Joyclick();

// Initialisation du clavier
void Keyboardinit();

// Key up
void Keyup(int keysym, int scancode);

// Key down
void Keydown(int keysym, int scancode);

// Test touches SHIFT, ALT et CTRL gauche et droite
void Testshiftkey();

#endif