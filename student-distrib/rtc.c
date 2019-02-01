#include "rtc.h"
volatile int RTC_STATUS[3] = {0,0,0}; /* store the status of the RTC register */

/* void rtc_handler(void);
 * Inputs: None
 * Return Value: none
 * Function: It is called every time the RTC generates an interrupt*/
void rtc_handler(){

  cli();			// disable interrupts
  //set flag to 1
  RTC_STATUS[0] = 1;
  RTC_STATUS[1] = 1;
  RTC_STATUS[2] = 1;
  //empty contents of register C
  outb(RTC_REG_C_OFF, RTC_INDEX_PORT);
  inb(RTC_DATA_PORT);
  sti();
  // send end-of-interrup signal to allow next interrupt to occur
  send_eoi(RTC_PIC_PORT);

}
/* void set_rtc_interrupt_freq();
 * Inputs: freq - the frequency at which the RTC is currently operating
 * Return Value: none
 * Function: It receives the frequency at which the RTC is supposed to generate interrupts and
 *            finds the corresponding rate and makes the RTC operate at that
 */
void set_rtc_interrupt_freq(int32_t freq){

  int rate;
  // RTC allowed to generate interrupts at power of 2 and upto 8192 Hz
  if((freq & (freq - 1))!= 0 || freq > 1024||freq == 0)
  return;
  // switch case to determine the rate given a particular frequency
  switch(freq){
    case 1024: rate = 0x06; break; // given to us in a table in the documents
    case 512: rate = 0x07; break;
    case 256: rate = 0x08; break;
    case 128: rate = 0x09; break; // the frequency is given to us in Hz
    case 64: rate = 0x0A; break;
    case 32: rate = 0x0B; break;
    case 16: rate = 0x0C; break;
    case 8: rate = 0x0D; break;
    case 4: rate = 0x0E; break;
    case 2: rate = 0x0F; break;
    default: rate = 0x0F; break;
  }

    cli();
    // writing to Register A as that should store our rate
    outb(NMI_MASK | RTC_REG_A_OFF, RTC_INDEX_PORT);
    uint8_t prev = inb(RTC_DATA_PORT);
    outb(NMI_MASK | RTC_REG_A_OFF, RTC_INDEX_PORT);
    // writing in the last 4 bits as divider is written there
    outb((prev & U4BIT_MASK) | rate, RTC_DATA_PORT);
    sti();
}

/* void activate_rtc_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: Inializes rtc and sets registers
 */
void activate_rtc_interrupt(){

    cli();
    outb(NMI_MASK | RTC_REG_B_OFF, RTC_INDEX_PORT);
    uint8_t prev = inb(RTC_DATA_PORT);
    outb(NMI_MASK | RTC_REG_B_OFF, RTC_INDEX_PORT);
    outb(prev | RTC_B_PERIODIC_INT, RTC_DATA_PORT);
    // outb(0x0C, 0x70);
    // inb(RTC_DATA_PORT);
    sti();

    //Enable the slave port and the rtc port
    enable_irq(PIC_SLAVE_PORT);
    enable_irq(RTC_PIC_PORT);


    // Clear the register C from RTC to allow other interrupts to occur
    outb(RTC_REG_C_OFF, RTC_INDEX_PORT);
    inb(RTC_DATA_PORT);
    // printf("Activated RTC int!! \n");

}

/* int32_t open_rtc();
 * Inputs: filename - the name of the file (input to generalise the functions)
 * Return Value: PASS/FAIL in integer form
 * Function: It sets the RTC frequency to 2Hz
 */
int32_t open_rtc(const uint8_t* filename){
  //checks if filename does not exist
  if(filename == NULL){
    return -1;
  }
  // sets the default frequency of the rtc to 2Hz
    set_rtc_interrupt_freq(RTC_DEF_FREQ);
    return 0;

}
/* nt32_t close_rtc(void);
 * Inputs: fd - file descriptor for generalization
 * Return Value: 0
 * Function: Does nothing but written to maintain format of file system
 */
int32_t close_rtc(int32_t fd){
  // should do nothing as RTC should not be closed
  return 0;
}
/*  int32_t read_rtc();
 * Inputs: fd - file descriptor for generalization
 *         buf - input for generalization
 *         nbytes - number of bytes read
 * Return Value: 0
 * Function: returns 0 only after an interrupt has occured
 */
 int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes){
   // stays in an infinite loop and breaks out as soon as an interrupt is being generated
   while(!RTC_STATUS[TERMINAL_I_FLAG]);
  // Reinitialse RTC flag to 0
   RTC_STATUS[TERMINAL_I_FLAG] = 0;
   return 0;
 }
 /* int32_t write_rtc();
  * Inputs: fd - file descriptor for generalization
  *         buf - buffer containing new frequncy of RTC
  *         nbytes - number of bytes read
  * Return Value: No. of bytes or FAIL
  * Function: It is called to change the frequency of RTC
  */
 int32_t write_rtc (int32_t fd, const void* buf, int32_t nbytes){
   // checks if buff is Null or the number of bytes not equal to 4 as it only accepts 4 byte integers
   if(buf == NULL || nbytes !=4)
   return -1;
  // type cast buf
   int32_t freq =  *((int32_t *)buf);
   // set RTC frequency to a desired frequency
   set_rtc_interrupt_freq(freq);
   //return number of bytes read
   return nbytes;

 }
