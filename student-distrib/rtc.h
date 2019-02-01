#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "tests.h"

#define RTC_INDEX_PORT      0x70
#define RTC_DATA_PORT       0x71
#define NMI_MASK            0x80
#define RTC_REG_A_OFF       0x0A
#define RTC_REG_B_OFF       0x0B
#define RTC_REG_C_OFF       0x0C
#define RTC_B_PERIODIC_INT  0x40
#define RTC_PIC_PORT        0x08
#define PIC_SLAVE_PORT      0x02
//Upper 4 bit mask in a byte -> 11110000
#define U4BIT_MASK          0xF0
#define RTC_IS_OPEN         0x01  /* means RTC is busy*/
#define RTC_DEF_FREQ        0x02  /* RTC frequency is 2Hz by default */

#ifndef ASM

void set_rtc_interrupt_freq(int32_t freq);
void activate_rtc_interrupt();
void rtc_handler();
int32_t open_rtc(const uint8_t* filename);
int32_t close_rtc(int32_t fd);
int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes);
int32_t write_rtc (int32_t fd, const void* buf, int32_t nbytes);

#endif
#endif
