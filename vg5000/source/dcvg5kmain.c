//////////////////////////////////////////////////////////////////////////////
// DCVG5KMAIN.C - dcvg5k v11 emulator main program
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcvg5k.free.fr
// Created  : December 2007
// Last updated : 2008-01-11
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
#include "../include/policettf.h"
#include "../include/dcvg5kicon.h"
#include "dcvg5kglobal.h"
#include "dcvg5kmsg.h"

// global variables //////////////////////////////////////////////////////////
const SDL_VideoInfo *videoinfo;
SDL_AudioSpec audio;

extern int pausez80;

// Joysticks initialization //////////////////////////////////////////////////
void Joyinit()
{ 
 //int i = 0;
 //if(joySetCapture(hwndmain, JOYSTICKID1, 0, TRUE)) i += 1; 
 //if(joySetCapture(hwndmain, JOYSTICKID2, 0, TRUE)) i += 2;
 //joySetThreshold(JOYSTICKID1, 2000); //10 trop peu pour Trust Predator
 //joySetThreshold(JOYSTICKID2, 2000); //2000 semble correct
}

// Play sound ////////////////////////////////////////////////////////////////
void Playsound(void *udata, Uint8 *stream, int bufferlength)
{
 int i;
 int icycles; //nombre entier de cycles entre deux echantillons
 extern int sound, frequency;
 extern int Run(int n);
 extern void Testshiftkey();
 icycles = 1814 * frequency / 10000;
 for(i = 0; i < bufferlength; i++)
 {
  if(pausez80) {stream[i] = 128; continue;}
  Run(icycles); stream[i] = sound + 128;
 }
 Testshiftkey(); //contournement bug SDL sur l'appui simultane SHIFT, ALT, CTRL
}  

//Event loop /////////////////////////////////////////////////////////////////
void Eventloop()
{
 SDL_Event event;
 extern int xmouse, ymouse, xmove, ymove;
 extern void Mouseclick(), Dialogmove();
 extern void Resizescreen(int x, int y), Displayscreen();
 extern void Keydown(int keysym, int scancode), Keyup(int keysym, int scancode);

 while(1)
 {
  while(SDL_PollEvent(&event))
  {
   switch(event.type)
   {
    case SDL_VIDEORESIZE:
         Resizescreen(event.resize.w, event.resize.h); break;
    case SDL_MOUSEBUTTONDOWN:
         Mouseclick(); break;
    case SDL_MOUSEBUTTONUP:
         xmove = ymove = 0; break;
    case SDL_MOUSEMOTION:
         xmouse = event.motion.x; ymouse = event.motion.y;
         if(xmove) if(ymove) Dialogmove(); break;	
    case SDL_KEYDOWN:
         Keydown(event.key.keysym.sym, event.key.keysym.scancode); break;
    case SDL_KEYUP:
         Keyup(event.key.keysym.sym, event.key.keysym.scancode); break;
    case SDL_QUIT: return;
   }
  }
  SDL_Delay(20);
 }
}     

/*
// Restaurer la fenetre ///////////////////////////////////////////////////////
void Restorewindow()
{
 //Avec SDL 1.2, il semble impossible
 //- de maximiser ou restaurer la fenetre
 //- de savoir si la fenetre est maximisee ou pas
 //La fenetre initiale de SDL n'etant pas maximisee, l'idee etait d'arreter
 //puis de redemarrer le sous-systeme video pour restaurer la fenetre.
 //Il y a eu un resultat (fenetre correctement restauree) mais trop
 //d'inconvenients : perte du focus, plantages et autres phenomenes inexpliques.
 //Je crois qu'il faut attendre la version 1.3 

 extern int penbutton, xclient, yclient, ystatus; 
 extern void Resizescreen(int x, int y);
  
 //SDL_PauseAudio(1);    //Arrete l'emulation
 //SDL_Delay(200);
 SDL_QuitSubSystem(SDL_INIT_VIDEO);
 SDL_InitSubSystem(SDL_INIT_VIDEO);
 atexit(SDL_Quit);
 SDL_WM_SetIcon(SDL_LoadBMP("dcvg5k.bmp"), NULL);
 SDL_WM_SetCaption(msg[language][0], NULL);            //titre fenetre
 SDL_SetCursor(SDL_CreateCursor(cursor, cursor + 48, 16, 24, 0, 0));
 Resizescreen(xclient, ystatus + yclient);
 penbutton = 0;
 //SDL_PauseAudio(0);    //Lance l'emulation
} 
*/

//Main program ///////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
 SDL_RWops *rw;
 unsigned char cursor[] = {
  128,0,192,0,160,0,144,0,136,0,132,0,130,0,129,0,128,128,128,64,128,32,129,240,
  137,0,152,128,164,128,196,64,130,64,2,32,1,32,1,16,0,176,0,192,0,0,0,0,128,0,
  192,0,224,0,240,0,248,0,252,0,254,0,255,0,255,128,255,192,255,224,255,240,255,
  0,231,128,199,128,131,192,3,192,3,224,1,224,1,240,0,240,0,192,0,0,0,0};
 extern int language;
 extern TTF_Font *mono10;
 extern TTF_Font *vera9;
 extern TTF_Font *vera11;
 extern int xclient, yclient, screencount;
 extern void Initbuttonsurfaces(), Initstatusbar();
 extern void Initoptions(), Saveoptions(), SDL_error(int n);
 extern void Init6809registerpointers(), Initfilenames(), Hardreset();
 extern void Resizescreen(int x, int y), Displayscreen(), Keyboardinit();
 
 //initialisations
 Joyinit();       //Joysticks initialization
 Initoptions();   //Option initialization
 Keyboardinit();  //Keyboard initialization
 Hardreset();     //MO5 initialization

 //initialize SDL ttf
 if(TTF_Init() == -1) SDL_error(1);
 rw = SDL_RWFromMem(veramono, sizeof(veramono));
 mono10 = TTF_OpenFontRW(rw, 0, 10);
 rw = SDL_RWFromMem(verattf, sizeof(verattf));
 vera9 = TTF_OpenFontRW(rw, 0, 9);
 rw = SDL_RWFromMem(verattf, sizeof(verattf));
 vera11 = TTF_OpenFontRW(rw, 0, 11);
 
 //initialize SDL video and keyboard
 if(SDL_Init(SDL_INIT_VIDEO) < 0) SDL_error(2);
 atexit(SDL_Quit);
 rw = SDL_RWFromMem(dcvg5kicon, sizeof(dcvg5kicon));
 SDL_WM_SetIcon(SDL_LoadBMP_RW(rw, 1), NULL);  //icone fenetre
 SDL_WM_SetCaption(msg[language][0], NULL);    //titre fenetre
 SDL_SetCursor(SDL_CreateCursor(cursor, cursor + 48, 16, 24, 0, 0)); //curseur
 SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
 Resizescreen(xclient, yclient + YSTATUS);
 screencount = 0;
 Initbuttonsurfaces(); //Initialisation des surfaces des boutons
 Initstatusbar();      //Initialisation de la surface de la barre de statut  
 Displayscreen();      //Affichage ecran avec la barre de statut

 //initialize SDL audio
 if(SDL_Init(SDL_INIT_AUDIO) < 0) SDL_error(3);
 audio.freq = 22050;
 audio.format = AUDIO_U8;
 audio.channels = 1;
 audio.samples = 1024; 
 audio.callback = Playsound;
 audio.userdata = NULL;
 if(SDL_OpenAudio(&audio, NULL) < 0 ) SDL_error(4);
 SDL_PauseAudio(0);    //Lance l'emulation

 //test events
 Eventloop();
 
 //quit
 pausez80 = 1;
 SDL_PauseAudio(1);
 Saveoptions();
 SDL_Delay(100);
 TTF_Quit();
 SDL_Quit();
 return 0;
}
