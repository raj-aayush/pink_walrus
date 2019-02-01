#include "pit.h"

//int32_t temp_counter = 0;
/*
* pit_handler()
* Functionality: handler for PIT
* Input: NONE
* Return value: NONE
* Side effect: sends interruptS using PIT
*/
void pit_handler(){
		int32_t prev_pit = pit_val;
		pit_val++;
		// finds the mod of 3 every time to find the pit value every time i.e. 0, 1 or 2
		pit_val %= 3;

    send_eoi(IRQ_PIT);
		pcb_t * prev_pcb;
		pcb_t * pcb;
		prev_pcb = (pcb_t *) (KERNEL_BLOCK_BASE - ((current_pid[prev_pit]+1) * STACK_SIZE));

		if(current_pid[prev_pit] != -1){
	 	asm volatile("                \n\
	                 movl %%esp, %0    \n\
	                 movl %%ebp, %1    \n\
	                 "
	                 :"=g"(prev_pcb->k_esp), "=g"(prev_pcb->k_ebp)
	                 :

	 				);
    }
		// Done saving previous task and stopping it.
		// Start new task here:
		// 		>Remap the virtual mem
		//		>Probably even the vid mem
		//		>flush_tlb
		// 		>Get new TSS
		//		>If shell exists, load esp and ebp for this task, send eoi return
		//		>If not, send eoi and launch shell

		pcb = (pcb_t *) (KERNEL_BLOCK_BASE - (current_pid[pit_val]+1) * STACK_SIZE);



		if(pit_val == TERMINAL_I_FLAG){
		     map_va2pa_video_mem(VIRTUAL_VID_MEM, VID_MEM_PTR);
		}
		else{
				map_va2pa(VIRTUAL_VID_MEM, (pit_val+VID_MEM_STORE_OFFSET)*page_entry_size);
		}
		flush_tlb();

		if(current_pid[pit_val] == -1){
    	map_va2pa(V_ADDR_128MB, EIGHTMB + pit_val*FOURMB);
		}
    else {
			map_va2pa(V_ADDR_128MB, EIGHTMB + current_pid[pit_val]*FOURMB);
		}
		flush_tlb();


		if(process_number_array[pit_val] == 0){
				send_eoi(IRQ_PIT);
				execute((uint8_t *)"shell");
		 		return;
		}
		tss.ss0 = KERNEL_DS;
		tss.esp0 = KERNEL_BLOCK_BASE - (current_pid[pit_val] * STACK_SIZE) - 4;
     if(current_pid[pit_val] != -1){
			asm volatile("                \n\
		                movl %0, %%esp    \n\
		                movl %1, %%ebp    \n\
		                "
		                :
		                :"g" (pcb->k_esp), "g" (pcb->k_ebp)
						);
			}
			send_eoi(IRQ_PIT);
		return;

}

/*
* pit_init()
* Functionality: initialization of PIT
* Input: NONE
* Return value: NONE
*/
void pit_init(){
	// initialise the first pit value with 0
	pit_val = 0;
	outb(FREQ & LOW_BYTE, CHAN_0);
	outb(FREQ >> HIGH_BYTE, CHAN_0);
	outb(SET_COM_REG, COM_REG);
	enable_irq(IRQ_PIT);
	return;
}
