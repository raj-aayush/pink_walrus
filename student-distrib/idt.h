#ifndef _IDT_H
#define _IDT_H

#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "interrupt_handlers.h"
#include "rtc.h"
#include "system_call_assembly_linkage.h"
#include "pit.h"

/*Global variable definition*/
#define SYSTEM_CALL_LOC 		0x80
#define EXCEPTION				0x20
#define KEYBOARD_VEC 			0x21
#define RTC_VEC					0x28
#define PIT_VEC					0x20

/*Undefined Interrupt*/
void undefined_interrupt()
{
	printf("Undefined Interrupt");
	while(1) {}
}

/*Divide by Zero Error*/
void DE()
{
	printf("Divide by Zero Error");
	while(1) {}
}

/*Reserved*/
void DB()
{
	printf("Reserved");
	while(1) {}
}

/*Nonmaskable Interrupt Exception*/
void NMI()
{
	printf("Nonmaskable Interrupt Exception");
	while(1) {}
}

/*Breakpoint Exception*/
void BP()
{
	printf("Breakpoint Exception");
	while(1) {}
}

/*Overflow Exception*/
void OF()
{
	printf("Overflow Exception");
	while(1) {}
}

/*Bound Range Exceeded Exception*/
void BR()
{
	printf("Bound Range Exceeded Exception");
	while(1) {}
}

/*Invalid Opcode Exception*/
void UD()
{
	printf("Invalid Opcode Exception");
	while(1) {}
}

/*Device Not Available Exception*/
void NM()
{
	printf("Device Not Available Exception");
	while(1) {}
}

/*Double Fault Exception*/
void DF()
{
	printf("Double Fault Exception");
	while(1) {}
}

/*Coprocessor Segment Overrun Exception*/
void CSO()
{
	printf("Coprocessor Segment Overrun Exception");
	while(1) {}
}

/*Invalid TSS Exception*/
void TS()
{
	printf("Invalid TSS Exception");
	while(1) {}
}

/*Segment Not Present*/
void NP()
{
	printf("Segment Not Present");
	while(1) {}
}

/*Stack-Segment Fault Exception*/
void SS()
{
	printf("Stack-Segment Fault Exception");
	while(1) {}
}

/*General Protection Exception*/
void GP()
{
	printf("General Protection Exception");
	while(1) {}
}

/*Page Fault Exception*/
void PF()
{
	printf("Page Fault Exception");
	while(1) {}
}

/*Reserved*/
void Resr()
{
	printf("Reserved");
	while(1) {}
}

/*Floating Point Error Exception*/
void MF()
{
	printf("Floating Point Error Exception");
	while(1) {}
}

/*Alignment Check Exception*/
void AC()
{
	printf("Alignment Check Exception");
	while(1) {}
}

/*Machine Check Exception*/
void MC()
{
	printf("Machine Check Exception");
	while(1) {}
}

/*SIMD Floating Point Exception*/
void XF()
{
	printf("SIMD Floating Point Exception");
	while(1) {}
}

/*mapping IDT to keyboard_handler */
extern void k_handler();

/*mapping IDT to RTC_handler */
extern void R_handler();

/*mapping IDT to PIT handler */
extern void p_handler();

/* initialize IDT */
extern void init_idt();

#endif
