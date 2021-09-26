#ifndef CONTROL_H
#define CONTROL_H

void powerOff();
void displayOn();
void displayOff();
void audioOn();
void audioOff();
void pc_sleep();
void set_power(uint8_t device, uint8_t target_state);
void pc_wake();
#endif