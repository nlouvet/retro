#ifndef __DCMO5OPTIONS__
#define __DCMO5OPTIONS__

#include "dcmo5global.h"

extern FILE *fpi;     // fichier dcmo5.ini
extern int language;  // 0=francais 1=anglais
extern int frequency; // frequence 6809 en kHz
extern button optionbutton[OPTIONBUTTON_MAX];

void Drawoptionbox();
void Initdefault();
void Optionclick();
void Options();
void Initoptions();
void Saveoptions();

#endif