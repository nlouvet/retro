//////////////////////////////////////////////////////////////////////////////
// DCVG5KKEYB.H - VG5000 keyboard definition
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

#ifdef WIN32
//scancode hardware du PC Windows pour chaque touche VG5000
//(+ 0x40 pour les touches du pave numerique pour les distinguer des autres)
int pckeycode[KEYBOARDKEY_MAX]=
{
 0x47, //00 list
 0x0e, //01 reset
 0x2a, //02 leftshift
 0x4b, //03 <
 0x4d, //04 >
 0x50, //05 v
 0x1d, //06 ctrl
 0x52, //07 insc
 0x4f, //08 run
 0x1e, //09 Q
 0x39, //0a espace
 0x3a, //0b shiftlock
 0x36, //0c rightshift
 0x1c, //0d return   
 0x48, //0e ^
 0x10, //0f A
 0x2c, //10 W
 0x2d, //11 X
 0x2e, //12 C
 0x2f, //13 V
 0x30, //14 B
 0x02, //15 1
 0x34, //16 :
 0x11, //17 Z
 0x1f, //18 S
 0x12, //19 E
 0x04, //1a 3
 0x05, //1b 4
 0x06, //1c 5
 0x07, //1d 6
 0x03, //1e 2
 0x33, //1f ;
 0x3b, //20 ./.
 0x2b, //21 x
 0x21, //22 F
 0x22, //23 G
 0x16, //24 U
 0x17, //25 I
 0x18, //26 O
 0x19, //27 P
 0x29, //28
 0x0b, //29 0
 0x35, //2a ]
 0x28, //2b ..
 0x32, //2c ,
 0x08, //2d 7
 0x09, //2e 8 
 0x0a, //2f 9
 0x0c, //30 -
 0x0d, //31 +
 0x13, //32 R
 0x14, //33 T
 0x15, //34 Y
 0x56, //35 <
 0x37, //36 prt
 0x20, //37 D
 0x1a, //38 =
 0x53, //39 effc
 0x31, //3a N
 0x24, //3b J
 0x23, //3c H
 0x25, //3d K
 0x26, //3e L
 0x27  //3f M
};

//scancode hardware du PC Windows pour chaque fonction des manettes VG5000
//(+ 0x40 pour les touches du pave numerique pour les distinguer des autres)
int pcjoycode[JOYSTICKKEY_MAX]=
{
 0x8c, //0 manette 0 HAUT 
 0x91, //1 manette 0 DROITE 
 0x90, //2 manette 0 BAS
 0x8f, //3 manette 0 GAUCHE
 0x5c, //4 manette 0 ACTIONA
 0xff, //5 manette 0 ACTIONB
 0xff, //6 manette 0 ACTIONC
 0xff, //7 manette 0 ACTIOND
 0x75, //8 manette 1 HAUT 
 0x89, //9 manette 1 DROITE
 0x88, //a manette 1 BAS
 0x87, //b manette 1 GAUCHE
 0x8e, //c manette 1 ACTIONA
 0xff, //d manette 1 ACTIONB
 0xff, //e manette 1 ACTIONC
 0xff  //f manette 1 ACTIOND
};

#else

//scancode hardware du PC Linux pour chaque touche VG5000
//(+ 0x40 pour les touches du pave numerique pour les distinguer des autres)
int pckeycode[KEYBOARDKEY_MAX]=
{
 0x63, //00 list
 0x16, //01 reset
 0x32, //02 leftshift
 0x64, //03 <
 0x66, //04 >
 0x68, //05 v
 0x25, //06 ctrl
 0x6a, //07 insc
 0x4f, //08 run
 0x26, //09 Q
 0x41, //0a espace
 0x42, //0b shiftlock
 0x3e, //0c rightshift
 0x24, //0d return   
 0x62, //0e ^
 0x18, //0f A
 0x34, //10 W
 0x35, //11 X
 0x36, //12 C
 0x37, //13 V
 0x38, //14 B
 0x0a, //15 1
 0x3c, //16 :
 0x19, //17 Z
 0x27, //18 S
 0x1a, //19 E
 0x0c, //1a 3
 0x0d, //1b 4
 0x0e, //1c 5
 0x0f, //1d 6
 0x0b, //1e 2
 0x3b, //1f ;
 0x30, //20 ./.
 0x33, //21 x
 0x29, //22 F
 0x2a, //23 G
 0x1e, //24 U
 0x1f, //25 I
 0x20, //26 O
 0x21, //27 P
 0xff, //28
 0x13, //29 0
 0x3d, //2a ]
 0x22, //2b ..
 0x3a, //2c ,
 0x10, //2d 7
 0x11, //2e 8 
 0x12, //2f 9
 0x14, //30 -
 0x15, //31 +
 0x1b, //32 R
 0x1c, //33 T
 0x1d, //34 Y
 0x5e, //35 <
 0x69, //36 prt
 0x28, //37 D
 0xff, //38 =
 0x6b, //39 effc
 0x39, //3a N
 0x2c, //3b J
 0x2b, //3c H
 0x2d, //3d K
 0x2e, //3e L
 0x2f  //3f M
};

//scancode hardware du PC Linux pour chaque fonction des manettes VG5000
//(+ 0x40 pour les touches du pave numerique pour les distinguer des autres)
int pcjoycode[JOYSTICKKEY_MAX]=
{
 0x94, //0 manette 0 HAUT 
 0x99, //1 manette 0 DROITE 
 0x98, //2 manette 0 BAS
 0x97, //3 manette 0 GAUCHE
 0xac, //4 manette 0 ACTIONA
 0xff, //5 manette 0 ACTIONB
 0xff, //6 manette 0 ACTIONC
 0xff, //7 manette 0 ACTIOND
 0xb0, //8 manette 1 HAUT 
 0x91, //9 manette 1 DROITE
 0x90, //a manette 1 BAS
 0x8f, //b manette 1 GAUCHE
 0x96, //c manette 1 ACTIONA
 0xff, //d manette 1 ACTIONB
 0xff, //e manette 1 ACTIONC
 0xff  //f manette 1 ACTIOND
};

#endif
