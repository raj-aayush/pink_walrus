#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "terminal_driver.h"
#include "file_sys.h"
#include "types.h"
#include "systemcalls.h"

#define PASS 1
#define FAIL 0

#define test_deref 0
#define test_valid_paging 0
#define test_invalid_paging 0
#define NULL_PTR_TEST 0
#define RTC_AND_KBD_TEST 0
#define DIVIDE_BY_ZERO_TEST 0
#define File_name_test   0
#define file_contents_test 0
#define index_test    0
#define rtc_test_  0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* idt_null_check
* Test case to check for a null pointer exception
*/
int idt_null_check(){
		int * null_ptr;
		int test_ptr = *null_ptr;
		test_ptr++;
		return 0;
}

/* divide_zero_check
* Checks whether a variable is trying to be divided by 0
*/
int divide_zero_check(){
	int j = 0;
	int i = 1/j;
	i++;
	return 0;
}

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}
//to check dereferencing NULL
int test_derefernce_null()
{
	TEST_HEADER;
	int *ptr=(int*) NULL;
	printf("%#x",ptr,*ptr);
	return 0;
}
// to check valid input for paging
int  test_video_mem_valid()
{
	TEST_HEADER;
	int *ptr=(int*) 0xB8FFF;

	printf("%#x",ptr,*ptr);
	return 0;
}
// to check invalid input for paging
int  test_video_mem_invalid()
{
	TEST_HEADER;
	int *ptr=(int*) 0xFFFFFFFF;

	printf("%#x",ptr,*ptr);
	return 0;
}
/* Checkpoint 2 tests */
/* RTC Test
 *
 * Calls rtc_read and write to check functionality
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void rtc_test(){
	//printf("Entered rtc_read");
	int i, j;
	uint8_t term_arg = '1';
	for(i = 2; i <= 1024; i *= 2){
		clear();
		// resets the video memory to 0,0
		reset_screen();
		write_rtc(0, &i, 4);
		for(j = 1; j <= i; j++){
			read_rtc(0,&i,0);
			write_terminal(0, &term_arg, 1);
		}
	}
	clear();
	reset_screen();
	// call to open rtc to check if it is set back to 0
	uint8_t ignore = 33;
	open_rtc(&ignore);
	for(i = 1; i<= 20; i++){
		read_rtc(0,&i,0);
		write_terminal(0, &term_arg, 1);
	}

}

void test_term_read_write(uint8_t buf_size){
		uint8_t temp_buffer[buf_size+1];
		int i = 0;
		for(i = 0; i < buf_size; i++) temp_buffer[i] = 0;
		if(buf_size != 0){
			temp_buffer[buf_size - 1] = '\n';
			temp_buffer[buf_size] = '\0';
		}
		read_terminal(0, temp_buffer, buf_size);
		write_terminal(0, temp_buffer, buf_size);
		//execute(temp_buffer);
}

void test_file_filename()
{
	  dentry_t dentry;
	    int i;
	    for( i=0;i<16;i++)
	    {
	      read_dentry_by_index(i, &dentry);
				uint8_t temp[32];
				int j=0;
				int k=0;
				for(j=0;j<32;j++)
				{
					if(temp[j]!='\0' && k==0)
					{
						temp[j]=dentry.file_name[j];
					}
					if(temp[j]=='\0' && k==0)
					k=1;
					if(k==1)
					temp[j]=' ';
		}
	      printf("File_name:");
				for(j=0;j<32;j++)
				printf("%c", temp[j]);
        printf(",");
        printf(" File_type: ");
        printf("%d", dentry.file_type );
        printf(",");
        printf(" file_size:    ");
	      putc(' ');
	      printf("%d", r_in(dentry.innodes));
	      putc('\n');

	    }
}

void test_file_content()
{
		dentry_t dentry;
		int ctr=0;
		int8_t * ch="verylargetextwithverylongname.txt";
		//USE T TO TEST
		int8_t * t="pingpong";
		if(strncmp((const int8_t*)ch,(const int8_t*)t,(strlen((const int8_t*)t)))==0)
		{
				read_dentry_by_index(11, &dentry);
				ctr=1;
		}
		else
				read_dentry_by_name((uint8_t*)t, &dentry);
		uint32_t curr=r_in(dentry.innodes);
		uint8_t * buf[curr];
		read_data(dentry.innodes,0,(uint8_t *)buf,curr);
		write_terminal(0,buf,curr);
		putc('\n');
		printf(" File_name: ");
		if(ctr==1)
		printf("%s","verylargetextwithverylongname.tx");
		else
		printf("%s",dentry.file_name);
}

void test_by_index()
{
		dentry_t dentry;
		//CHNAGE I TO CHNAGE INDEX
    int i=22;
		read_dentry_by_index(i, &dentry);
		uint8_t  temp[32];
		int j=0;
		int k=0;
		for(j=0;j<32;j++)
		{
			if(temp[j]!='\0' && k==0)
			{
				temp[j]=dentry.file_name[j];
			}
			if(temp[j]=='\0' && k==0)
			k=1;
			if(k==1)
			temp[j]=' ';
		}
		printf("File_name:");
		for(j=0;j<32;j++)
		{
				if(-1==read_dentry_by_index(i, &dentry))
				{
					printf("Invalid index");
					break;
				}
				else printf("%c", temp[j]);
		}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests(){
	// CP1 tests
	// launch your tests here
		// printf("Starting process #1\n");
		// execute((uint8_t *)"shell");
		// printf("Starting process #2\n");
		// execute((uint8_t *)"shell");
		// printf("Starting process #3\n");
		// execute((uint8_t *)"shell");


		//Pushal First
		//Change ebp
		//Popal
		//iRET
}
