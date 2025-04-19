//////////////////////////////////////////////////////////////////////////////
// DCMO5KEYB.C - keyboard input and customization
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcmo5.free.fr
// Created  : December 2007
//
// This file is part of DCMO5 v11.
//
// DCMO5 v11 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DCMO5 v11 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCMO5 v11.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "dcmo5global.h"
#include "dcmo5emulation.h"
#include "dcmo5video.h"
#include "dcmo5dialog.h"
#include "dcmo5pckeycode.h"
#include "dcmo5boutons.h"
#include "dcmo5options.h"
#include "dcmo5main.h"

// variables globales

button mo5key[MO5KEY_MAX] = {
    {"N", 222, 114, 24, 24, 0},        // 0x00
    {"EFF", 418, 114, 24, 24, 1},      // 0x01
    {"J", 234, 86, 24, 24, 2},         // 0x02
    {"H", 206, 86, 24, 24, 3},         // 0x03
    {"U", 222, 58, 24, 24, 4},         // 0x04
    {"Y", 194, 58, 24, 24, 5},         // 0x05
    {"7 '", 206, 30, 24, 24, 6},       // 0x06
    {"6 &", 178, 30, 24, 24, 7},       // 0x07
    {" , <", 250, 114, 24, 24, 8},     // 0x08
    {"INS", 390, 114, 24, 24, 9},      // 0x09
    {"K", 262, 86, 24, 24, 10},        // 0x0a
    {"G", 178, 86, 24, 24, 11},        // 0x0b
    {"I", 250, 58, 24, 24, 12},        // 0x0c
    {"T", 166, 58, 24, 24, 13},        // 0x0d
    {"8 (", 234, 30, 24, 24, 14},      // 0x0e
    {"5 %", 150, 30, 24, 24, 15},      // 0x0f
    {" . >", 278, 114, 24, 24, 16},    // 0x10
    {"[retour]", 38, 86, 24, 24, 17},  // 0x11
    {"L", 290, 86, 24, 24, 18},        // 0x12
    {"F", 150, 86, 24, 24, 19},        // 0x13
    {"O", 278, 58, 24, 24, 20},        // 0x14
    {"R", 138, 58, 24, 24, 21},        // 0x15
    {"9 )", 262, 30, 24, 24, 22},      // 0x16
    {"4 $", 122, 30, 24, 24, 23},      // 0x17
    {"@ ^", 306, 114, 24, 24, 24},     // 0x18
    {"[droite]", 418, 58, 24, 24, 25}, // 0x19
    {"M", 318, 86, 24, 24, 26},        // 0x1a
    {"D", 122, 86, 24, 24, 27},        // 0x1b
    {"P", 306, 58, 24, 24, 28},        // 0x1c
    {"E", 110, 58, 24, 24, 29},        // 0x1d
    {"0 `", 290, 30, 24, 24, 30},      // 0x1e
    {"3 #", 94, 30, 24, 24, 31},       // 0x1f
    {"ESPACE", 150, 142, 108, 24, 32}, // 0x20
    {"[bas]", 402, 86, 24, 24, 33},    // 0x21
    {"B", 194, 114, 24, 24, 34},       // 0x22
    {"S", 94, 86, 24, 24, 35},         // 0x23
    {" / ?", 334, 58, 24, 24, 36},     // 0x24
    {"Z", 82, 58, 24, 24, 37},         // 0x25
    {" - =", 318, 30, 24, 24, 38},     // 0x26
    {"2 \"", 66, 30, 24, 24, 38},      // 0x27
    {"X", 110, 114, 24, 24, 40},       // 0x28
    {"[gauche]", 390, 58, 24, 24, 41}, // 0x29
    {"V", 166, 114, 24, 24, 42},       // 0x2a
    {"Q", 66, 86, 24, 24, 43},         // 0x2b
    {" * :", 362, 58, 24, 24, 44},     // 0x2c
    {"A", 54, 58, 24, 24, 45},         // 0x2d
    {" + ;", 346, 30, 24, 24, 46},     // 0x2e
    {"1 !", 38, 30, 24, 24, 47},       // 0x2f
    {"W", 82, 114, 24, 24, 48},        // 0x30
    {"[haut]", 402, 30, 24, 24, 49},   // 0x31
    {"C", 138, 114, 24, 24, 50},       // 0x32
    {"RAZ", 10, 86, 24, 24, 51},       // 0x33
    {"ENT", 346, 86, 40, 24, 52},      // 0x34
    {"CNT", 10, 58, 24, 24, 53},       // 0x35
    {"ACC", 374, 30, 24, 24, 54},      // 0x36
    {"STP", 10, 30, 24, 24, 55},       // 0x37
    {"SHIFT", 10, 114, 52, 24, 56},    // 0x38
    {"BASIC", 334, 114, 52, 24, 57}    // 0x39
};

button joykey[JOYKEY_MAX] = {
    {"[0] haut", 135, 30, 0, 0, 58},    // 0x00
    {"[0] bas", 135, 80, 0, 0, 61},     // 0x01
    {"[0] gauche", 110, 55, 0, 0, 59},  // 0x02
    {"[0] droite", 160, 55, 0, 0, 60},  // 0x03
    {"[1] haut", 293, 30, 0, 0, 58},    // 0x04
    {"[1] bas", 293, 80, 0, 0, 61},     // 0x05
    {"[1] gauche", 268, 55, 0, 0, 59},  // 0x06
    {"[1] droite", 318, 55, 0, 0, 60},  // 0x07
    {"[0] action", 123, 112, 0, 0, 62}, // 0x08
    {"[1] action", 281, 112, 0, 0, 62}  // 0x09
};

button keyboardbutton[KEYBOARDBUTTON_MAX] = {
    {"[defaut]", 10, 209, 0, 0, 68}, // 0x00
    {"[sauver]", 228, 209, 0, 0, 68} // 0x01
};

button joystickbutton[JOYSTICKBUTTON_MAX] = {
    {"[defaut]", 10, 209, 0, 0, 68}, // 0x00
    {"[sauver]", 228, 209, 0, 0, 68} // 0x01
};


char mo5keycode[256]; // scancode mo5 en fonction du scancode pc
char mo5joycode[256]; // numero bouton joystick en fonction du scancode pc
int lastkeycode;      // keycode derniere touche enfoncee
int lastkeysym;       // keysym derniere touche enfoncee

// Emulation clavier : affichage du nom de la derniere touche pressee //////////
void Displaykey() {
    SDL_Rect rect;
    int i;
    char string[80];

    // code et nom de la touche PC
    rect.x = 10;
    rect.y = 154;
    rect.w = 140;
    rect.h = 15;
    sprintf(string, "0x%02x = %s", lastkeycode, SDL_GetKeyName(lastkeysym));
    Drawtextbox(dialogbox, string, rect, 0);
    // code et nom de la touche MO5
    rect.x = 275;
    rect.y = 154;
    rect.w = 175;
    rect.h = 15;
    i = mo5keycode[lastkeycode & 0xff];
    sprintf(string, "%s : ", msg[language][15]);
    strcat(string, (i < MO5KEY_MAX) ? mo5key[i].name : msg[language][18]);
    Drawtextbox(dialogbox, string, rect, 0);
}

// Emulation manettes : affichage du nom de la derniere touche pressee /////////
void Displayjoy() {
    SDL_Rect rect;
    int i;
    char string[80];

    // code et nom de la touche PC
    rect.x = 80;
    rect.y = 154;
    rect.w = 140;
    rect.h = 15;
    sprintf(string, "0x%02x = %s", lastkeycode, SDL_GetKeyName(lastkeysym));
    Drawtextbox(dialogbox, string, rect, 0);
    // code et nom de la fonction manette
    rect.x = 230;
    rect.y = 154;
    rect.w = 180;
    rect.h = 15;
    i = mo5joycode[lastkeycode & 0xff];
    sprintf(string, "%s : ", msg[language][35]);
    strcat(string, (i < JOYKEY_MAX) ? joykey[i].name : msg[language][18]);
    Drawtextbox(dialogbox, string, rect, 0);
}

// Création de la boite de dialogue du clavier ////////////////////////////////
void Clavier() {
    SDL_Rect rect;
    int i, n;
    char string[256];

    pause6809 = 1;
    lastkeycode = 0;
    lastkeysym = 0;

    Createdialogbox(454, 244);
    // titre
    rect.x = 10;
    rect.w = dialogbox->w - 32;
    rect.y = 5;
    rect.h = 15;
    Drawtextbox(dialogbox, msg[language][14], rect, 13);
    // message d'aide
    rect.x = 80;
    rect.y = 175;
    rect.w = 370;
    sprintf(string, "%s %s.", msg[language][16], msg[language][15]);
    Drawtextbox(dialogbox, string, rect, 0);
    rect.y += 15;
    Drawtextbox(dialogbox, msg[language][17], rect, 0);
    // touches du clavier
    for (i = 0; i < MO5KEY_MAX; i++) {
        rect.x = mo5key[i].x;
        rect.y = mo5key[i].y;
        SDL_BlitSurface(buttonsurface[i][0], NULL, dialogbox, &rect);
    }
    // boutons
    for (i = 0; i < KEYBOARDBUTTON_MAX; i++) {
        n = keyboardbutton[i].n;
        rect.x = keyboardbutton[i].x;
        rect.y = keyboardbutton[i].y;
        SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
        Buttontext(i, rect.x, rect.y);
    }
    dialog = 3;
    Displaykey(0); // nom de la touche lastkey
    pause6809 = 0;
}

// Cr�aétion de la boite de dialogue des manettes //////////////////////////////
void Manettes() {
    SDL_Rect rect;
    int i, n;
    char string[256];

    pause6809 = 1;
    lastkeycode = 0;
    lastkeysym = 0;

    Createdialogbox(454, 244);
    // titre
    rect.x = 10;
    rect.w = dialogbox->w - 32;
    rect.y = 5;
    rect.h = 15;
    Drawtextbox(dialogbox, msg[language][34], rect, 13);
    // message d'aide
    rect.x = 80;
    rect.y = 175;
    rect.w = 370;
    sprintf(string, "%s %s.", msg[language][16], msg[language][35]);
    Drawtextbox(dialogbox, string, rect, 0);
    rect.y += 15;
    Drawtextbox(dialogbox, msg[language][17], rect, 0);
    // boutons et numeros des manettes
    for (i = 0; i < JOYKEY_MAX; i++) {
        n = joykey[i].n;
        rect.x = joykey[i].x;
        rect.y = joykey[i].y;
        SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
    }
    rect.x = 136;
    rect.y = 132;
    rect.w = 20;
    Drawtextbox(dialogbox, "[0]", rect, 0);
    rect.x += 158;
    Drawtextbox(dialogbox, "[1]", rect, 0);
    // boutons d'action
    for (i = 0; i < JOYSTICKBUTTON_MAX; i++) {
        n = joystickbutton[i].n;
        rect.x = joystickbutton[i].x;
        rect.y = joystickbutton[i].y;
        SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
        Buttontext(i, rect.x, rect.y);
    }
    dialog = 4;
    Displayjoy(0); // nom de la fonction lastjoy
    pause6809 = 0;
}

// Restauration de la configuration par defaut des touches ///////////////////
void Restorekeydefault() {
    SDL_Rect rect;
    int i, j, n;
    n = keyboardbutton[0].n;
    rect.x = keyboardbutton[0].x;
    rect.y = keyboardbutton[0].y;
    // dessin du bouton enfonce
    SDL_BlitSurface(buttonsurface[n][1], NULL, dialogbox, &rect);
    Buttontext(0, rect.x, rect.y);
    Displayscreen();
    // traitement
    for (i = 0; i < 256; i++) {
        for (j = 0; j < MO5KEY_MAX; j++)
            if (pckeycode[j] == i)
                break;
        mo5keycode[i] = j;
    }
    SDL_Delay(200);
    // dessin du bouton relache
    SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
    Buttontext(0, rect.x, rect.y);
    Displayscreen();
}

// Restauration de la configuration par defaut des manettes //////////////////
void Restorejoydefault() {
    SDL_Rect rect;
    int i, j, n;
    n = joystickbutton[0].n;
    rect.x = joystickbutton[0].x;
    rect.y = joystickbutton[0].y;
    // dessin du bouton enfonce
    SDL_BlitSurface(buttonsurface[n][1], NULL, dialogbox, &rect);
    Buttontext(0, rect.x, rect.y);
    Displayscreen();
    // traitement
    for (i = 0; i < 256; i++) {
        for (j = 0; j < JOYKEY_MAX; j++)
            if (pcjoycode[j] == i)
                break;
        mo5joycode[i] = j;
    }
    SDL_Delay(200);
    // dessin du bouton relache
    SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
    Buttontext(0, rect.x, rect.y);
    Displayscreen();
}

// Sauvegarde de la configuration du clavier /////////////////////////////////
void Savekeyfile() {
    SDL_Rect rect;
    int n;

    n = keyboardbutton[1].n;
    rect.x = keyboardbutton[1].x;
    rect.y = keyboardbutton[1].y;
    // dessin du bouton enfonce
    SDL_BlitSurface(buttonsurface[n][1], NULL, dialogbox, &rect);
    Buttontext(1, rect.x, rect.y);
    Displayscreen();
    // traitement
    fseek(fpi, 0x40, SEEK_SET);
    fwrite(mo5keycode, 256, 1, fpi);
    SDL_Delay(200);
    // dessin du bouton relache
    SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
    Buttontext(1, rect.x, rect.y);
    Displayscreen();
}

// Sauvegarde de la configuration des manettes ///////////////////////////////
void Savejoyfile() {
    SDL_Rect rect;
    int n;

    n = joystickbutton[1].n;
    rect.x = joystickbutton[1].x;
    rect.y = joystickbutton[1].y;
    // dessin du bouton enfonce
    SDL_BlitSurface(buttonsurface[n][1], NULL, dialogbox, &rect);
    Buttontext(1, rect.x, rect.y);
    Displayscreen();
    // traitement
    fseek(fpi, 0x140, SEEK_SET);
    fwrite(mo5joycode, 256, 1, fpi);
    SDL_Delay(200);
    // dessin du bouton relache
    SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
    Buttontext(1, rect.x, rect.y);
    Displayscreen();
}

// Traitement des clics boite de dialogue clavier /////////////////////////////
void Keyclick() {
    SDL_Rect rect;
    int i, j, n, x, y;

    // recherche d'un clic bouton
    for (i = 0; i < KEYBOARDBUTTON_MAX; i++) {
        n = keyboardbutton[i].n;
        x = dialogrect.x + keyboardbutton[i].x;
        y = dialogrect.y + keyboardbutton[i].y;
        if (xmouse > x)
            if (xmouse < (x + buttonsurface[n][0]->w))
                if (ymouse > y)
                    if (ymouse < (y + buttonsurface[n][0]->h))
                        break;
    }
    if (i == 0) {
        Restorekeydefault();
        return;
    }
    if (i == 1) {
        Savekeyfile();
        return;
    }
    // recherche d'un clic touche
    for (i = 0; i < MO5KEY_MAX; i++) {
        x = dialogrect.x + mo5key[i].x;
        y = dialogrect.y + mo5key[i].y;
        if (xmouse > x)
            if (xmouse < (x + buttonsurface[i][0]->w))
                if (ymouse > y)
                    if (ymouse < (y + buttonsurface[i][0]->h))
                        break;
    }
    if (i >= MO5KEY_MAX)
        return;
    // dessin du bouton enfonce
    rect.x = mo5key[i].x;
    rect.y = mo5key[i].y;
    SDL_BlitSurface(buttonsurface[i][1], NULL, dialogbox, &rect);
    Displayscreen();
    // suppression de l'ancienne affectation de la touche
    for (j = 0; j < 256; j++)
        if (mo5keycode[j] == i)
            mo5keycode[j] = MO5KEY_MAX;
    // ajout de la nouvelle affectation
    if (lastkeycode != 0)
        mo5keycode[lastkeycode & 0xff] = i;
    SDL_Delay(200);
    // affichage de la touche relachee et du nouveau nom
    SDL_BlitSurface(buttonsurface[i][0], NULL, dialogbox, &rect);
    Displaykey();
}

// Traitement des clics boite de dialogue manettes ////////////////////////////
void Joyclick() {
    SDL_Rect rect;
    int i, j, n, x, y;

    // recherche d'un clic bouton
    for (i = 0; i < JOYSTICKBUTTON_MAX; i++) {
        n = joystickbutton[i].n;
        x = dialogrect.x + joystickbutton[i].x;
        y = dialogrect.y + joystickbutton[i].y;
        if (xmouse > x)
            if (xmouse < (x + buttonsurface[n][0]->w))
                if (ymouse > y)
                    if (ymouse < (y + buttonsurface[n][0]->h))
                        break;
    }
    if (i == 0) {
        Restorejoydefault();
        return;
    }
    if (i == 1) {
        Savejoyfile();
        return;
    }
    // recherche d'un clic manette
    for (i = 0; i < JOYKEY_MAX; i++) {
        n = joykey[i].n;
        x = dialogrect.x + joykey[i].x;
        y = dialogrect.y + joykey[i].y;
        if (xmouse > x)
            if (xmouse < (x + buttonsurface[n][0]->w))
                if (ymouse > y)
                    if (ymouse < (y + buttonsurface[n][0]->h))
                        break;
    }
    if (i >= JOYKEY_MAX)
        return;
    // dessin du bouton enfonce
    n = joykey[i].n;
    rect.x = joykey[i].x;
    rect.y = joykey[i].y;
    SDL_BlitSurface(buttonsurface[n][1], NULL, dialogbox, &rect);
    Displayscreen();
    // suppression de l'ancienne affectation de la touche
    for (j = 0; j < 256; j++)
        if (mo5joycode[j] == i)
            mo5joycode[j] = JOYKEY_MAX;
    // ajout de la nouvelle affectation
    if (lastkeycode != 0)
        mo5joycode[lastkeycode & 0xff] = i;
    SDL_Delay(200);
    // affichage de la touche relachee et du nouveau nom
    SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
    Displayjoy();
}

// Initialisation du clavier /////////////////////////////////////////////////
void Keyboardinit() {
    int i, j;

    // initialisation des tables par defaut
    for (i = 0; i < 256; i++) {
        for (j = 0; j < MO5KEY_MAX; j++)
            if (pckeycode[j] == i)
                break;
        mo5keycode[i] = j;
        for (j = 0; j < JOYKEY_MAX; j++)
            if (pcjoycode[j] == i)
                break;
        mo5joycode[i] = j;
    }
    // recuperation des valeurs de dcmo5.ini (0x40=touches 0x140=joysticks)
    fseek(fpi, 0, SEEK_END);
    i = (int)ftell(fpi); // Info(i);
    if (i < 0x140)
        return;
    fseek(fpi, 0x40, SEEK_SET);
    j = fgetc(fpi);
    fseek(fpi, 0x40, SEEK_SET);
    if (j == MO5KEY_MAX) {
        if(fread(mo5keycode, 256, 1, fpi) != 1) goto Keyboardinit_error;
    }
    if (i < 0x240)
        return;
    fseek(fpi, 0x140, SEEK_SET);
    j = fgetc(fpi);
    fseek(fpi, 0x140, SEEK_SET);
    if (j == JOYKEY_MAX) {
        if(fread(mo5joycode, 256, 1, fpi) != 1) goto Keyboardinit_error;
    }
Keyboardinit_error:
    fprintf(stderr, "Keyboardinit: error while reading the initialization file...\n");
}

// Key up ////////////////////////////////////////////////////////////////////
void Keyup(int keysym, int scancode) {
    SDL_Rect rect;
    int i, n, keycode;

    // le scancode seul ne permet pas de distinguer le pave numerique
    // keycode = scancode + 0x40 pour le pave numerique
    // keycode = scancode pour toutes les autres touches
    keycode = scancode;
    if (keysym == 0x12c)
        keycode += 0x40; // numlock
    if ((keysym & 0xff0) == 0x100)
        keycode += 0x40; // autres touches numpad

    // emulation joystick
    i = mo5joycode[keycode & 0xff];
    if (i < JOYKEY_MAX) {
        Joysemul(i, 0x80);
        if (dialog == 4) {
            n = joykey[i].n;
            rect.x = joykey[i].x;
            rect.y = joykey[i].y;
            SDL_BlitSurface(buttonsurface[n][0], NULL, dialogbox, &rect);
        }
    }

    // emulation clavier
    i = mo5keycode[keycode & 0xff];
    if (i < MO5KEY_MAX) {
        touche[i] = 0x80;
        // dessin de la touche relachee
        if (dialog == 3) {
            rect.x = mo5key[i].x;
            rect.y = mo5key[i].y;
            SDL_BlitSurface(buttonsurface[i][0], NULL, dialogbox, &rect);
        }
    }
}

// Key down //////////////////////////////////////////////////////////////////
void Keydown(int keysym, int scancode) {
    SDL_Rect rect;
    int i, n, keycode;

#ifdef DEBUG_SCANCODE
    // to devise keyboard mapping
    printf("scan: %#04x - symb: %#04x \n", scancode, keysym);
#endif
    // le scancode seul ne permet pas de distinguer le pave numerique
    // keycode = scancode + 0x40 pour le pave numerique
    // keycode = scancode pour toutes les autres touches
    keycode = scancode;
    if (keysym == 0x12c)
        keycode += 0x40; // numlock
    if ((keysym & 0xff0) == 0x100)
        keycode += 0x40; // autres touches numpad
    lastkeycode = keycode;
    lastkeysym = keysym;

    // essai (infructueux) de detection des deux touches shift simultanees
    // quand une touche shift est enfonc�e, les mouvements de l'autre
    // ne sont pas detectes, et elle est toujours consideree comme relachee
    // idem pour les touches CTRL droit et gauche
    // difference pour les touches ALT : si la gauche est enfoncee, la droite
    // est detectee, mais pas l'inverse.
    // Remarque : ce comportement est observe dans Windows, mais pas dans Linux

    // static int flag = 0;
    // Uint8 *keystate;
    // SDL_PumpEvents();
    // keystate = SDL_GetKeyState(NULL);
    // flag = 256 - flag;
    // Keyboard(flag + 2 * keystate[SDLK_LSHIFT] + keystate[SDLK_RSHIFT]);
    // return;

    // SDL_Event event;
    // event.key.type = SDL_KEYUP;
    // event.key.state = SDL_RELEASED;
    // Keyboard(SDL_GetModState());
    // SDL_PushEvent(&event);
    // SDL_SetModState(0);

    // touches de raccourcis dcmo5
    if (keysym == SDLK_ESCAPE) {
        Initprog();
        pause6809 = 0;
        return;
    }
    if (keysym == SDLK_PAUSE) {
        pause6809 = 1;
        return;
    }
    // if(keycode == 0x01) {Initprog(); pause6809 = 0; return;}//touche ESC
    // enfoncee if(keycode == 0x45) {pause6809 = 1; return;}            //touche
    // PAUSE enfoncee

    // la touche AltGr envoie 2 evenements keydown
    // le premier keysym = 0x132 (LCTRL)
    // le deuxieme keysym = 0x133 (RALT)
    // il faut appeler Keyup pour annuler le faux Keydown de LCTRL
    // if(keysym == 0x133) Keyup(0x132);
    if (keycode == 0x38)
        Keyup(0, 0x1d);
    pause6809 = 0; // l'appui sur une touche arrete la pause

    // emulation joystick
    i = mo5joycode[keycode & 0xff]; // Info(i);
    if (i < JOYKEY_MAX)
        Joysemul(i, 0x00);
    if (dialog == 4) {
        Displayjoy();
        if (i < JOYKEY_MAX) {
            n = joykey[i].n;
            rect.x = joykey[i].x;
            rect.y = joykey[i].y;
            SDL_BlitSurface(buttonsurface[n][1], NULL, dialogbox, &rect);
        }
    }

    // emulation clavier
    i = mo5keycode[keycode & 0xff];
    if (i < MO5KEY_MAX)
        touche[i] = 0x00;
    if (dialog == 3) {
        Displaykey();
        if (i < MO5KEY_MAX) {
            rect.x = mo5key[i].x;
            rect.y = mo5key[i].y;
            SDL_BlitSurface(buttonsurface[i][1], NULL, dialogbox, &rect);
        }
    }
}

// Test touches SHIFT, ALT et CTRL gauche et droite //////////////////////////
void Testshiftkey() {
    // Dans Windows, si une touche majuscule droite ou gauche est enfoncee
    // l'evenement SDL_KEYDOWN n'est pas envoye par l'autre.
    // Il faut donc tester periodiquement leur etat (pour Flipper par exemple).
    // Malheureusement les fonctions SDL_GetModState et SDL_GetKeyState ne
    // marchent pas en cas d'appui simultane sur SHIFT, CTRL et ALT droit et
    // gauche Cette routine utilise la fonction GetAsyncKeyState de windows pour
    // detecter la veritable position des touches.
    // Elle est appellee environ 20 fois par seconde par la fonction Playsound

    // SDL_GetModState ne marche pas si les 2 touches sont enfoncees
    // int mode = SDL_GetModState();
    // int touche[]; //position touches MO5
    // touche[mo5key[SDLK_LSHIFT]] = ((mode & 1)) ? 0x00 : 0x80;
    // touche[mo5key[SDLK_RSHIFT]] = ((mode & 2)) ? 0x00 : 0x80;
    // sprintf(infos, "ModState=%04x", mode);
    // Printstring(infos, 0, 238, 7);
    // SDL_GetKeyState ne marche pas mieux
    // Uint8 *keystate = SDL_GetKeyState(NULL);
    // sprintf(infos, "Left=%i    Right=%i",
    //        keystate[SDLK_LSHIFT], keystate[SDLK_RSHIFT]);
    // Printstring(infos, 100, 238, 7);

#ifdef WIN32
    static int lshift, rshift; // position des touches shift
    // Remarque : VK_LSHIFT et VK_RSHIFT ne marchent pas avec Windows 98se
    // D'apr�s Microsoft : Windows 95/98/Me: Windows 95 does not support
    // the left- and right-distinguishing constants. If you call
    // GetAsyncKeyState with these constants, the return value is zero.
    if (GetAsyncKeyState(VK_LSHIFT) < 0) {
        if (!lshift) {
            lshift = 1;
            Keydown(SDLK_LSHIFT, 0x2a);
        }
    } else {
        if (lshift) {
            lshift = 0;
            Keyup(SDLK_LSHIFT, 0x2a);
        }
    }
    if (GetAsyncKeyState(VK_RSHIFT) < 0) {
        if (!rshift) {
            rshift = 1;
            Keydown(SDLK_RSHIFT, 0x36);
        }
    } else {
        if (rshift) {
            rshift = 0;
            Keyup(SDLK_RSHIFT, 0x36);
        }
    }
#endif
}
