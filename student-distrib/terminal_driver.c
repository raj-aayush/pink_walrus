#include "terminal_driver.h"
#include "interrupt_handlers.h"
#include "lib.h"
#include "types.h"
#include "file_sys.h"
#include "paging.h"

/*
* void initialize_terminals()()
* Functionality: function to initialise the three terminals
* Input: none
* Return value: NONE
* Side effect: initializes the three terminals by copying from the given memory places to the video memory pointer
*/
void initialize_terminals(){
   TERMINAL_I_FLAG = 0;
    map_va2pa_video_mem(VIRTUAL_VID_MEM, VID_MEM_PTR);
    flush_tlb();
		memcpy((void*) TERM_1_VID_MEM_PTR, (const void*) VID_MEM_PTR, VID_MEM_SIZE);
		memcpy((void*) TERM_2_VID_MEM_PTR, (const void*) VID_MEM_PTR, VID_MEM_SIZE);
		memcpy((void*) TERM_3_VID_MEM_PTR, (const void*) VID_MEM_PTR, VID_MEM_SIZE);

  }
/*
* void reset_keyboard_buf_vars()
* Functionality: resets the kbd buffer vars
* Input: none
* Return value: NONE
* Side effect: resets kbd vars
*/
void reset_keyboard_buf_vars(){
  keyboard_buffer_i[TERMINAL_I_FLAG] = 0;
  int i = 0;
  for(i = 0; i < BUFFER_SIZE; i++) keyboard_buffer[TERMINAL_I_FLAG][i] = 0;
}

/*
* int32_t open_terminal(const uint8_t* filename)
* Input: filename
* Return value: status
* Side effect: initialises kbd buffer vars
*/
int32_t open_terminal(const uint8_t* filename){
    // Initialize the global array

    enable_cursor(CURSOR_POS, CURSOR_POS);
    clear();
    reset_screen();
    reset_keyboard_buf_vars();
    //curr=0;
    return 0;
}

/*
* int32_t close_terminal(int32_t fd)
* Input: fd
* Return value: status
* Side effect: does nothing rn
*/
int32_t close_terminal(int32_t fd){
    // Probably clear it and set it to 0??
    return 0;
}

/*
* int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes)
* Input: fd, buf, nbytes
* Return value: status
* Side effect: writes n bytes from a given buffer to the terminal
*/
int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes) {
    int i;

    const uint8_t * temp_buf = buf;
    if(temp_buf == NULL) return -1;
    cli();
    for(i = 0;  nbytes >= 0 && i < nbytes; i++){
        if(temp_buf[i] == '\0') continue;
        putc(temp_buf[i]);
    }
    sti();
  //  strncpy((uint8_t *)buf_history[curr],(uint8_t)temp_buf, strlen(temp_buf));

    return i;
}
/*
* int32_t write_terminal_vir(int32_t fd, const void* buf, int32_t nbytes)
* Input: fd, buf, nbytes
* Return value: status
* Side effect: writes n bytes from a given buffer to the terminal
*/
int32_t write_terminal_vir(int32_t fd, const void* buf, int32_t nbytes) {
    int i;

    const uint8_t * temp_buf = buf;
    if(temp_buf == NULL) return -1;
    cli();
    for(i = 0;  nbytes >= 0 && i < nbytes; i++){
        if(temp_buf[i] == '\0') continue;
        putc_vir(temp_buf[i]);
    }
    sti();
  //  strncpy((uint8_t *)buf_history[curr],(uint8_t)temp_buf, strlen(temp_buf));

    return i;
}

/*
* int32_t read_terminal(int32_t fd, void* buf, int32_t nbytes)
* Input: fd, buf, nbytes
* Return value: status
* Side effect: reads n bytes from kbd buffer and writes it to the buf provided
*/
int32_t read_terminal(int32_t fd, void* buf, int32_t nbytes) {
    while(!ENTER_FLAG[pit_val]);
    int i = 0;
    int count = 0;
    uint8_t * temp_buf = buf;
    if (keyboard_buffer[TERMINAL_I_FLAG] == NULL || temp_buf == NULL) return 0;
    for(i = 0; nbytes >= 0 && i < nbytes && i < BUFFER_SIZE && i < keyboard_buffer_i[TERMINAL_I_FLAG]; i++){
        if(keyboard_buffer[TERMINAL_I_FLAG][i] == 0) break;
        temp_buf[i] = keyboard_buffer[TERMINAL_I_FLAG][i];
        count++;
    }
    // Reset the keyboard buffer
    for(i = 0; i < BUFFER_SIZE; i++) keyboard_buffer[TERMINAL_I_FLAG][i] = 0;
    keyboard_buffer_i[TERMINAL_I_FLAG] = 0;
    ENTER_FLAG[pit_val] = 0;
    return count;
}
