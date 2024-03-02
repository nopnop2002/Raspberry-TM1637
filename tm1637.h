#include <stdint.h>
#include <stdbool.h>

//**** definitions for GPIO
#define CLK_GPIO 3 //Pin#5
#define DIO_GPIO 2 //Pin#3

//**** definitions for brightness
#define BRIGHT_DARKEST 0
#define BRIGHT_TYPICAL 2
#define BRIGHTEST 7

//***** definitions for TM1637
#define TM1637_ADDR_AUTO  0x40
#define TM1637_ADDR_FIXED 0x44
#define TM1637_AUTO_DELAY 300000

void tm1637_begin(int Clk, int Data);
void tm1637_set_brightness(int Bright);
void tm1637_set_segment_fixed(const uint8_t segment_idx, const uint8_t data);
void tm1637_set_segment_auto(const uint8_t *data, const int data_length);
void tm1637_show_ascii(char * text);
void tm1637_show_ascii_with_time(char * text, int time);
void tm1637_set_segment_number(const uint8_t segment_idx, const uint8_t num, const bool dot);
void tm1637_set_number(uint16_t number);
void tm1637_set_number_lead(uint16_t number, const bool lead_zero);
void tm1637_set_number_lead_dot(uint16_t number, bool lead_zero, const uint8_t dot_mask);
