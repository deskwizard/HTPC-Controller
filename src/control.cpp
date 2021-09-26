#include "defines.h"

uint8_t SystemState = OFF;
uint8_t TVState = OFF;
uint8_t AmpState = OFF;

extern uint8_t staticLed;

void powerOff() {

  if (SystemState == ON) {
    displayOff();
    audioOff();
    pc_sleep();

    SystemState = OFF; // Set system Off

    digitalWrite(RED_LED, LED_ON);
    digitalWrite(staticLed, LED_OFF);

#ifdef SERIAL_DEBUG3
    Serial1.println("Power off");
#endif
  }
} // power_off()

void powerOn() {
  displayOn(); // will turn on the audio receiver
  pc_wake();

  SystemState = ON; // Set system On

#ifdef SERIAL_DEBUG3
  Serial1.println("System on");
#endif

} // power_on()

void displayOn() {

  if (TVState == OFF) {

    Serial1.write(exlink_on_cmd,
                  sizeof(exlink_on_cmd)); // Send TV power on command
    TVState = ON;

#ifdef SERIAL_DEBUG3
    Serial1.println("ExLink on sent");
#endif

    if (AmpState == OFF) {
      audioOn();
      SystemState = ON; // Set system On
      digitalWrite(staticLed, LED_ON);
      digitalWrite(RED_LED, LED_OFF);
    }
  }

#ifdef SERIAL_DEBUG3
  else {
    Serial1.println("TV Already Off");
  }
#endif

} // display_on()

void displayOff() {

  if (TVState == ON) {
    Serial1.write(exlink_off_cmd,
                  sizeof(exlink_off_cmd)); // Send TV power off command

#ifdef SERIAL_DEBUG3
    Serial1.println("ExLink off sent");
#endif

    TVState = OFF;
  }

#ifdef SERIAL_DEBUG3
  else {
    Serial1.println("TV already off");
  }
#endif

} // display_off()

void audioOff() {
  if (AmpState == ON) {
    // digitalWrite(PWR_PIN, LOW);           // Turn Amplifier off
    // mySender.send(NEC, 0x10E03FC, 0); // H/K AVR-20 mkII power
    send_HK(HK_POWER);
    AmpState = OFF;

#ifdef SERIAL_DEBUG3
    Serial1.println("Audio off");
#endif
  }
} // audioOff()

void audioOn() {

  if (AmpState == OFF) {
    // digitalWrite(PWR_PIN, HIGH);          // Turn Amplifier on
    //    mySender.send(NEC, 0x10E03FC, 0); // H/K AVR-20 mkII power
    send_HK(HK_POWER);
    AmpState = ON;

#ifdef SERIAL_DEBUG3
    Serial1.println("Audio on");
#endif
  }

#ifdef SERIAL_DEBUG3
  else {
    Serial1.println("Amp Already On");
  }
#endif

} // audioOn()

void pc_sleep() {

#ifdef SERIAL_DEBUG3
  Serial1.println("PC sleep");
#endif

#ifndef PC_SLEEP_DISABLED
  // Send sleep signal to HTPC
  System.write(SYSTEM_SLEEP);
#endif
} // pc_sleep()

void pc_wake() {

#ifdef SERIAL_DEBUG3
  Serial1.println("PC wake");
#endif

  // Send wake (CTRL) signal to HTPC   *** Might needs external power to work
  // ***
  Keyboard.write(KEY_LEFT_CTRL);

} // pc_wake()

void set_power(uint8_t device, uint8_t target_state) {

  if (target_state == OFF) { // target off

    switch (device) {

    case TV:
      displayOff();
      break;

    case RECEIVER:
      audioOff();
      break;

    case SYSTEM:
      powerOff();
      break;
    }
  } else if (target_state == ON) {

    switch (device) {

    case TV:
      displayOn();
      break;

    case RECEIVER:
      audioOn();
      break;

    case SYSTEM:
      powerOn();
      break;
    }
  } // target on

} // set_power()
