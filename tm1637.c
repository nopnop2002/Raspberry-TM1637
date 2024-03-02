/*
 Raspberry Pi driving the TM1637
 port from here
 https://github.com/petrows/esp-32-tm1637 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <wiringPi.h>
#include "tm1637.h"
#include "symbols.h"

int _brightness;
int _gpio_clk;
int _gpio_dio;

static void bitDelay(void)
{
	delayMicroseconds(1);
}

//send segment signal to TM1637
static void tm1637_send_byte(uint8_t wr_data)
{
	uint8_t i;
	// Sent 8bit data
	for(i=0;i<8;i++) {
		digitalWrite(_gpio_clk, LOW);
		bitDelay();
		if(wr_data & 0x01)digitalWrite(_gpio_dio, HIGH);
		else digitalWrite(_gpio_dio, LOW);
		wr_data >>= 1;
		bitDelay();
		digitalWrite(_gpio_clk, HIGH);
		bitDelay();
	}

	pinMode(_gpio_dio, INPUT);
	digitalWrite(_gpio_clk, LOW);
	bitDelay();
	digitalWrite(_gpio_clk, HIGH);
	bitDelay();
	digitalWrite(_gpio_clk, LOW);
	bitDelay();
	pinMode(_gpio_dio, OUTPUT);
	return;
}

//send start signal to TM1637
static void tm1637_start(void)
{
	digitalWrite(_gpio_dio, LOW);
	bitDelay();
}

//send end signal to TM1637
static void tm1637_stop(void)
{
	digitalWrite(_gpio_dio, LOW);
	bitDelay();
	digitalWrite(_gpio_clk, HIGH);
	bitDelay();
	digitalWrite(_gpio_dio, HIGH);
	bitDelay();
}

void tm1637_begin(int Clk, int Data)
{
	_gpio_clk = Clk;
	_gpio_dio = Data;
	pinMode(_gpio_clk, OUTPUT);
	digitalWrite(_gpio_clk, LOW);
	bitDelay();
	pinMode(_gpio_dio, OUTPUT);
	digitalWrite(_gpio_dio, HIGH);
	bitDelay();
	digitalWrite(_gpio_clk, HIGH);
	bitDelay();
}

void tm1637_set_brightness(int Bright)
{
	// Set the brightness and it takes effect the next time it displays.
	_brightness = 0x88 + Bright;
}

// Fix address mode
// Display on specific addresses
// [Set data][Set address][Display data][Control display]
void tm1637_set_segment_fixed(const uint8_t segment_idx, const uint8_t data)
{
	tm1637_start();
	tm1637_send_byte(TM1637_ADDR_FIXED);
	tm1637_stop();
	tm1637_start();
	tm1637_send_byte(segment_idx | 0xc0);
	tm1637_send_byte(data);
	tm1637_stop();
	tm1637_start();
	tm1637_send_byte(_brightness);
	tm1637_stop();
}

// Automatic address adding mode
// Display on consecutive addresses
// [Set data][Set address][Display data1][Display data2][Display data3][Display data4][Control display]
void tm1637_set_segment_auto(const uint8_t *data, const int data_length)
{
	tm1637_start();
	tm1637_send_byte(TM1637_ADDR_AUTO);
	tm1637_stop();
	tm1637_start();
	tm1637_send_byte(0xc0);
	for (int i=0;i<data_length;i++) {
		tm1637_send_byte(data[i]);
	}
	tm1637_stop();
	tm1637_start();
	tm1637_send_byte(_brightness);
	tm1637_stop();
}

void tm1637_show_ascii(char * text)
{
	if (strlen(text) <= 4) {
		// show fix segment
		for (int segment_idx=0;segment_idx<strlen(text);segment_idx++) {
			int c = text[segment_idx];
			uint8_t seg_data = ascii_symbols[c];
			// printf("text[%d]=%d seg_data=0x%x\n", segment_idx, c, seg_data);
			tm1637_set_segment_fixed(segment_idx, seg_data);
		}

	} else {
		// show sliding segment
		uint8_t segments[4] = {0,0,0,0};

		for (int i=0;i<strlen(text);i++) {
			segments[0] = segments[1];
			segments[1] = segments[2];
			segments[2] = segments[3];
			int c = text[i];
			segments[3] = ascii_symbols[c];
			tm1637_set_segment_auto(segments, 4);
			delayMicroseconds(TM1637_AUTO_DELAY);
		}
		for (int i=0;i<4;i++) {
			segments[0] = segments[1];
			segments[1] = segments[2];
			segments[2] = segments[3];
			segments[3] = 0;
			tm1637_set_segment_auto(segments, 4);
		}
	}
}

void tm1637_show_ascii_with_time(char * text, int time)
{
	char _text[5];
	int _text_length = strlen(text);
	memset(_text, 0, sizeof(_text));
	if (_text_length == 4) {
		strcpy(_text, text);
	} else if (_text_length < 4) {
		memcpy(&_text[4-_text_length], text, _text_length);
	} else {
		memcpy(_text, text, 4);
	}

	for (int segment_idx=0; segment_idx<4; segment_idx++) {
		tm1637_set_segment_fixed(segment_idx, 0);
	}

	for (int segment_idx=3; segment_idx>=0; segment_idx--) {
		int c = text[segment_idx];
		uint8_t seg_data = ascii_symbols[c];
		// Find the lower half segment(segment=c/d/e/g)
		seg_data = seg_data & 0x5c; // 0b0101-1100
		tm1637_set_segment_fixed(segment_idx, seg_data);
		delayMicroseconds(TM1637_AUTO_DELAY);
		seg_data = ascii_symbols[c];
		tm1637_set_segment_fixed(segment_idx, seg_data);
		delayMicroseconds(TM1637_AUTO_DELAY);
	}

	delayMicroseconds(time*1000);
	for (int segment_idx=3; segment_idx>=0; segment_idx--) {
		int c = text[segment_idx];
		uint8_t seg_data = ascii_symbols[c];
		// Find the upper half segment(segment=a/b/f/g)
		seg_data = seg_data & 0x63; // 0b0110-0011
		tm1637_set_segment_fixed(segment_idx, seg_data);
		delayMicroseconds(TM1637_AUTO_DELAY);
		tm1637_set_segment_fixed(segment_idx, 0);
		delayMicroseconds(TM1637_AUTO_DELAY);
	}
}

void tm1637_set_segment_number(const uint8_t segment_idx, const uint8_t num, const bool dot)
{
	uint8_t seg_data = 0x00;

	if (num < (sizeof(numerical_symbols)/sizeof(numerical_symbols[0]))) {
		seg_data = numerical_symbols[num]; // Select proper segment image
	}

	if (dot) {
		seg_data |= 0x80; // Set DOT segment flag
	}

	tm1637_set_segment_fixed(segment_idx, seg_data);
}

void tm1637_set_number(uint16_t number)
{
	tm1637_set_number_lead_dot(number, false, 0x00);
}

void tm1637_set_number_lead(uint16_t number, const bool lead_zero)
{
	tm1637_set_number_lead_dot(number, lead_zero, 0x00);
}

void tm1637_set_number_lead_dot(uint16_t number, bool lead_zero, const uint8_t dot_mask)
{
	uint8_t lead_number = lead_zero ? 0xFF : numerical_symbols[0];

	if (number < 10) {
		tm1637_set_segment_number(3, number, dot_mask & 0x01);
		tm1637_set_segment_number(2, lead_number, dot_mask & 0x02);
		tm1637_set_segment_number(1, lead_number, dot_mask & 0x04);
		tm1637_set_segment_number(0, lead_number, dot_mask & 0x08);
	} else if (number < 100) {
		tm1637_set_segment_number(3, number % 10, dot_mask & 0x01);
		tm1637_set_segment_number(2, (number / 10) % 10, dot_mask & 0x02);
		tm1637_set_segment_number(1, lead_number, dot_mask & 0x04);
		tm1637_set_segment_number(0, lead_number, dot_mask & 0x08);
	} else if (number < 1000) {
		tm1637_set_segment_number(3, number % 10, dot_mask & 0x01);
		tm1637_set_segment_number(2, (number / 10) % 10, dot_mask & 0x02);
		tm1637_set_segment_number(1, (number / 100) % 10, dot_mask & 0x04);
		tm1637_set_segment_number(0, lead_number, dot_mask & 0x08);
	} else {
		tm1637_set_segment_number(3, number % 10, dot_mask & 0x01);
		tm1637_set_segment_number(2, (number / 10) % 10, dot_mask & 0x02);
		tm1637_set_segment_number(1, (number / 100) % 10, dot_mask & 0x04);
		tm1637_set_segment_number(0, (number / 1000) % 10, dot_mask & 0x08);
	}
}
