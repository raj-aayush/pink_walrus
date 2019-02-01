#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "lib.h"
#include "paging.h"
#include "types.h"
#include "file_sys.h"
#include "terminal_driver.h"
#include "interrupt_handlers.h"
#include "rtc.h"

#ifndef ASM

#define VID_MEM_PTR         0xb8000
#define VID_MEM_SIZE        0x1000


static const uint8_t exe_magic_num[4] = {0x7f, 0x45, 0x4c, 0x46};
int32_t execute(const uint8_t* command);
void get_filename(const uint8_t* command, uint8_t* filename, uint8_t* com_buf);
uint32_t calc_curr_process();
int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);
void create_pcb(const uint8_t* filename, dentry_t file_dentry, pcb_t * pcb_addr, uint32_t process_num);
int32_t init_stdin(int32_t fd);
int32_t init_stdout(int32_t fd);
int32_t fail();

#endif

#endif
