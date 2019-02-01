#ifndef _FILE_SYS_H
#define _FILE_SYS_H


#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "rtc.h"
#include "paging.h"

#define file_len 0x1000
#define num_files 64
#define MAX_FILE_COUNT      8
#ifndef ASM
//struct for the boot block
typedef struct boot_block_t
{
    uint32_t dir_enteries;
    uint32_t innodes;
    uint32_t data_blocks;
    uint8_t reserved[52];

}boot_block_t;

//struct for dentry
typedef struct dentry_t
{
    uint8_t file_name[32];
    uint32_t file_type;
    uint32_t innodes;
    uint8_t reserved[24];

}dentry_t;

//struct for data block
typedef struct data_block_t
{
    uint32_t length;
    uint32_t data[1023];
}data_block_t;

//struct for fops table
typedef struct fops_t{
    int32_t (* read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (* write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (* open) (const uint8_t* filename);
    int32_t (* close) (int32_t fd);
}fops_t;

//struct for file descriptor
typedef struct file_descriptor_t{
    fops_t  fops_t_ptr;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
}file_descriptor_t;

//struct for pcb
typedef struct pcb_t{
    file_descriptor_t files_open[MAX_FILE_COUNT];
    uint8_t file_names[8][32];
    uint8_t process_number;
    uint8_t p_process_number;
    uint8_t is_process_running;
    uint8_t ret_val;
    uint32_t p_kernel_stack_pointer;
    uint32_t p_kernel_base_pointer;
    uint8_t arg_buf[128];
    uint32_t esp;
    uint32_t ebp;
    uint32_t k_esp;
    uint32_t k_ebp;
} pcb_t;

boot_block_t  b_b;
dentry_t * file_dentry;
data_block_t * innode;

//universal variables
int32_t start_addr;

int32_t file_is_open;

int32_t dir_ctr;

uint32_t file_end;

uint32_t file_start;

int32_t file_is_open;
int32_t find_free_file();


//functions to be implemented
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry );

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);


//helper functions
int32_t file_init(void);

uint32_t get_file_start(void);

uint32_t get_file_end(void);


//all functions done on files//
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
//use read_data
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

int32_t file_close(int32_t fd);

int32_t file_open(const uint8_t* filename);


// all fucntions done on directories//
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

int32_t dir_close(int32_t fd);

int32_t dir_open(const uint8_t* filename);


//helper functions//
int32_t r_in(uint32_t n);




#endif
#endif
