#include "PushButton.h"

PushButton::PushButton(uint8_t pin) {
    buttonPin = pin;
    is_pressed = false;
    pinMode(buttonPin, INPUT_PULLUP);

    buttonState = HIGH;
    lastButtonState = HIGH;
    lastDebounceTime = 0;
    debounceDelay = 50;
}
 
void PushButton::tick(void) {
    // read the state of the switch
    int reading = digitalRead(buttonPin);

    // if the switch changed, for whatever reason
    // (noise or pressing) reset the debouncing timer
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
  
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // if the button state has changed
        if (reading != buttonState) {
            buttonState = reading;
            // if the new button state is LOW, the button
            // was pressed for a sufficiently long time
            if (buttonState == HIGH) is_pressed = true;
        }
    }
    lastButtonState = reading;
  }

bool PushButton::isPressed(void) {
    if(is_pressed) {
        is_pressed = false;
        return true;
    }
    return false;
}
