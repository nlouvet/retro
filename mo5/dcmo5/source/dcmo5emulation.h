#ifndef __DCMO5EMULATION__
#define __DCMO5EMULATION__

#include <stdint.h>

// memory
extern int8_t car[0x10000]; // espace cartouche 4x16K
extern int8_t ram[0xc000];  // ram 48K
extern int8_t port[0x40];   // ports d'entree/sortie
// pointers
extern int8_t *ramvideo; // pointeur couleurs ou formes
extern int8_t *ramuser;  // pointeur ram utilisateur fixe
extern int8_t *romsys;   // pointeur rom systeme
extern int8_t *rombank;  // pointeur banque rom ou cartouche

// flags cartouche
extern int cartype;  // type de cartouche (0=simple 1=switch bank, 2=os-9)
extern int carflags; // bits0,1,4=bank, 2=cart-enabled, 3=write-enabled
// keyboard, joysticks and mouse
extern int touche[58];   // etat des touches MO5
extern int joysposition; // position des manches
extern int joysaction;   // position des boutons d'action
extern int xpen, ypen;   // coordonnees crayon optique
extern int penbutton;    // mouse left button state
// affichage
extern int videolinecycle;  // compteur ligne (0-63)
extern int videolinenumber; // numero de ligne video affich�e (0-311)
extern int bordercolor;     // couleur de la bordure de l'�cran
// divers
extern int opcycles; // nombre de cycles de la derni�re op�ration
extern int sound;    // niveau du haut-parleur

extern int pause6809; // processor pause state

void MO5videoram();
void MO5rombank();

void Switchmemo5bank(unsigned short a);

int Iniln();
int Initn();

void Joysemul(int i, int state);
void Joysmove(int n, int x, int y);

void Initprog();
void Hardreset();
int Entreesortie(int opcode);
int Run(int n);

void Mputc(uint16_t a, int8_t c);
void Mputw(uint16_t a, int16_t w);

char Mgetc(uint16_t a);
short Mgetw(uint16_t a);

#endif
