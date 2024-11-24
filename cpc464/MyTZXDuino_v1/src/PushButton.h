#ifndef __PUSHBUTTON__
#define __PUSHBUTTON__

#include <Arduino.h>

class PushButton {

private:

  uint8_t buttonPin;  
  bool is_pressed;
  uint8_t buttonState;             // the current reading from the input pin
  uint8_t lastButtonState;   // the previous reading from the input pin
  unsigned long lastDebounceTime;  // the last time the output pin was toggled
  unsigned long debounceDelay;    // the debounce time; increase if the output flickers

public:

    // constructor; takes as a parameter the
    //  pin the button is attached to.
    PushButton(uint8_t pin);

    // maintain the stat of the object by
    // sensing the input pin.
    void tick(void);

    // true if the button is the button was pressed and release
    // not to long ago, false otherwise; debounced.
    bool isPressed(void);

};

#endif