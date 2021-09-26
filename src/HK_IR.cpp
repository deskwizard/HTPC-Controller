#include "defines.h"

volatile uint32_t current_code = 0;
bool sending = false;

void HKInit() {

  // Wired IR Sending Using atmega32u4 Timer 3

  cli(); // stop interrupts

  IR_OUT_DDR |= (1 << IR_OUT); // set pin as output (PC6)

  // Setting those to 0 probably isn't required, not taking any chances.
  TCCR3A = 0; // set entire TCCR3A register to 0
  TCCR3B = 0; // same for TCCR3B
  TCNT3  = 0; // initialize counter value to 0
  OCR3A = 0; // = 16000000 / (1 * 1000) - 1 (must be <65536)

  // turn on CTC mode
  TCCR3B |= (1 << WGM32);

  //*   DON'T ENABLE CLOCK == TIMER OFF *//

  // Enable output on OC3A (Toggle on match)
  TCCR3A |= (1 << COM3A0);

  // enable timer3 compare interrupt
  TIMSK3 |= (1 << OCIE3A);

  sei(); // allow interrupts

}


ISR(TIMER3_COMPA_vect) {

  static int8_t bit_counter = 32; // bit 32 = header, bit 31-0 data.
  static uint8_t pulse = 0;

  if (bit_counter == 32) { // Header

    if (pulse == 0) {  // high pulse
      OCR3A = HEADER_HIGH;
    }
    else if (pulse == 1) {
      OCR3A = HEADER_LOW; // low pulse
    }
    else if (pulse == 2) {
      OCR3A = MARK_PULSE; // low pulse
      bit_counter--;
      pulse = 0;
      return;
    }

    pulse++;
  }

  else if (bit_counter < 0) { // DONE_PULSE sending
    bit_counter = 32;
    stop_timer();
  }

  else { // bits 31 to 0

    if (!pulse) { // Low pulse (actual sent bit)

      if ((current_code >> bit_counter) & 1U) {
        OCR3A = ONE_PULSE;
      }
      else {
        OCR3A = ZERO_PULSE;
      }

    }

    else {
      OCR3A = MARK_PULSE;
      bit_counter--;
    }

    pulse = !pulse;
  }

}

void send_HK(uint32_t keycode) {

  sending = true;
  current_code = keycode;

  start_timer();
}


void stop_timer() {

  // Clear clock prescaler select bit (Stops timer)
  TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));
  sending = false;
  current_code = 0;

}


//void runSerialDebug() {
//  if (Serial.available()) {
//
//    unsigned char c = Serial.read();
//
//
//    switch (c) {
//      case '+':
//        Serial.println("Volume up");
//        send_HK(HK_VOL_UP);
//        break;
//      case '-':
//        Serial.println("Volume down");
//        send_HK(HK_VOL_DN);
//        break;
//      case 'p':
//        Serial.println("power");
//        send_HK(HK_POWER);
//        break;
//      case 'm':
//        Serial.println("mute");
//        send_HK(HK_MUTE);
//        break;
//      case 'r':
//        Serial.println("repeat");
//        send_HK(HK_REPEAT);
//        break;
//    }//case
//  }
//}
