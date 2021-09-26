
/*
 Arduino pro micro 16MHz/5V (Leonardo)

  Used Libraries:
  https://github.com/NicoHood/HID 2.4.4
  https://github.com/cyborg5/IRLib2 2.0.16


   *** Might needs external power to wake up from USB ***

*/

#include "defines.h"

// Include a receiver (IRLibRecvPCI or IRLibRecvLoop)
#include <IRLibRecvPCI.h>
IRrecvPCI myReceiver(IR_PIN);  // pin number for the receiver

#include <IRLibDecodeBase.h> // First include the decode base
#include <IRLib_P04_RC6.h>   // Now include only the protocols you wish to use
#include <IRLibCombo.h>      // After all protocols, include this

// All of the above automatically creates a universal decoder
// class called "IRdecode" containing only the protocols you want.
// Now declare an instance of that decoder.
IRdecode myDecoder;

extern uint8_t IRMode;
extern uint8_t IRLed;
extern uint8_t staticLed;    // Not really used anymore
extern uint32_t IRLedMillis;

extern uint32_t keycode;
extern uint8_t repeatCount;
extern uint32_t keyTime;

// Devices state variables
extern uint8_t SystemState;
extern uint8_t TVState;
extern uint8_t AmpState;


#ifdef SERIAL_DEBUG
#include <SoftwareSerial.h>
SoftwareSerial mySerial(16, 15); // RX, TX
#endif

void setIO() {

  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, LED_ON);
  digitalWrite(GREEN_LED, LED_OFF);

}


void setup() {

  HKInit();

  setIO();

  // Start USB_CDC
  Serial.begin(115200);

  // Start UART (Ex-Link RS232)
  Serial1.begin(9600);

  // debug
#ifdef SERIAL_DEBUG
  mySerial.begin(9600);
#endif

  // Start HID devices
  BootKeyboard.begin();
  BootMouse.begin();
  Consumer.begin();
  System.begin();

  myReceiver.enableIRIn(); // Start the receiver

#ifdef SERIAL_DEBUG3
  Serial1.println(F("Ready to receive IR signals"));
#endif

}

void loop() {

  // Continue looping until you get a complete signal received
  if (myReceiver.getResults()) { // Received.

    myDecoder.decode();           // Decode it
    uint32_t codeValue = myDecoder.value;

    if (myDecoder.protocolNum == RC6) {
      handleIR(codeValue);
    }

    myReceiver.enableIRIn();      // Restart receiver


#ifdef SERIAL_DEBUG
    mySerial.print(F("Code -----> 0x"));
    mySerial.println(codeValue, HEX);
    mySerial.println();
#endif

  }

  // key timeout for TV vs system power 
  //(ie. don't turn the tv back on when it's off and the system is on and the key is held)
  if (TVState == OFF && keycode == POWER && repeatCount == 2 && (millis() - keyTime >= 400)) {
    set_power(TV, ON);

#ifdef SERIAL_DEBUG
    mySerial.print(F("***** Loop -----> 0x"));
    mySerial.println(keycode, HEX);
    mySerial.println();
#endif
  }

  // LED flash reset
  if ((uint32_t)(millis() - IRLedMillis) >= FLASH_DELAY && IRLedMillis != 0 && SystemState == ON) {
    digitalWrite(IRLed, LED_OFF);
    digitalWrite(staticLed, LED_ON);
    IRLedMillis = 0;
  }


  // serial
  if (Serial.available()) {

    char c = Serial.read();

#ifdef SERIAL_DEBUG3
    Serial1.print("Serial command: ");
    Serial1.println(c);
#endif

    switch (c) {
      case 's':
        set_power(SYSTEM, OFF);

#ifdef SERIAL_DEBUG
        Serial1.println("Serial Sleep");
#endif
        break;

      case 'd':
        set_power(TV, OFF);

#ifdef SERIAL_DEBUG
        Serial1.println("Serial Display Off");
#endif
        break;

      case 'D':
        set_power(TV, ON);

#ifdef SERIAL_DEBUG
        Serial1.println("Serial Display On");
#endif
        break;
    }

  } //serial available

}

void flashLED() {
  digitalWrite(staticLed, LED_OFF);
  digitalWrite(IRLed, LED_ON);
  IRLedMillis = millis();
}
