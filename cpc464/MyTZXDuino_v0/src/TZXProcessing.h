#include <Arduino.h>
#include <TimerOne.h>
#include "SdFat.h"

#define outputPin 3 // data output pin
#define motorPin 4  // motor sens pin

// error codes
#define ERROR_NONE  0           // no error
#define ERROR_OPEN  1           // error while trying to open a file
#define ERROR_FILE  2           // uncorrect file type
#define ERROR_READ  3           // uncorrect file type
#define ERROR_UNKNOWN_ID 4      // unknown ID found in the file
#define ERROR_UNKNOWN_HEADER 5  // unknown HEADER file

void TZXSetup(SdFat *sdp);
bool TZXPlay(const char *filename);
bool TZXLoop();
void TZXStop();

bool TXZIsStopped();
byte TXZGetErrorCode();