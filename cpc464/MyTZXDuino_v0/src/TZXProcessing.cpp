#include "TZXProcessing.h"

// define constants TRUE and FALSE: in the code, most boolean values are
// stored as bytes, and we will use TRUE and FALSE to set these variables

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

const byte TZXTape[7] = {'Z','X','T','a','p','e','!'};
const byte TAPcheck[7] = {'T','A','P','t','a','p','.'};

// TZX block list - uncomment as supported
#define ID10                0x10    // Standard speed data block
#define ID11                0x11    // Turbo speed data block
#define ID12                0x12    // Pure tone
#define ID13                0x13    // Sequence of pulses of various lengths
#define ID14                0x14    // Pure data block
#define ID15                0x15    // Direct recording block -- TBD - curious to load OTLA files using direct recording (22KHz)
//#define ID18              0x18    // CSW recording block
//#define ID19              0x19    // Generalized data block
#define ID20                0x20    // Pause (silence) ot 'Stop the tape' command
#define ID21                0x21    // Group start
#define ID22                0x22    // Group end
#define ID23                0x23    // Jump to block
#define ID24                0x24    // Loop start
#define ID25                0x25    // Loop end
#define ID26                0x26    // Call sequence
#define ID27                0x27    // Return from sequence
#define ID28                0x28    // Select block
#define ID2A                0x2A    // Stop the tape is in 48K mode
#define ID2B                0x2B    // Set signal level
#define ID30                0x30    // Text description
#define ID31                0x31    // Message block
#define ID32                0x32    // Archive info
#define ID33                0x33    // Hardware type
#define ID35                0x35    // Custom info block
#define IDPAUSE				      0x59    // Custom Pause processing
#define ID5A                0x5A    // Glue block (90 dec, ASCII Letter 'Z')
#define TAP                 0xFE    // Tap File Mode
#define IDEOF               0xFF    // End of file

// TZX File Tasks
#define GETFILEHEADER         0
#define GETID                 1
#define PROCESSID             2

// TZX ID Tasks
#define READPARAM             0
#define PILOT                 1
#define SYNC1                 2
#define SYNC2                 3
#define DATA                  4
#define PAUSE                 5

// buffer size
#define BUFFSIZE              64

// spectrum Standards
#define PILOTLENGTH           619
#define SYNCFIRST             191
#define SYNCSECOND            210
#define ZEROPULSE             244
#define ONEPULSE              489
#define PILOTNUMBERL          8063
#define PILOTNUMBERH          3223
#define PAUSELENGTH           1000   

// keep track of which ID, Task, and Block Task we're dealing with
static byte currentID = 0;
static byte currentTask = 0;
static byte currentBlockTask = 0;
static byte endOfFile = FALSE;

// temporarily store for a pulse period before loading it into the buffer.
static word currentPeriod = 1;

// ISR variables
static volatile byte pos = 0;                 // index
static volatile word wbuffer[BUFFSIZE+1][2];
static volatile byte morebuff = HIGH;         // HIGH/LOW
static volatile byte workingBuffer = 0;       // 0/1
static volatile byte isStopped = TRUE;        // TRUE/FALSE
static volatile byte isMotorPaused = TRUE;    // TRUE/FALSE
static volatile byte pinState = LOW;          // HIGH/LOW
static volatile byte isPauseBlock = FALSE;    // TRUE/FALSE
static volatile byte wasPauseBlock = FALSE;   // TRUE/FALSE

//Main Variables
static byte btemppos = 0;                     // index
static byte copybuff = LOW;                   // HIGH/LOW
static unsigned long bytesRead = 0;           // index/counter
static unsigned long bytesToRead = 0;         // index/counter
static word pilotPulses = 0;
static word pilotLength = 0;
static word sync1Length = 0;
static word sync2Length = 0;
static word zeroPulse = 0;
static word onePulse = 0;
static word TstatesperSample = 0;
static byte usedBitsInLastByte = 8;
static word loopCount = 0;
static byte seqPulses = 0;

static unsigned long loopStart = 0;
static word pauseLength = 0;
static word temppause = 0;
static byte outByte = 0;
static word outWord = 0;
static unsigned long outLong = 0;
static byte count = 128;
static volatile byte currentBit = 0;
static volatile byte currentByte = 0;
static volatile byte currentChar = 0;
static byte pass = 0;

static SdFat *sd_ptr;
static File file;

// =======================================================================

static byte errorCode = ERROR_NONE;

byte TXZGetErrorCode() {
  return errorCode;
}

// =======================================================================

// unsigned int makeWord(unsigned char h, unsigned char l) { return (h << 8) | l; }
// uint16_t makeWord(byte h, byte l);
// #define word(...) makeWord(__VA_ARGS__)

// =======================================================================

void printtext_(const char* text) {
  Serial.println(text);
}

void clearBuffer() {
  for(int i=0;i<=BUFFSIZE;i++) {
    wbuffer[i][0]=0;
    wbuffer[i][1]=0;
  } 
}

word TickToUs(word ticks) {
  return (word) ((((float) ticks)/3.5)+0.5);
}

// =======================================================================

void wave();

void TZXSetup(SdFat *sdp) {
    pinMode(outputPin, OUTPUT);               // set output pin
    pinMode(motorPin, INPUT_PULLUP);          // motor sense pin
    digitalWrite(outputPin, LOW);             // start output LOW
    isStopped = TRUE;
    isMotorPaused = TRUE;
    pinState = LOW;
    errorCode = ERROR_NONE;
    Timer1.initialize(100000);                // 100ms pause prevents anything bad happening before we're ready
    Timer1.attachInterrupt(wave);
    Timer1.stop();                            // stop the timer until we're ready
    sd_ptr = sdp;
}

// =======================================================================

bool checkForTap(const char *filename);

bool TZXPlay(const char *filename) {
  Timer1.stop();                               //Stop timer interrupt
  file = sd_ptr->open(filename);
  if(!file) {                                 // open file and check for errors
    errorCode = ERROR_OPEN;
    return false;
  }
  bytesRead=0;                                // start of file
  currentTask = GETFILEHEADER;                  // first task: search for header
  if(checkForTap(filename)) {                 // check for Tap File.  As these have no header we can skip straight to playing data
    currentTask = PROCESSID;
    currentID = TAP;
  }
  currentBlockTask = READPARAM;               // first block task is to read in parameters
  clearBuffer();
  isStopped = FALSE;                          // playing can start as soon as requested
  isMotorPaused = TRUE;                       // assume the motor is paused for now
  pinState = LOW;                             // always Start on a LOW output for simplicity
  count = 255;                                // end of file buffer flush
  endOfFile = FALSE;
  errorCode = ERROR_NONE;
  digitalWrite(outputPin, pinState);
  Timer1.setPeriod(1000);                     //set 1ms wait at start of a file.
  return true;
}

bool checkForTap(const char *filename) {
  //Check for TAP file extensions as these have no header
  const char tapext[] = ".tap";
  int8_t i, j, len = 0;

  while(filename[len] != '\0') len++;
  if(len < 4) return false;
  for(i = 0, j = len-4; j < len; i++, j++)
    if(tapext[i] != filename[j]) break;
  return j == len;
}

// =======================================================================

void TZXStop() {
  Timer1.stop(); // stop timer
  isStopped = TRUE;
  file.close(); // close file
  Serial.println("Stop reading file.");
  bytesRead = 0; // reset read bytes counter
}

bool TXZIsStopped() {
  return isStopped == TRUE;
}

// =======================================================================

bool TZXProcess();

bool TZXLoop() {
  // nothing to do if the player is stopped or is on error
  if(isStopped || errorCode != ERROR_NONE) return false;
  // pause interrupts to prevent var reads and copy values out
  noInterrupts();
  copybuff = morebuff;
  morebuff = LOW;
  isMotorPaused = ( digitalRead(motorPin) == HIGH );
  interrupts();
  if(copybuff == HIGH) {
    // buffer has swapped, start from the beginning of the new page
    btemppos = 0;
    copybuff = LOW;
  }

  if(btemppos <= BUFFSIZE) { // keep filling until full
    // generate the next period to add to the buffer
    if(!TZXProcess()) return false;
    if(currentPeriod > 0) {
      // pause interrupts while we add a period to the buffer
      noInterrupts();
      wbuffer[btemppos][workingBuffer ^ 1] = currentPeriod; //add period to the buffer
      interrupts();
      btemppos += 1;
    }
  }
  return true;
}

// =======================================================================

bool ReadTZXHeader();
void StandardBlock();
void PureToneBlock();
void PulseSequenceBlock();
void PureDataBlock();
void DirectRecording();
void writeData();

// Read from file
int ReadByte(unsigned long pos);
int ReadWord(unsigned long pos);
int ReadLong(unsigned long pos);
int ReadDword(unsigned long pos);

// returns false if the player has stopped (eof) or 
// if an error was detected; returns true otherwise.
bool TZXProcess() {
  byte r = 0;
  currentPeriod = 0;

  if(currentTask == GETFILEHEADER) {
    //grab 7 byte string
    if( !ReadTZXHeader() ) return false;
    //set current task to GETID
    currentTask = GETID;
  }

  if(currentTask == GETID) {
    //grab 1 byte ID
    if(ReadByte(bytesRead) == 1) currentID = outByte;
    else currentID = IDEOF;
    //reset data block values
    currentBit = 0;
    pass = 0;
    //set current task to PROCESSID
    currentTask = PROCESSID;
    currentBlockTask = READPARAM;
  }

  if(currentTask == PROCESSID) {
    // ID Processing
    switch(currentID) {

      case ID10:
        // Process ID10 - Standard Block
        switch (currentBlockTask) {
          case READPARAM:
            if((r = ReadWord(bytesRead)) == 2) pauseLength = outWord;
            if((r = ReadWord(bytesRead)) == 2) bytesToRead = outWord + 1;
            if((r = ReadByte(bytesRead)) == 1) {
              if(outByte == 0) pilotPulses = PILOTNUMBERL;
              else pilotPulses = PILOTNUMBERH;
              bytesRead += -1;
            }
            pilotLength = PILOTLENGTH;
            sync1Length = SYNCFIRST;
            sync2Length = SYNCSECOND;
            zeroPulse = ZEROPULSE;
            onePulse = ONEPULSE;
            currentBlockTask = PILOT;
            usedBitsInLastByte = 8;
          break;
        
          default:
            StandardBlock();
          break;
        }
      break;
      
      case ID11:
        // Process ID11 - Turbo Tape Block
        switch (currentBlockTask) {
          case READPARAM:
            if((r = ReadWord(bytesRead)) == 2) pilotLength = TickToUs(outWord);
            if((r = ReadWord(bytesRead)) == 2) sync1Length = TickToUs(outWord);
            if((r = ReadWord(bytesRead)) == 2) sync2Length = TickToUs(outWord);
            if((r = ReadWord(bytesRead)) == 2) zeroPulse = TickToUs(outWord);
            if((r = ReadWord(bytesRead)) == 2) onePulse = TickToUs(outWord);
            if((r = ReadWord(bytesRead)) == 2) pilotPulses = outWord;
            if((r = ReadByte(bytesRead)) == 1) usedBitsInLastByte = outByte;
            if((r = ReadWord(bytesRead)) == 2) pauseLength = outWord;
            if((r = ReadLong(bytesRead)) == 3) bytesToRead = outLong + 1;
            currentBlockTask = PILOT;
          break;
        
          default:
            StandardBlock();
          break;
        }
      break;

      case ID12:
        // Process ID12 - Pure Tone Block
        if(currentBlockTask == READPARAM) {
          if((r = ReadWord(bytesRead)) == 2) pilotLength = TickToUs(outWord);
          if((r = ReadWord(bytesRead)) == 2) pilotPulses = outWord;
          currentBlockTask = PILOT;
        }
        else PureToneBlock();
      break;

      case ID13:
        // Process ID13 - Sequence of Pulses          
        if(currentBlockTask == READPARAM) {  
          if((r = ReadByte(bytesRead)) == 1) seqPulses = outByte;
          currentBlockTask = DATA;
        }
        else PulseSequenceBlock();
      break;

      case ID14:
        // process ID14 - Pure Data Block
        if(currentBlockTask == READPARAM) {
          if((r = ReadWord(bytesRead)) == 2) zeroPulse = TickToUs(outWord);
          if((r = ReadWord(bytesRead)) == 2) onePulse = TickToUs(outWord);
          if((r = ReadByte(bytesRead)) == 1) usedBitsInLastByte = outByte;
          if((r = ReadWord(bytesRead)) == 2) pauseLength = outWord;
          if((r = ReadLong(bytesRead)) == 3) bytesToRead = outLong + 1;
          currentBlockTask=DATA;
        }
        else PureDataBlock();
      break;
      
      case ID15:
        // process ID15 - Direct Recording
        if(currentBlockTask == READPARAM) {
          //Number of T-states per sample (bit of data) 79 or 158 - 22.6757uS for 44.1KHz
          if((r = ReadWord(bytesRead)) == 2) TstatesperSample = TickToUs(outWord); 
            //Pause after this block in milliseconds
          if((r = ReadWord(bytesRead)) == 2) pauseLength = outWord;  
          //Used bits in last byte (other bits should be 0)
          if((r = ReadByte(bytesRead)) == 1) usedBitsInLastByte = outByte;
          // Length of samples' data
          if((r = ReadLong(bytesRead)) == 3) bytesToRead = outLong + 1;
          currentBlockTask=DATA;
        } else DirectRecording();
      break;
     
      /* Old ID20
      case ID20:
        // process ID20 - Pause Block
        if((r = ReadWord(bytesRead))==2) {
          if(outWord>0) {
            currentPeriod = pauseLength;
            bitSet(currentPeriod, 15);
          }
          currentTask=GETID;
        }
      break;
      */     
      
      case ID20:
        //process ID20 - Pause Block
        if((r = ReadWord(bytesRead)) == 2) {
          if(outWord>0) {
            temppause = outWord;
            currentID = IDPAUSE;
          }
          else {
            currentTask = GETID;
          }
        }
      break;

      case ID21:
        //Process ID21 - Group Start
        if((r = ReadByte(bytesRead)) == 1) bytesRead += outByte;
        currentTask = GETID;
      break;

      case ID22:
        // Process ID22 - Group End
        currentTask = GETID;
      break;

      case ID24:
        //Process ID24 - Loop Start
        if((r = ReadWord(bytesRead)) == 2) {
          loopCount = outWord;
          loopStart = bytesRead;
        }
        currentTask = GETID;
      break;

      case ID25:
        // Process ID25 - Loop End
        loopCount += -1;
        if(loopCount != 0) {
          bytesRead = loopStart;
        } 
        currentTask = GETID;
      break;
      
      case ID30:
        // Process ID30 - Text Description
        if((r = ReadByte(bytesRead)) == 1) {
          //Show info on screen - removed
          bytesRead += outByte;
        }
        currentTask = GETID;
      break;

      case ID32:
        // Process ID32 - Archive Info
        // Block Skipped until larger screen used
        if(ReadWord(bytesRead)==2) bytesRead += outWord;
        currentTask = GETID;
      break;

      case ID33:
        // Process ID32 - Archive Info
        // Block Skipped until larger screen used
        if(ReadByte(bytesRead) == 1) bytesRead += ((long)outByte) * 3;
        currentTask = GETID;
      break;       

      case ID35:
        // Process ID35 - Custom Info Block
        // Block Skipped
        bytesRead += 10;
        if((r = ReadDword(bytesRead)) == 4) bytesRead += outLong;
      break;
      
      case TAP:
        // Pure Tap file block
        if(currentBlockTask == READPARAM) {
          pauseLength = PAUSELENGTH;
          if((r = ReadWord(bytesRead)) == 2) bytesToRead = outWord + 1;
          if((r = ReadByte(bytesRead)) == 1) {
            if(outByte == 0) pilotPulses = PILOTNUMBERL + 1;
            else pilotPulses = PILOTNUMBERH + 1;
            bytesRead += -1;
          }
          pilotLength = PILOTLENGTH;
          sync1Length = SYNCFIRST;
          sync2Length = SYNCSECOND;
          zeroPulse = ZEROPULSE;
          onePulse = ONEPULSE;
          currentBlockTask = PILOT;
          usedBitsInLastByte = 8;
        }
        else StandardBlock();
      break;

      case IDPAUSE:
        if(temppause > 0) {
          if(temppause > 1000) {
            currentPeriod = 1000;
            temppause += -1000;					
          }
          else {
            currentPeriod = temppause;
            temppause = 0;
          }
          bitSet(currentPeriod, 15);
        }
        else {
          currentTask = GETID;
          if(endOfFile) currentID = IDEOF;	
        }
      break;
  
      case IDEOF:
        //Handle end of file
        if(!count == 0) {
          currentPeriod = 32767;
          count += -1;
        } else {
          TZXStop();
          return false;
        }
      break; 
      
      default:
        delay(5000);
        TZXStop();
        errorCode = ERROR_UNKNOWN_ID;
        return false;
      break;
    }
  }

  return true;
}

void StandardBlock() {
  //Standard Block Playback
  switch (currentBlockTask) {
    case PILOT:
      //Start with Pilot Pulses
      currentPeriod = pilotLength;
      pilotPulses += -1;
      if(pilotPulses == 0) currentBlockTask = SYNC1;
    break;
    
    case SYNC1:
      // First Sync Pulse
      currentPeriod = sync1Length;
      currentBlockTask = SYNC2;
    break;
    
    case SYNC2:
      // Second Sync Pulse
      currentPeriod = sync2Length;
      currentBlockTask = DATA;
    break;
    
    case DATA:
        // Playing data
        writeData();
    break;
    
    case PAUSE:
      // Close block with a pause
      if(currentID != TAP) {
    		temppause = pauseLength;
        currentID = IDPAUSE;
      } else {
		    currentPeriod = pauseLength;
		    bitSet(currentPeriod, 15);
        currentBlockTask = READPARAM;
      }
      if(endOfFile) currentID = IDEOF;
    break;
  }
}

void PureToneBlock() {
  //Pure Tone Block - Long string of pulses with the same length
  currentPeriod = pilotLength;
  pilotPulses += -1;
  if(pilotPulses == 0) currentTask = GETID;
}

void PulseSequenceBlock() {
  //Pulse Sequence Block - String of pulses each with a different length
  //Mainly used in speedload blocks
  byte r = 0;
  if((r = ReadWord(bytesRead)) == 2) currentPeriod = TickToUs(outWord);    
  seqPulses += -1;
  if(seqPulses == 0) currentTask = GETID;
}

void PureDataBlock() {
  //Pure Data Block - Data & pause only, no header, sync
  switch(currentBlockTask) {
    case DATA:
      writeData();          
    break;
    
    case PAUSE:
      temppause = pauseLength;
	    currentID = IDPAUSE;
    break;
  }
}

void DirectRecording() {
  //Direct Recording - Output bits based on specified sample rate (Ticks per clock) either 44.1KHz or 22.05
  switch(currentBlockTask) {
    case DATA:
      writeData();          
    break;
    
    case PAUSE:
      temppause = pauseLength;
	  currentID = IDPAUSE;
    break;
  }
}

void writeData() {
  //Convert byte from file into string of pulses.  One pulse per pass
  byte r;
  if(currentBit == 0) {                           // Check for byte end/first byte
    if((r = ReadByte(bytesRead)) == 1) {          // Read in a byte
      currentByte = outByte;
      bytesToRead += -1;
      if(bytesToRead == 0) {                      // Check for end of data block
        bytesRead += -1;                          // rewind a byte if we've reached the end
        if(pauseLength == 0) currentTask = GETID; // Search for next ID if there is no pause
        else currentBlockTask = PAUSE;            // Otherwise start the pause
        return;                                   // exit
      }
    }
    else if(r == 0) {                             // If we reached the EOF
      endOfFile = TRUE;
      if(pauseLength == 0) currentTask = GETID;
      else currentBlockTask = PAUSE;
      return;                                     // return here if normal TAP or TZX
    }
    if(bytesToRead!=1) currentBit = 8;            // If we're not reading the last byte play all 8 bits
    else currentBit = usedBitsInLastByte;         // Otherwise only play back the bits needed
    pass = 0;
  } 
  if(currentByte & 0x80) currentPeriod = onePulse; // Set next period depending on value of bit 0
  else currentPeriod = zeroPulse;
  pass += 1;                                       // Data is played as 2 x pulses
  if(pass == 2) {
    currentByte <<= 1;                             // Shift along to the next bit
    currentBit += -1;
    pass = 0;  
  }    
}

// =======================================================================

void wave() {
  //ISR Output routine
  unsigned long fudgeTime = micros();         // fudgeTime is used to reduce length of the next period by
                                              // the time taken to process the ISR
  word workingPeriod = wbuffer[pos][workingBuffer];
  byte pauseFlipBit = FALSE;
  unsigned long newTime = 1;
  
  if(!isStopped && !isMotorPaused) {
    if(workingPeriod >= 1) {
      if(bitRead(workingPeriod, 15)) {
        //If bit 15 of the current period is set we're about to run a pause
        //Pauses start with a 1.5ms where the output is untouched after which the output is set LOW
        //Pause block periods are stored in milliseconds not microseconds
        isPauseBlock = TRUE;
        bitClear(workingPeriod,15);         // clear pause block flag
        pinState = !pinState;
        pauseFlipBit = TRUE;
        wasPauseBlock = TRUE;
      }
      else {
        if(!wasPauseBlock) pinState = !pinState;
        else if(!isPauseBlock) wasPauseBlock = FALSE;
      }
      digitalWrite(outputPin, pinState);                 // set the output pin state
      if(pauseFlipBit) {
        newTime = 1500;                                  // set 1.5ms initial pause block
        pinState = LOW;                                  // set next pinstate LOW
        wbuffer[pos][workingBuffer] = workingPeriod - 1; // reduce pause by 1ms as we've already pause for 1.5ms
        pauseFlipBit = FALSE;
      }
      else {
        if(isPauseBlock) {
          newTime = long(workingPeriod)*1000;            // set pause length in microseconds
          isPauseBlock = FALSE;
        }
        else newTime = workingPeriod;                    // after all that, if it's not a pause block set the pulse period 
        pos += 1;
        if(pos > BUFFSIZE) {                             // swap buffer pages if we've reached the end
          pos = 0;
          workingBuffer ^= 1;
          morebuff = HIGH;                               // request more data to fill inactive page
        } 
      }
    }
    else { // workingPeriod == 0
      newTime = 1000;                                      // just in case we have a 0 in the buffer
      pos += 1;
      if(pos > BUFFSIZE) {
        pos = 0;
        workingBuffer ^= 1;
        morebuff = HIGH;
      }
    }
  }
  else newTime = 1000000;                                // just in case we have a 0 in the buffer

  newTime += 12;
  fudgeTime = micros() - fudgeTime;                      // compensate for stupidly long ISR
  Timer1.setPeriod(newTime - fudgeTime);                 // finally set the next pulse length
}

// =======================================================================

int ReadByte(unsigned long pos) {
  //Read a byte from the file, and move file position on one if successful
  byte out[1];
  int i=0;
  if(file.seekSet(pos)) {
    i = file.read(out,1);
    if(i==1) bytesRead += 1;
  }
  outByte = out[0];
  return i;
}

int ReadWord(unsigned long pos) {
  //Read 2 bytes from the file, and move file position on two if successful
  byte out[2];
  int i = 0;
  if(file.seekSet(pos)) {
    i = file.read(out,2);
    if(i == 2) bytesRead += 2;
  }
  outWord = word(out[1],out[0]);
  return i;
}

int ReadLong(unsigned long pos) {
  //Read 3 bytes from the file, and move file position on three if successful
  byte out[3];
  int i = 0;
  if(file.seekSet(pos)) {
    i = file.read(out,3);
    if(i == 3) bytesRead += 3;
  }
  outLong = (word(out[2],out[1]) << 8) | out[0];
  return i;
}

int ReadDword(unsigned long pos) {
  //Read 4 bytes from the file, and move file position on four if successful  
  byte out[4];
  int i = 0;
  if(file.seekSet(pos)) {
    i = file.read(out,4);
    if(i == 4) bytesRead += 4;
  }
  outLong = (((unsigned long)word(out[3],out[2])) << 16) | word(out[1],out[0]);
  return i;
}

bool ReadTZXHeader() {
  //Read and check first 10 bytes for a TZX header
  char tzxHeader[11];
  
  if(file.seekSet(0)) {
    file.read(tzxHeader, 10);
    if(memcmp(tzxHeader, TZXTape, 7) != 0) {
      errorCode = ERROR_UNKNOWN_HEADER;
      TZXStop();
      return false;
    }
  }
  else {
      errorCode = ERROR_READ;
      TZXStop();
      return false;
  }
  bytesRead = 10;
  return true;
}
