#ifndef _TERMINAL_DRIVER
#define _TERMINAL_DRIVER

#include "lib.h"
#include "types.h"

#define BUFFER_SIZE         128
#define CURSOR_POS          13

#define VIRTUAL_VID_MEM   0x08800000
#define VID_MEM_PTR       0xb8000

#ifndef ASM



void initialize_terminals();




//initialises terminal
int32_t open_terminal(const uint8_t* filename);

//closes the terminal
int32_t close_terminal(int32_t fd);

//reads the terminal command
int32_t read_terminal(int32_t fd, void* buf, int32_t nbytes);

//writes to the terminal buffer
int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes);
int32_t write_terminal_vir(int32_t fd, const void* buf, int32_t nbytes);
//uint8_t buf_history[100][1024];
//uint32_t curr;

#endif
#endif
