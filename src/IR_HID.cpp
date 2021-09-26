#include "defines.h"

uint8_t IRMode = MOUSE_MODE;
uint8_t IRLed = RED_LED;
uint8_t staticLed = GREEN_LED; // Not really used anymore
uint32_t IRLedMillis = 0;

uint32_t keycode = 0;
uint32_t lastkeycode = 0;
uint32_t lastKeyTime = 0;
uint32_t keyTime = millis();
uint32_t diff = keyTime;
uint8_t repeatCount = 0;

extern uint8_t SystemState;
extern uint8_t TVState;

void handleIR(uint32_t received_keycode) {

  keycode = received_keycode;
  //  static uint8_t repeatCount = 0;
  //
  //  uint32_t keyTime = millis();
  //  uint32_t diff = keyTime - lastKeyTime;

  // repeatCount = 0;
  keyTime = millis();
  diff = keyTime - lastKeyTime;

#ifdef SERIAL_DEBUG2
  mySerial.print("received code: 0x");
  mySerial.println(keycode, HEX);
  mySerial.print("delay: ");
  mySerial.println(diff);
#endif

  if (lastkeycode == keycode) {
    repeatCount++;
#ifdef SERIAL_DEBUG2
    mySerial.print("--- Repeat: ");
    mySerial.println(repeatCount);
#endif
    if (repeatCount == 1) {
#ifdef SERIAL_DEBUG2
      mySerial.println("Bounce - return");
#endif
      return;
    }
  } else {
    repeatCount = 0;
  }

  lastkeycode = keycode;
  lastKeyTime = keyTime;

  bitClear(keycode, 15);

#ifdef SERIAL_DEBUG2
  mySerial.print("masked code: 0x");
  mySerial.println(keycode, HEX);
#endif

  if (keycode == POWER) {

#ifdef SERIAL_DEBUG
    mySerial.println("Power");
#endif

    if (SystemState == ON) {

      flashLED();

      if (repeatCount == 0) {
        if (TVState == ON) {
          set_power(TV, OFF);
        } else {
          set_power(TV, ON);
        }

      } else if (repeatCount == 10) {
        set_power(SYSTEM, OFF);
      }

    } else if (SystemState == OFF && repeatCount == 0) {
      //----------------------------------------------------HERE!!------------------------------------------------
      pc_wake(); // Just wake the PC, it wakes up the TV and amp when it wakes
      // set_power(SYSTEM, ON);
    }

    return;
  }

  if (SystemState == ON) {
    flashLED();

    switch (keycode) {

    case MUTE:
      send_HK(HK_MUTE);
      break;
    case VOL_UP:
      //        if (repeatCount == 0) {
      send_HK(HK_VOL_UP);
      //        }
      //        else {
      //          send_HK(HK_REPEAT);
      //        }

      break;
    case VOL_DOWN:
      send_HK(HK_VOL_DN);
      break;

    case CHAN_UP:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_PAGE_UP);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.move(0, 0, min(repeatCount + 1, 10));
      }
      break;
    case CHAN_DOWN:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_PAGE_DOWN);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.move(0, 0, -1 * min(repeatCount + 1, 10));
      }
      break;

    case PLAY:
      if (IRMode == PC_MODE) {
        Consumer.write(MEDIA_PLAY_PAUSE);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.write('k');
      }
      break;
    case PAUSE:
      if (IRMode == PC_MODE) {
        Consumer.write(MEDIA_PLAY_PAUSE);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.write('k');
      }
      break;
    case FF:
      if (IRMode == PC_MODE) {
        Consumer.write(MEDIA_FAST_FORWARD);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.write('l');
      }
      break;
    case REW:
      if (IRMode == PC_MODE) {
        Consumer.write(MEDIA_REWIND);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.write('j');
      }
      break;
    case NEXT:
      if (IRMode == PC_MODE) {
        Consumer.write(MEDIA_NEXT);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.press(KEY_LEFT_SHIFT);
        BootKeyboard.press('n');
        BootKeyboard.releaseAll();
      }
      break;
    case PREV:
      if (IRMode == PC_MODE) {
        Consumer.write(MEDIA_PREVIOUS);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.press(KEY_LEFT_SHIFT);
        BootKeyboard.press('p');
        BootKeyboard.releaseAll();
      }
      break;

    case UP:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_UP_ARROW);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.move(0, -1 * min(5 * repeatCount + 1, 100));
      }
      break;
    case DOWN:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_DOWN_ARROW);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.move(0, min(5 * repeatCount + 1, 100));
      }
      break;
    case RIGHT:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_RIGHT_ARROW);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.move(min(5 * repeatCount + 1, 100), 0);
      }
      break;
    case LEFT:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_LEFT_ARROW);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.move(-1 * min(5 * repeatCount + 1, 100), 0);
      }
      break;

    case OK:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_RETURN);
      } else if (IRMode == MOUSE_MODE) {
        // Left click
        BootMouse.press();
        BootMouse.releaseAll();
      }
      break;
    case BACK:
      if (IRMode == PC_MODE) {
        BootKeyboard.write(KEY_ESC);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.press(KEY_LEFT_ALT);
        BootKeyboard.press(KEY_LEFT_ARROW);
        BootKeyboard.releaseAll();
      }
      break;

    case MORE_INFO:
      if (IRMode == PC_MODE) {
        // BootKeyboard.write(KEY_RETURN);
      } else if (IRMode == MOUSE_MODE) {
        BootMouse.press(4);
        delay(1);
        BootMouse.releaseAll();
        delay(1);
      }
      break;
    case SLIDESHOW:
      if (IRMode == PC_MODE) {
        // BootKeyboard.write(KEY_ESC);
      } else if (IRMode == MOUSE_MODE) {
        BootKeyboard.write('f');
      }
      break;

    case VISUAL:
      send_HK(HK_DISPLAY);
      break;
    case PRINT:
      //        illumUp();
      break;
    }

#ifdef SERIAL_DEBUG

    switch (keycode) {

    case MUTE:
      mySerial.println("Mute");
      break;
    case VOL_UP:
      mySerial.println("Vol_Up");
      break;
    case VOL_DOWN:
      mySerial.println("Vol_Down");

    case CHAN_UP:
      mySerial.println("Chan_Up");
      break;
    case CHAN_DOWN:
      mySerial.println("Chan_Down");
      break;

    case PLAY:
      mySerial.println("Play");
      break;
    case PAUSE:
      mySerial.println("Pause");
      break;
    case FF:
      mySerial.println("Fast Forward");
      break;
    case REW:
      mySerial.println("Rewind");
      break;
    case NEXT:
      mySerial.println("Next");
      break;
    case PREV:
      mySerial.println("Previous");
      break;

    case UP:
      mySerial.println("Up");
      break;
    case DOWN:
      mySerial.println("Down");
      break;
    case RIGHT:
      mySerial.println("Right");
      break;
    case LEFT:
      mySerial.println("Left");
      break;

    case OK:
      mySerial.println("OK");
      break;
    case BACK:
      mySerial.println("Back");
      break;

    case MORE_INFO:
      mySerial.println("More Info");
      break;
    case SLIDESHOW:
      mySerial.println("Slideshow");
      break;

    case VISUAL:
      Serial.println("illum down");
      break;
    case PRINT:
      Serial.println("illum up");
      break;
    }
    mySerial.println();
#endif

  } // if system on

} // handleIR
