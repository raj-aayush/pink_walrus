#include "idt.h"

/*
* init_idt()
* Functionality: initializing IDT with appropriate vectors
* Input: NONE
* Return value: NONE
* Side effect: initialized IDT
*/
void init_idt()
{
	int i;
	/*loading IDT*/
	lidt(idt_desc_ptr);
	for (i = 0; i < NUM_VEC; i++)
	{
		idt[i].present = 0x1;				//set to present
		idt[i].dpl = 0x0;					//level 0
		idt[i].reserved0 = 0x0;				//reserved 0
		idt[i].size = 0x1;					//not sure 1 for 32bits or 0 for 16bits
		idt[i].reserved1 = 0x1;				//reserved 1
		idt[i].reserved2 = 0x1;				//reserved 2
		idt[i].reserved3 = 0x0;				//reserved 3
		idt[i].reserved4 = 0x0;				//reserved 4
		idt[i].seg_selector = KERNEL_CS;

		/*setting reserved3 for keyboard and RTC*/
		if(i == KEYBOARD_VEC || i == RTC_VEC) idt[i].reserved3 = 0x1;

		/*setting runtime parameters for undefined interrupts*/

		/* if it is system call, set to user space, should be trap */
		if (i == SYSTEM_CALL_LOC) {
			idt[i].reserved3 = 0x1;
			idt[i].dpl = 0x3;
		}

		/* Sets runtime parameters for IDT entries */
		SET_IDT_ENTRY(idt[i], undefined_interrupt);
	}

	/*setting runtime parameters for IDT*/
	SET_IDT_ENTRY(idt[0], DE);
	SET_IDT_ENTRY(idt[1], DB);
	SET_IDT_ENTRY(idt[2], NMI);
	SET_IDT_ENTRY(idt[3], BP);
	SET_IDT_ENTRY(idt[4], OF);
	SET_IDT_ENTRY(idt[5], BR);
	SET_IDT_ENTRY(idt[6], UD);
	SET_IDT_ENTRY(idt[7], NM);
	SET_IDT_ENTRY(idt[8], DF);
	SET_IDT_ENTRY(idt[9], CSO);
	SET_IDT_ENTRY(idt[10], TS);
	SET_IDT_ENTRY(idt[11], NP);
	SET_IDT_ENTRY(idt[12], SS);
	SET_IDT_ENTRY(idt[13], GP);
	SET_IDT_ENTRY(idt[14], PF);
	SET_IDT_ENTRY(idt[15], Resr);
	SET_IDT_ENTRY(idt[16], MF);
	SET_IDT_ENTRY(idt[17], AC);
	SET_IDT_ENTRY(idt[18], MC);
	SET_IDT_ENTRY(idt[19], XF);

	/*setting system call handler*/
	SET_IDT_ENTRY(idt[SYSTEM_CALL_LOC], sys_handler);
	/*setting keyboard handler*/
	SET_IDT_ENTRY(idt[KEYBOARD_VEC], k_handler);
	/*setting RTC handler*/
	SET_IDT_ENTRY(idt[RTC_VEC], R_handler);
	/*setting PIT handler*/
	SET_IDT_ENTRY(idt[PIT_VEC], p_handler);
}

/*
* R_handler()
* Functionality: wrapper for mapping to RTC handler
* Input: NONE
* Return value: NONE
* Side effect: none
*/
void R_handler()
{
	asm volatile
	(	"pushl %ebp;"
		"movl %ebp,%esp;"
		"pushl %ebx;"
		"pushl %esi;"
		"pushl %edi;"
		"pushl %eax;"
		"pushl %ecx;"
		"pushl %edx;"
		"call rtc_handler;"
		"popl %edx;"
		"popl %ecx;"
		"popl %eax;"
		"popl %edi;"
		"popl %esi;"
		"popl %ebx;"
		"leave;"
		"iret;"
	);
}

/*
* k_handler()
* Functionality: wrapper for mapping to keyboard handler
* Input: NONE
* Return value: NONE
* Side effect: none
*/
void k_handler()
{
	asm volatile
	(	"pushl %ebp;"
		"movl %ebp,%esp;"
		"pushl %ebx;"
		"pushl %esi;"
		"pushl %edi;"
		"pushl %eax;"
		"pushl %ecx;"
		"pushl %edx;"
		"call keyboard_handler;"
		"popl %edx;"
		"popl %ecx;"
		"popl %eax;"
		"popl %edi;"
		"popl %esi;"
		"popl %ebx;"
		"leave;"
		"iret;"
	);
}

/*
* p_handler()
* Functionality: wrapper for mapping to PIT handler
* Input: NONE
* Return value: NONE
* Side effect: none
*/
void p_handler()
{
	asm volatile
	(	"pushl %ebp;"
		"movl %ebp,%esp;"
		"pushl %ebx;"
		"pushl %esi;"
		"pushl %edi;"
		"pushl %eax;"
		"pushl %ecx;"
		"pushl %edx;"
		"call pit_handler;"
		"popl %edx;"
		"popl %ecx;"
		"popl %eax;"
		"popl %edi;"
		"popl %esi;"
		"popl %ebx;"
		"leave;"
		"iret;"
	);
}
