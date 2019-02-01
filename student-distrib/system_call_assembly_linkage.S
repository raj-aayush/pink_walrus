
#define ASM     1
#include "system_call_assembly_linkage.h"

.globl sys_handler

#jumptable for system calls
system_call_jumptable:
	.long 0,halt,execute,read,write,open,close,getargs,vidmap
#	jmp restore_regs

#
# sys_handler()
# Functionality: acts like the asm linkage for system calls and
#				 uses jumptable for redirecting to the appropriate function
# Input: none
# Return value: NONE
# Side effect: stores and restores all registers
#
sys_handler:
	pushl %ebp
	pushl %esi				#saving all regs and flags
	pushl %edi
	pushfl
	pushl %edx				#pushing args on stack
	pushl %ecx
	pushl %ebx
	cmpl $1, %eax			#checking if eax is out of bounds
	jl error_code
	cmpl $8, %eax
	jg error_code
	call *system_call_jumptable(,%eax,4)	#loading address in eax and calling function

restore_regs:
	popl %ebx				#restoring args
	popl %ecx
	popl %edx
	popfl
	popl %edi
	popl %esi
	popl %ebp				#restoring regs and flags
	iret

error_code:					#error handling code
	movl $-1, %eax
	jmp restore_regs
