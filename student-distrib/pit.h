#ifndef _PIT_H
#define _PIT_H

#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "paging.h"
#include "systemcalls.h"

#define IRQ_PIT			0
#define FREQ			11931
#define LOW_BYTE		0xFF
#define HIGH_BYTE		8
#define CHAN_0			0x40
#define CHAN_1			0x41
#define CHAN_2			0x42
#define SET_COM_REG		0x36
#define COM_REG			0x43

#define VIRTUAL_VID_MEM   0x08800000
#define page_entry_size   0x1000
#define VID_MEM_STORE_OFFSET 0xBA
#define VID_MEM_PTR       0xb8000

/*
* pit_handler()
* Functionality: handler for PIT
* Input: NONE
* Return value: NONE
* Side effect: sends interruptS using PIT
*/
void pit_handler();

/*
* pit_init()
* Functionality: initialization of PIT
* Input: NONE
* Return value: NONE
*/
void pit_init();

#endif
