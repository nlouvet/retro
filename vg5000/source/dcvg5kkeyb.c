//////////////////////////////////////////////////////////////////////////////
// DCVG5KKEYB.C - keyboard input and customization
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcvg5k.free.fr
// Created  : December 2007
// Last updated : 2008-01-09
//
// This file is part of DCVG5K v2.
// 
// DCVG5K v2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// DCVG5K v2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCVG5K v2.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h> 
#include <stdio.h>
#include <string.h>
#ifdef WIN32
 #include <windows.h>
#endif 
#include "dcvg5kglobal.h"
#include "dcvg5kkeyb.h"

//variables globales
char vg5kkeycode[256]; //scancode vg5k en fonction du scancode pc
char vg5kjoycode[256]; //numero bouton joystick en fonction du scancode pc
int lastkeycode;       //keycode derniere touche enfoncee
int lastkeysym;        //keysym derniere touche enfoncee

//variables externes
extern int language;
extern int pausez80;
extern int touche[];
extern int rmask, gmask, bmask, amask;
extern int dialog;             //0 ou n�boite de dialogue affichee
extern SDL_Surface *screen;    //surface d'affichage de l'ecran
extern SDL_Surface *textbox;   //surface d'affichage de texte
extern SDL_Surface *dialogbox; //surface d'affichage dialogbox
extern SDL_Surface *buttonsurfaceup[][LANGUAGE_MAX]; 
extern SDL_Surface *buttonsurfacedown[][LANGUAGE_MAX]; 
extern char *msg[LANGUAGE_MAX][MSG_MAX]; //messages en plusieurs langues
extern button bouton[];
extern const dialogbutton keyboardbutton[];
extern const dialogbutton joystickbutton[];

//fonctions externes
extern void Key(int keycode, int state);
extern void Joy(int joycode, int state);
extern void Info(int i); 
extern void Displayscreen();
extern void Drawtextbox(SDL_Surface *surf, char *string, SDL_Rect rect, int c);

//Emulation clavier : affichage du nom de la derniere touche pressee //////////
void Displaykey()
{
 SDL_Rect rect;
 int i;
 char string[80];
 rect.x = 10; rect.y = 194; rect.w = 140; rect.h = 15;
 sprintf(string, "0x%02x = %s", lastkeycode, SDL_GetKeyName(lastkeysym));    
 Drawtextbox(dialogbox, string, rect, 1); 
 //code et nom de la touche de l'ordinateur emule
 rect.x = 238; rect.y = 194; rect.w = 214;
 rect.h = 15;
 i = vg5kkeycode[lastkeycode & 0xff];
 sprintf(string, "%s : %s", msg[language][15],    
  (i < KEYBOARDKEY_MAX) ? bouton[keyboardbutton[i].n].name : msg[language][18]);
 Drawtextbox(dialogbox, string, rect, 1); 
}           

//Emulation manettes : affichage du nom de la derniere touche pressee /////////
void Displayjoy()
{
 SDL_Rect rect;
 int i;
 char string[80];
 //code et nom de la touche PC
 rect.x = 60; rect.y = 154; rect.w = 140; rect.h = 15;
 sprintf(string, "0x%02x = %s", lastkeycode, SDL_GetKeyName(lastkeysym));    
 Drawtextbox(dialogbox, string, rect, 1); 
 //code et nom de la fonction manette
 rect.x = 230; rect.y = 154; rect.w = 180; rect.h = 15;
 i = vg5kjoycode[lastkeycode & 0xff];
 sprintf(string, "%s : %s", msg[language][35],    
  (i < JOYSTICKKEY_MAX) ? bouton[joystickbutton[i].n].name : msg[language][18]);
 Drawtextbox(dialogbox, string, rect, 1); 
}           

//Cr�ation de la boite de dialogue du clavier ////////////////////////////////
void Clavier()
{
 SDL_Rect rect;
 int i, n;
 char string[256];
 extern void Createdialogbox(int w, int h);
 pausez80 = 1;
 lastkeycode = 0;
 lastkeysym = 0;
 Createdialogbox(466, 284);
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][14], rect, 13);
 //message d'aide
 rect.x = 66; rect.y = 215; rect.w = 370;
 sprintf(string, "%s %s.", msg[language][16], msg[language][15]);
 Drawtextbox(dialogbox, string, rect, 4); 
 rect.y += 15;
 Drawtextbox(dialogbox, msg[language][17], rect, 4); 
 //dessin des boutons
 for(i = 0; i < KEYBOARDBUTTON_MAX; i++)
 {
  n = keyboardbutton[i].n;
  rect.x = keyboardbutton[i].x;
  rect.y = keyboardbutton[i].y;
  SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
 }            
 dialog = 3;     
 Displaykey(0);  //nom de la touche lastkey
 pausez80 = 0;
}     

//Cr�ation de la boite de dialogue des manettes //////////////////////////////
void Manettes()
{
 SDL_Rect rect;
 int i, n;
 char string[256];
 extern void Createdialogbox(int w, int h);
 pausez80 = 1;
 lastkeycode = 0;
 lastkeysym = 0;
 Createdialogbox(454, 244);
 //titre
 rect.x = 10; rect.w = dialogbox->w - 32;
 rect.y = 5; rect.h = 15;
 Drawtextbox(dialogbox, msg[language][34], rect, 13);
 //message d'aide
 rect.x = 60; rect.y = 175; rect.w = 370;
 sprintf(string, "%s %s.", msg[language][16], msg[language][35]);
 Drawtextbox(dialogbox, string, rect, 4); 
 rect.y += 15;
 Drawtextbox(dialogbox, msg[language][17], rect, 4); 
 //boutons et numeros des manettes
 for(i = 0; i < JOYSTICKBUTTON_MAX; i++)
 {
  n = joystickbutton[i].n;     
  rect.x = joystickbutton[i].x;
  rect.y = joystickbutton[i].y;
  SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
 }            
 rect.x = 136; rect.y = 132; rect.w = 20;
 Drawtextbox(dialogbox, "[0]", rect, 4); 
 rect.x += 158;
 Drawtextbox(dialogbox, "[1]", rect, 4); 
 dialog = 4;
 Displayjoy(0); //nom de la fonction lastjoy
 pausez80 = 0;
}     

// Restauration de la configuration par defaut des touches ///////////////////
void Restorekeydefault()
{
 int i, j;
 for(i = 0; i < 256; i++)
 {
  for(j = 0; j < KEYBOARDKEY_MAX; j++) if(pckeycode[j] == i) break;
  vg5kkeycode[i] = j;     
 }
} 

// Restauration de la configuration par defaut des manettes //////////////////
void Restorejoydefault()
{
 int i, j;
 for(i = 0; i < 256; i++)
 {
  for(j = 0; j < JOYSTICKKEY_MAX; j++) if(pcjoycode[j] == i) break;
  vg5kjoycode[i] = j;     
 }
} 

// Sauvegarde de la configuration du clavier /////////////////////////////////
void Savekeyfile()
{
 extern FILE *fpi;
 fseek(fpi, 0x40, SEEK_SET);    
 fwrite(vg5kkeycode, 256, 1, fpi);    
} 

// Sauvegarde de la configuration des manettes ///////////////////////////////
void Savejoyfile()
{
 extern FILE *fpi;
 fseek(fpi, 0x140, SEEK_SET);    
 fwrite(vg5kjoycode, 256, 1, fpi);    
} 

//Traitement des clics boite de dialogue clavier /////////////////////////////
void Keyclick()
{
 SDL_Rect rect;
 int i, j, n, x, y;    
 extern SDL_Rect dialogrect;    //position dialogbox 
 extern int xmouse, ymouse;
 //recherche d'un clic sur un bouton
 for(i = 0; i < KEYBOARDBUTTON_MAX; i++)
 {
  n = keyboardbutton[i].n;     
  x = dialogrect.x + keyboardbutton[i].x;     
  y = dialogrect.y + keyboardbutton[i].y;     
  if(xmouse > x) if(xmouse < (x + bouton[keyboardbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[keyboardbutton[i].n].h))
  break;
 }
 if(i >= KEYBOARDBUTTON_MAX) return;
 //dessin du bouton enfonce puis relache
 rect.x = keyboardbutton[i].x;
 rect.y = keyboardbutton[i].y;
 SDL_BlitSurface(buttonsurfacedown[n][language], NULL, dialogbox, &rect);
 Displayscreen(); SDL_Delay(200);
 SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
 //boutons restauration et sauvegarde
 if(n == 16) {Restorekeydefault(); return;}
 if(n == 17) {Savekeyfile(); return;}
 //suppression de l'ancienne affectation de la touche           
 for(j = 0; j < 256; j++) if(vg5kkeycode[j] == i) vg5kkeycode[j] = KEYBOARDKEY_MAX;
 //ajout de la nouvelle affectation
 if(lastkeycode != 0) vg5kkeycode[lastkeycode & 0xff] = i; 
 Displaykey();
}

//Traitement des clics boite de dialogue manettes ////////////////////////////
void Joyclick()
{
 SDL_Rect rect;
 int i, j, n, x, y;    
 extern SDL_Rect dialogrect;    //position dialogbox 
 extern int xmouse, ymouse;
 //recherche d'un clic bouton
 for(i = 0; i < JOYSTICKBUTTON_MAX; i++)
 {
  n = joystickbutton[i].n;     
  x = dialogrect.x + joystickbutton[i].x;     
  y = dialogrect.y + joystickbutton[i].y;     
  if(xmouse > x) if(xmouse < (x + bouton[joystickbutton[i].n].w))
  if(ymouse > y) if(ymouse < (y + bouton[joystickbutton[i].n].h))
  break;
 }
 if(i >= JOYSTICKBUTTON_MAX) return;
 //dessin du bouton enfonce puis relache
 rect.x = joystickbutton[i].x;
 rect.y = joystickbutton[i].y;
 SDL_BlitSurface(buttonsurfacedown[n][language], NULL, dialogbox, &rect);
 Displayscreen(); SDL_Delay(200);
 SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
 //touches restauration et sauvegarde
 if(n == 16) {Restorejoydefault(); return;}
 if(n == 17) {Savejoyfile(); return;}
 //suppression de l'ancienne affectation de la touche           
 for(j = 0; j < 256; j++) if(vg5kjoycode[j] == i) vg5kjoycode[j] = JOYSTICKKEY_MAX;
 //ajout de la nouvelle affectation
 if(lastkeycode != 0) vg5kjoycode[lastkeycode & 0xff] = i; 
 Displayjoy();
}

// Initialisation du clavier /////////////////////////////////////////////////
void Keyboardinit()
{
 int i, j; 
 extern FILE *fpi;   
 //initialisation des tables par defaut
 for(i = 0; i < 256; i++)
 {
  for(j = 0; j < KEYBOARDKEY_MAX; j++) if(pckeycode[j] == i) break;
  vg5kkeycode[i] = j;     
  for(j = 0; j < JOYSTICKKEY_MAX; j++) if(pcjoycode[j] == i) break;
  vg5kjoycode[i] = j;     
 }
 //recuperation des valeurs de dcvg5k.ini (0x40=touches 0x140=joysticks) 
 fseek(fpi, 0, SEEK_END);
 i = (int)ftell(fpi); //Info(i);  
 if(i < 0x140) return;
 fseek(fpi, 0x40, SEEK_SET); j = fgetc(fpi); fseek(fpi, 0x40, SEEK_SET); 
 if(j == KEYBOARDKEY_MAX) {
  if(fread(vg5kkeycode, 256, 1, fpi) != 1)
   fprintf(stderr, "%s, %d: fread error\n", __FILE__, __LINE__);
 }
 if(i < 0x240) return;
 fseek(fpi, 0x140, SEEK_SET); j = fgetc(fpi); fseek(fpi, 0x140, SEEK_SET); 
 if(j == JOYSTICKKEY_MAX) {
  if(fread(vg5kjoycode, 256, 1, fpi) != 1) {
   fprintf(stderr, "%s, %d: fread error\n", __FILE__, __LINE__);
  }
 }
}

// Key up ////////////////////////////////////////////////////////////////////
void Keyup(int keysym, int scancode)
{
 SDL_Rect rect;       
 int ijoy, ikey, n, keycode;
 //le scancode seul ne permet pas de distinguer le pave numerique
 //keycode = scancode + 0x40 pour le pave numerique
 //keycode = scancode pour toutes les autres touches
 keycode = scancode;
 if(keysym == 0x12c) keycode += 0x40;           //numlock
 if((keysym & 0xff0) == 0x100) keycode += 0x40; //autres touches numpad

 //emulation joystick
 ijoy = vg5kjoycode[keycode & 0xff];
 ikey = vg5kkeycode[keycode & 0xff];

 if(ijoy < JOYSTICKKEY_MAX)
 {
  Joy(ijoy, 0x80);
  if(dialog == 4)
  {
   n = joystickbutton[ijoy].n;
   rect.x = joystickbutton[ijoy].x;
   rect.y = joystickbutton[ijoy].y;
   SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
  }
 }  

 //emulation clavier
 if(ikey < KEYBOARDKEY_MAX)
 {
  Key(ikey, 0x80);
  //dessin de la touche relachee
  if(dialog == 3)
  {
   n = keyboardbutton[ikey].n;
   rect.x = keyboardbutton[ikey].x;
   rect.y = keyboardbutton[ikey].y;
   SDL_BlitSurface(buttonsurfaceup[n][language], NULL, dialogbox, &rect);
  }
 } 
} 

// Key down //////////////////////////////////////////////////////////////////
void Keydown(int keysym, int scancode)
{
 SDL_Rect rect;         
 int ijoy, ikey, n, keycode;
 extern void Initprog();
 extern void exit(int n);
 //le scancode seul ne permet pas de distinguer le pave numerique
 //keycode = scancode + 0x40 pour le pave numerique
 //keycode = scancode pour toutes les autres touches
 keycode = scancode;
 if(keysym == 0x12c) keycode += 0x40;           //numlock
 if((keysym & 0xff0) == 0x100) keycode += 0x40; //autres touches numpad
 lastkeycode = keycode;
 lastkeysym = keysym;

 //essai (infructueux) de detection des deux touches shift simultanees 
 //quand une touche shift est enfonc�e, les mouvements de l'autre
 //ne sont pas detectes, et elle est toujours consideree comme relachee
 //idem pour les touches CTRL droit et gauche
 //difference pour les touches ALT : si la gauche est enfoncee, la droite
 //est detectee, mais pas l'inverse.
 //Remarque : ce comportement est observe dans Windows, mais pas dans Linux 

 //static int flag = 0;
 //Uint8 *keystate;
 //SDL_PumpEvents();
 //keystate = SDL_GetKeyState(NULL);
 //flag = 256 - flag;
 //Keyboard(flag + 2 * keystate[SDLK_LSHIFT] + keystate[SDLK_RSHIFT]); return;
 
 //SDL_Event event;
 //event.key.type = SDL_KEYUP;
 //event.key.state = SDL_RELEASED;
 //Keyboard(SDL_GetModState());
 //SDL_PushEvent(&event);
 //SDL_SetModState(0);
 
 //touches de raccourcis dcvg5k
 if(keysym == SDLK_ESCAPE) {Initprog(); pausez80 = 0; return;}
 if(keysym == SDLK_PAUSE) {pausez80 = 1; return;}            
 
 //la touche AltGr envoie 2 evenements keydown
 //le premier keysym = 0x132 (LCTRL)
 //le deuxieme keysym = 0x133 (RALT)
 //il faut appeler Keyup pour annuler le faux Keydown de LCTRL
 //if(keysym == 0x133) Keyup(0x132);
 if(keycode == 0x38) Keyup(0, 0x1d);
 pausez80 = 0;  //l'appui sur une touche arrete la pause 
 
 //emulation joystick
 ijoy = vg5kjoycode[keycode & 0xff]; //Info(i);
 ikey = vg5kkeycode[keycode & 0xff];
 if(dialog == 3) Displaykey();
 if(dialog == 4) Displayjoy();


 if(ijoy < JOYSTICKKEY_MAX)
 {
  Joy(ijoy, 0x00);
  if(dialog == 4)
  {
   n = joystickbutton[ijoy].n;
   rect.x = joystickbutton[ijoy].x;
   rect.y = joystickbutton[ijoy].y;
   SDL_BlitSurface(buttonsurfacedown[n][language], NULL, dialogbox, &rect);
  }
 } 
 
 //emulation clavier
 if(ikey < KEYBOARDKEY_MAX)
 {
  Key(ikey, 0x00);
  if(dialog == 3)
  {
   n = keyboardbutton[ikey].n;
   rect.x = keyboardbutton[ikey].x;
   rect.y = keyboardbutton[ikey].y;
   SDL_BlitSurface(buttonsurfacedown[n][language], NULL, dialogbox, &rect);
  } 
 }
}     

// Test touches SHIFT, ALT et CTRL gauche et droite //////////////////////////
void Testshiftkey()
{
 //Dans Windows, si une touche majuscule droite ou gauche est enfoncee
 //l'evenement SDL_KEYDOWN n'est pas envoye par l'autre.
 //Il faut donc tester periodiquement leur etat (pour Flipper par exemple).
 //Malheureusement les fonctions SDL_GetModState et SDL_GetKeyState ne
 //marchent pas en cas d'appui simultane sur SHIFT, CTRL et ALT droit et gauche 
 //Cette routine utilise la fonction GetAsyncKeyState de windows pour
 //detecter la veritable position des touches.
 //Elle est appellee environ 20 fois par seconde par la fonction Playsound  
 
 //SDL_GetModState ne marche pas si les 2 touches sont enfoncees
 //int mode = SDL_GetModState();
 //extern int touche[]; //position touches ordinateur emule
 //touche[vg5kkey[SDLK_LSHIFT]] = ((mode & 1)) ? 0x00 : 0x80;     
 //touche[vg5kkey[SDLK_RSHIFT]] = ((mode & 2)) ? 0x00 : 0x80;     
 //sprintf(infos, "ModState=%04x", mode);
 //Printstring(infos, 0, 238, 7);
 //SDL_GetKeyState ne marche pas mieux 
 //Uint8 *keystate = SDL_GetKeyState(NULL);
 //sprintf(infos, "Left=%i    Right=%i",
 //        keystate[SDLK_LSHIFT], keystate[SDLK_RSHIFT]);
 //Printstring(infos, 100, 238, 7);

#ifdef WIN32
 static int lshift, rshift; //position des touches shift   
 //Remarque : VK_LSHIFT et VK_RSHIFT ne marchent pas avec Windows 98se
 //D'apr�s Microsoft : Windows 95/98/Me: Windows 95 does not support
 //the left- and right-distinguishing constants. If you call
 //GetAsyncKeyState with these constants, the return value is zero.
 if(GetAsyncKeyState(VK_LSHIFT) < 0)
   {if(!lshift) {lshift = 1; Keydown(SDLK_LSHIFT, 0x2a);}}                              
   else {if(lshift) {lshift = 0; Keyup(SDLK_LSHIFT, 0x2a);}}
 if(GetAsyncKeyState(VK_RSHIFT) < 0)
   {if(!rshift) {rshift = 1; Keydown(SDLK_RSHIFT, 0x36);}}                              
   else {if(rshift) {rshift = 0; Keyup(SDLK_RSHIFT, 0x36);}}
#endif
} 
