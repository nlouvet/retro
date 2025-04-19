#ifndef __DCMO5OPTIONS__
#define __DCMO5OPTIONS__

#include "dcmo5global.h"

extern FILE *fpi;     // fichier dcmo5.ini
extern int language;  // 0=francais 1=anglais
extern int frequency; // frequence 6809 en kHz
extern button optionbutton[OPTIONBUTTON_MAX];

// Draw option box
void Drawoptionbox();

// Initialisation des valeurs par defaut
void Initdefault();

// Traitement des clics des boutons d'options
void Optionclick();

// Option setting
void Options();

// Option initialization
void Initoptions();

// Option save
void Saveoptions();

#endif