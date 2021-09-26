#ifndef HK_IR_H
#define HK_IR_H

#if F_CPU == 16000000
/*
          Timer value VS pulse time: @ 16mHz, prescaler /64
          0.50ms  ->   124
          0.65ms  ->   162
          1.8ms   ->   449
          4.6ms   ->   1149
          9.0ms   ->   2248
*/
#define ZERO_PULSE 124
#define ONE_PULSE 449
#define MARK_PULSE 124
#define HEADER_HIGH 2248
#define HEADER_LOW 1149

// 16MHz -> Set clock prescaler to /64 (Starts timer)
#define start_timer() (TCCR3B |= (0 << CS32) | (1 << CS31) | (1 << CS30))

#else

// -- 8Mhz values  /8 prescaler
#define ZERO_PULSE 496
#define ONE_PULSE 1796
#define MARK_PULSE 496
#define HEADER_HIGH 8992
#define HEADER_LOW 4596

// 8MHz -> Set clock prescaler to /8 (Starts timer)
#define start_timer() (TCCR3B |= (0 << CS32) | (1 << CS31) | (0 << CS30);)
#endif

#define IR_OUT_DDR DDRC
#define IR_OUT_PORT PORTC
#define IR_OUT PC6

#define HK_POWER 0x10E03FC
#define HK_VOL_UP 0x10EE31C
#define HK_VOL_DN 0x10E13EC
#define HK_MUTE 0x10E837C
//#define HK_REPEAT 0xFFFFFFF
#define HK_DISPLAY 0x10E3BC4

void HKInit();
void send_HK(uint32_t keycode);
void stop_timer();
#endif