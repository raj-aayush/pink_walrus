/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H
#define NULL 0
#define BUFFER_SIZE							128
#define NUM_TERMINALS						3

//System calls.c
#define KERNEL_BLOCK_BASE   0x00800000
#define STACK_SIZE          0x00002000
#define PROGRAM_IMG_ADDR    0x08048000
#define PDE_SIZE            0x00400000
#define V_ADDR_128MB        0x08000000
#define V_ADDR_132MB        0x08400000
#define V_ADDR_136MB        0x08800000
#define FOURMB              0x00400000
#define EIGHTMB             0x00800000
#define MAX_FILE_COUNT      8
#define CURSOR_X            7
#define CURSOR_Y            1
//---


#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

uint8_t keyboard_buffer[NUM_TERMINALS][BUFFER_SIZE];
uint8_t keyboard_buffer_i[NUM_TERMINALS];
uint32_t cursor_pos[NUM_TERMINALS][2];
uint32_t process_number_array[6];
uint32_t current_pid[3];
volatile int32_t pit_val;
volatile int TERMINAL_I_FLAG;

#endif /* ASM */

#endif /* _TYPES_H */
