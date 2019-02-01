/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/*
* i8259_init(void)
* Functionality:initlizes the i8259 PIC
* Input: NONE
* Return value: NONE
* Side effect: enables interrupts
*/
void i8259_init(void) {
    // //Initialize the master pic
    // outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
    // outb(slave_mask, SLAVE_8259_PORT + DATA_PORT_OFFSET);

    //Declare 4 ICWs for master AND slave
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT + DATA_PORT_OFFSET);
    outb(ICW2_SLAVE, SLAVE_8259_PORT + DATA_PORT_OFFSET);
    outb(ICW3_MASTER, MASTER_8259_PORT + DATA_PORT_OFFSET);
    outb(ICW3_SLAVE, SLAVE_8259_PORT + DATA_PORT_OFFSET);
    outb(ICW4, MASTER_8259_PORT + DATA_PORT_OFFSET);
    outb(ICW4, SLAVE_8259_PORT + DATA_PORT_OFFSET);

    outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
    outb(slave_mask, SLAVE_8259_PORT + DATA_PORT_OFFSET);
    // outb(0, MASTER_8259_PORT + DATA_PORT_OFFSET);
    // outb(0, SLAVE_8259_PORT + DATA_PORT_OFFSET);

}

/*
*  enable_irq
* Functionality:unmask the given irq interrupt
* Input: uint32_t irq_num-- number of the IRQ to unmask
* Return value: NONE
* Side effect: unmasks the specified interrupt
*/
void enable_irq(uint32_t irq_num) {
    //uint8_t mask = 0;
    //Make sure that the range is between 0-15
    if(irq_num <= 15 && irq_num > 7){
        //Slave pic mask needs to be changed
        //irq_num -= 8;
        slave_mask = slave_mask & ~(1 << (irq_num-8));

        outb(slave_mask, SLAVE_8259_PORT + DATA_PORT_OFFSET);
    }
    else if(irq_num <= 7 && irq_num >= 0){
        //Master pic mask needs to be changed
        master_mask = master_mask & ~(1 << irq_num);
        // printf("%x : Master Mask\n", master_mask);
        outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
    }
}

/*
*  disable_irq
* Functionality:disables the given irq interrupt
* Input: uint32_t irq_num-- number of the IRQ to disable
* Return value: NONE
* Side effect: unmasks the specified interrupt
*/
void disable_irq(uint32_t irq_num) {
    //uint8_t mask = 0;
    //Make sure that the range is between 0-15
    if(irq_num <= 15 && irq_num > 7){
        //Slave pic mask needs to be changed
        //irq_num -= 8;
        slave_mask = slave_mask | (1 << (irq_num-8));

        outb(slave_mask, SLAVE_8259_PORT + DATA_PORT_OFFSET);
    }
    else if(irq_num <= 7 && irq_num >= 0){
        //Master pic mask needs to be changed
        master_mask = master_mask | (1 << irq_num);

        outb(master_mask, MASTER_8259_PORT + DATA_PORT_OFFSET);
    }
}

/*
*  disable_irq
* Functionality:disables the given irq interrupt
* Input: uint32_t irq_num-- number of the end-of-interrupt signal for the specified IRQ
* Return value: NONE
* Side effect:  Send end-of-interrupt signal for the specified IRQ
*/
void send_eoi(uint32_t irq_num) {
    //If a slave port needs EOI, send it to both the pics;
    //Make sure that the range is between 0-15
    if(irq_num >= 0 && irq_num <= 15){
        if(irq_num > 7){
            outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
            outb(EOI | 2, MASTER_8259_PORT);
        }
        else{
          outb(EOI | irq_num, MASTER_8259_PORT);
        }
    }
}
