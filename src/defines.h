#ifndef DEFINES_H
#define DEFINES_H

#include "Arduino.h"

#include "HID-Project.h"
#include "HK_IR.h"
#include "IR_HID.h"
#include "IR_keycodes.h"
#include "control.h"

#define PC_SLEEP_DISABLED

// #define SERIAL_DEBUG
// #define SERIAL_DEBUG2
// #define SERIAL_DEBUG3

#define OFF 0
#define ON 1

#define PC_MODE 1
#define MOUSE_MODE 2

// IO pins
#define IR_PIN 0
#define PWR_PIN 15
//#define BRIGHTNESS_PIN 10

#define RED_LED 8
#define GREEN_LED 9
#define BLUE_LED 7

// swap those around for active low
#define LED_ON LOW
#define LED_OFF HIGH
#define FLASH_DELAY 50

#define SYSTEM 1
#define TV 2
#define RECEIVER 3

// Samsung EX-Link command codes
// https://support.justaddpower.com/kb/article/16-samsung-rs232-control-exlink/
const uint8_t exlink_on_cmd[] = {0x08, 0x22, 0x00, 0x00, 0x00, 0x02, 0xd4};
const uint8_t exlink_off_cmd[] = {0x08, 0x22, 0x00, 0x00, 0x00, 0x01, 0xd5};
const uint8_t exlink_pwr_cmd[] = {0x08, 0x22, 0x00, 0x00, 0x00, 0x00, 0xd6};
const uint8_t exlink_mute_cmd[] = {0x08, 0x22, 0x02, 0x00, 0x00, 0x02, 0xd4};
const uint8_t exlink_volup_cmd[] = {0x08, 0x22, 0x01, 0x00, 0x01, 0x00, 0xd4};
const uint8_t exlink_voldn_cmd[] = {0x08, 0x22, 0x01, 0x00, 0x02, 0x00, 0xd3};

void flashLED();
#endif
