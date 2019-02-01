#include "lib.h"

#ifndef _INT_HANDLER_H
#define _INT_HANDLER_H

#define KEYBOARD_DATA_PORT 			0x60
#define KEYBOARD_IRQ 	  			 	0x1
#define LETTER_NUM 		   				26
#define NUMBER_NUM         			10
#define LETTER_OFFSET      			0x61
#define NUMBER_OFFSET      			0x30
#define UPPER_CASE_OFFSET				32
#define ALPHANUM_KEYS_COUNT			11
#define BUFFER_SIZE							128


#define CAPS_LOCK								0x3A
#define CONTROL_PRESSED					0x1D
#define CONTROL_RELEASED				0x9D
#define SHIFT_L_PRESSED					0x2A
#define SHIFT_L_RELEASED				0xAA
#define SHIFT_R_PRESSED					0x36
#define SHIFT_R_RELEASED				0xB6
#define CHAR_L									0x26
#define CHAR_ENTER							0x1C
#define CHAR_SPACE							0x39
#define CHAR_BACKSPACE					0x0E
#define ALT_PRESSED							0x38
#define ALT_RELEASED						0xB8
#define CHAR_F1									0x3B
#define CHAR_F2									0x3C
#define CHAR_F3									0x3D


#define VID_MEM_PTR         0xb8000
#define VID_MEM_SIZE        0x1000
#define VID_MEM_STORE_OFFSET 0xBA


#ifndef ASM


volatile int ENTER_FLAG[3];

// The following arrays are scancodes that are sent bythe different KBD character sets
// letter_code is an array of scancodes for all letters
static const uint8_t letter_code[LETTER_NUM] = {
												0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23,
												0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19,
												0x10, 0x13, 0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D,
												0x15, 0x2C};
// number_code is an array of scancodes for all numbers
static const uint8_t number_code[NUMBER_NUM] = {0x0B, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
// shift_num_ascii is an array of ascii codes for the respective alphanumric ascii characters
// that get printed when the above scancodes are sent along with shift button pressed down
static const uint8_t shift_num_ascii[NUMBER_NUM] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};


// alphanum_code is an array of scancodes for the remaining 11 keys on the KBD
/* {`, -, =, [, ], \, ;, ', (comma), ., /} */
static const uint8_t alphanum_code[ALPHANUM_KEYS_COUNT] = { 0x29, 0x0C, 0x0D, 0x1A, 0x1B, 0x2B, 0x27, 0x28, 0x33, 0x34, 0x35};
// alphanum_ascii is an array of asciicodes for the remaining 11 keys on the KBD
static const uint8_t alphanum_ascii[ALPHANUM_KEYS_COUNT] = { '`', '-', '=', '[', ']', 0x5C, ';', 0x27, ',', '.', '/'};

// alphanum_shift_ascii is an array of ascii codes for the respective alphanumric ascii characters
// that get printed when the above scancodes are sent along with shift button pressed down
static const uint8_t alphanum_shift_ascii[ALPHANUM_KEYS_COUNT] = { '~', '_', '+', '{', '}', '|', ':', '"', '<', '>', '?'};


//system call handler
// extern void sys_handler();

//keyboard handler
extern void keyboard_handler ();

//helper for keyboard handler
extern void keyboard_helper (uint8_t c);

#endif
#endif
