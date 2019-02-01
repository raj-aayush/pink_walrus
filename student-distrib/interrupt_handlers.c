#include "interrupt_handlers.h"
#include "terminal_driver.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"
#include "systemcalls.h"

//Flags that represent the state of any of the control buttons (Active high)
static int CAPS_LOCK_PRESSED_FLAG = 0;
static int SHIFT_PRESSED_FLAG = 0;
static int CTRL_PRESSED_FLAG = 0;
static int ALT_PRESSED_FLAG = 0;


void switch_terminal(int32_t prev, int32_t curr){
		//Switch vid mem to given process number
		memcpy((void *)((prev+VID_MEM_STORE_OFFSET)*VID_MEM_SIZE), (const void *)VID_MEM_PTR, VID_MEM_SIZE);
		memcpy((void *)VID_MEM_PTR, (const void *)((curr+VID_MEM_STORE_OFFSET)*VID_MEM_SIZE), VID_MEM_SIZE);
		update_cursor(cursor_pos[curr][0], cursor_pos[curr][1], curr);
}


/*
* keyboard_handler()
* Functionality: handler for keyboard
* Input: NONE
* Return value: NONE
* Side effect: prints typed char to terminal
*/
void keyboard_handler(){
	uint8_t c = 0;
	cli();
    while(1){
				c = inb(KEYBOARD_DATA_PORT);
		    // printf("TERM_FLAG: %d, PIT_VAL: %d\n", TERMINAL_I_FLAG, pit_val);
            if(c > 0)
                break;
			}

		//write_terminal(33, &c, 1);
    keyboard_helper(c);
    /*sending EOI for keyboard IRQ */
    sti();
    send_eoi(KEYBOARD_IRQ);
}

/*
* keyboard_helper()
* Functionality: helper for keyboard handler
* Input: scancode c
* Return value: NONE
* Side effect: prints characters to screen (according to the scancode c)
*/
void keyboard_helper(uint8_t c){
	if(ENTER_FLAG[TERMINAL_I_FLAG]){
		int i = 0;
    for(i = 0; i < BUFFER_SIZE; i++) keyboard_buffer[TERMINAL_I_FLAG][i] = 0;
    keyboard_buffer_i[TERMINAL_I_FLAG] = 0;
    ENTER_FLAG[TERMINAL_I_FLAG] = 0;
	}
	int i;
	int found = -1;
	char print;
	//Backspace character
	if(c == CHAR_BACKSPACE && keyboard_buffer_i[TERMINAL_I_FLAG] != 0){
		//printf("%YOOOOONNNNNNN\n" );
			putc('\b');																		// putc handles backspace printing
			keyboard_buffer_i[TERMINAL_I_FLAG]--;
			keyboard_buffer[TERMINAL_I_FLAG][keyboard_buffer_i[TERMINAL_I_FLAG]] = 0;       // Clear char from buffer
	}
	//CapsLock
	else if(c == CAPS_LOCK){
		 CAPS_LOCK_PRESSED_FLAG = !CAPS_LOCK_PRESSED_FLAG;
	}
	//control pressed
	else if(c == CONTROL_PRESSED){
		 CTRL_PRESSED_FLAG = 1;
	}
	//control released
	else if(c == CONTROL_RELEASED){
		 CTRL_PRESSED_FLAG = 0;
	}
	//Alt pressed
	else if(c == ALT_PRESSED){
			ALT_PRESSED_FLAG = 1;
	}
	//Alt pressed
	else if(c == ALT_RELEASED){
			ALT_PRESSED_FLAG = 0;
	}
	//shift pressed
	else if(c == SHIFT_L_PRESSED || c == SHIFT_R_PRESSED){
		 SHIFT_PRESSED_FLAG = 1;
	}
	//shift released
	else if(c == SHIFT_L_RELEASED || c == SHIFT_R_RELEASED){
		 SHIFT_PRESSED_FLAG = 0;
	}
	else if(ALT_PRESSED_FLAG == 1){
			int32_t prev_i = TERMINAL_I_FLAG;
			switch(c){
					case CHAR_F1:
							//TERMINAL_I_FLAG = 0;
							//send_eoi(KEYBOARD_IRQ);
							TERMINAL_I_FLAG = 0;
							switch_terminal(prev_i, TERMINAL_I_FLAG);
							 // printf("ALT+F%d\n", TERMINAL_I_FLAG);
							 return;
							break;
					case CHAR_F2:
							//TERMINAL_I_FLAG = 1;
							TERMINAL_I_FLAG = 1;
							switch_terminal(prev_i, TERMINAL_I_FLAG);
							// printf("ALT+F2\n");

							return;
							break;
					case CHAR_F3:
							TERMINAL_I_FLAG = 2;
							switch_terminal(prev_i, TERMINAL_I_FLAG);
							// printf("ALT+F3\n");
							return;
							break;
					default:;
			}
	}
	// CTRL + L should clear the screen and reset pointer to (0, 0)
	else if(c == CHAR_L && CTRL_PRESSED_FLAG != 0){
			clear();
			reset_screen();
			write_terminal_vir(0, keyboard_buffer[TERMINAL_I_FLAG], keyboard_buffer_i[TERMINAL_I_FLAG]);
	}
	//Enter Character
	else if(c == CHAR_ENTER){
			putc_vir('\n');
			keyboard_buffer[TERMINAL_I_FLAG][keyboard_buffer_i[TERMINAL_I_FLAG]] = '\n';
			keyboard_buffer_i[TERMINAL_I_FLAG]++;
			ENTER_FLAG[TERMINAL_I_FLAG] = 1;
	}
	// Space Character
	else if(c == CHAR_SPACE){
			print = ' ';
			found = 1;
	}
	//Numbers
	else if(c >= number_code[1] && c <= number_code[0]){
			for(i = 0; i < NUMBER_NUM; i++){
					if(c == number_code[i]){
							if(SHIFT_PRESSED_FLAG) print = shift_num_ascii[i];
							else print = NUMBER_OFFSET + i;
							found = i;
							break;
					}
			}
	}
	//Lowercase Letters and other stuff
	else if(found < 0){
			for(i = 0; i < LETTER_NUM; i++){
					if(c == letter_code[i]){
							// If(either shift or caps lock but not both are pressed) print upper case letter (-32)
							if(SHIFT_PRESSED_FLAG ^ CAPS_LOCK_PRESSED_FLAG) print = LETTER_OFFSET + i - UPPER_CASE_OFFSET;
							else print = LETTER_OFFSET + i;
							found = i;
							break;
					}
			}
			// If not a letter, it could be one of the remaining chars on the keyboard
			if(found < 0){
					for(i = 0; i < ALPHANUM_KEYS_COUNT; i++){
							if(c == alphanum_code[i]){
									if(SHIFT_PRESSED_FLAG) print = alphanum_shift_ascii[i];
									else print = alphanum_ascii[i];
									found = i;
									break;
							}
					}
			}
	}
	if(found != -1 && keyboard_buffer_i[TERMINAL_I_FLAG] < (BUFFER_SIZE-1)){
			putc_vir(print);
			keyboard_buffer[TERMINAL_I_FLAG][keyboard_buffer_i[TERMINAL_I_FLAG]] = print;
			keyboard_buffer_i[TERMINAL_I_FLAG]++;
	}
}
