#include "systemcalls.h"
#include "types.h"
#include "lib.h"
#include "file_sys.h"
#include "x86_desc.h"
#include "rtc.h"
#include "paging.h"
#include "systemcalls.h"

#define min(X, Y) (((X)<(Y))? (X):(Y))

uint8_t file_name_len;
uint8_t com_name_len;

//distinct fops (jump) tables
fops_t stdin_fops={read_terminal,write_terminal,open_terminal,close_terminal};
fops_t stdout_fops={read_terminal,write_terminal,open_terminal,close_terminal};
fops_t rtc_fops={read_rtc,write_rtc,open_rtc,close_rtc};
fops_t file_fops={file_read, file_write, file_open,file_close};
fops_t dir_fops={dir_read,dir_write,dir_open, dir_close};

/* execute()
 * Functionality: handler for execute system call
 *				  attempts to load and execute a new program
 *				  hands off the processor to the new program
 * Input: command (space-separated sequence of words)
 * Return value: -1 if command cannot be executed
 * 				 256 if program dies from exception
 *				 0 to 255 if program executes halt
 * Side effect: as described in functionality
 */


int32_t execute(const uint8_t* command)
{
	//Parse arguments:
    //Check if the command is valid
    if(command == NULL)
	{
        //printf("FAILED: NULL Command \n");
        return -1;
    }

    //Set the filename and com_buff to be the same size as the command because variable lengths.
    uint8_t cmd_len = strlen((int8_t *)command);
    uint8_t filename[cmd_len];
    uint8_t com_buf[cmd_len];

	//Parse the command:
    get_filename(command, filename, com_buf);
    //Check if either command is empty
    if(file_name_len == 0)
	  {
        return -1;
    }

	//Check file validity and load Dentry:
    //Check file name validity here and read file data and load dentry:
    dentry_t file_dentry;
    if(read_dentry_by_name(filename, &file_dentry) == -1)
	  {
        return -1;
    }

	//Check ELF constant:
    uint8_t file_header_data[40];
    //Sub-check to see if read_data was successful
    if(read_data(file_dentry.innodes, 0, file_header_data, 40) == -1)
	  {
        return -1;
    }

	//Check if the first four bytes of the header match the exe magic number:
    if(strncmp((int8_t *)file_header_data, (int8_t *)exe_magic_num, 4) != 0)
	  {
        return -1;
    }

	//Set up paging here:
    uint32_t curr_process_number = calc_curr_process();
    if(curr_process_number == -1)	{
      printf("Can't run more than 6 processes at the same time!\n");
		return 0;
    }
    current_pid[pit_val] = curr_process_number;
    map_va2pa(V_ADDR_128MB, EIGHTMB + curr_process_number*FOURMB);
    flush_tlb();

	//User Level Program Loader:
    uint32_t file_data_length = r_in(file_dentry.innodes);
    read_data(file_dentry.innodes, 0, (uint8_t *) PROGRAM_IMG_ADDR, file_data_length);

	//Setup fd and PCB
    pcb_t curr_pcb;
    //strncpy((int8_t*)curr_pcb.arg_buf, (int8_t*)com_buf, (uint32_t)cmd_len);
    create_pcb(filename, file_dentry, &curr_pcb, curr_process_number);

	//Prepare for context switch:
    int i = 0;
    uint32_t EIP = 0x0;
    for(i = 0; i < 4; i++){
        EIP |= (file_header_data[24+i] << (i)*8);
    }
    // Value of process stack that was given to us
    uint32_t Process_stack = 0x83ffffc;
    uint32_t NEW_TSS_ESP0 = KERNEL_BLOCK_BASE - (curr_process_number * STACK_SIZE) - 4;
    tss.ss0 = KERNEL_DS;
    tss.esp0 = NEW_TSS_ESP0;

    //Setting the pcb correctly
    pcb_t * pcb = (pcb_t *) (KERNEL_BLOCK_BASE - ((curr_process_number+1) * STACK_SIZE));
    (*pcb) = curr_pcb;
	  strncpy((int8_t*)pcb->arg_buf, (int8_t*)com_buf, (uint32_t)cmd_len);

    init_stdin(0);
    init_stdout(1);

  	asm volatile("                \n\
                  movl %%esp, %0    \n\
                  movl %%ebp, %1    \n\
                  "
                  :"=g"(pcb->esp), "=g"(pcb->ebp)
                  :

  				);
          sti();
    //Switch context
    asm volatile("pushl %0;"
                 "pushl %1;"
                 "pushfl;"
                 "pushl %2;"
                 "pushl %3;"
                 "iRET;"
                 "execute_return:;"
                 :
                 : "r"(USER_DS), "r"(Process_stack), "r"(USER_CS), "r"(EIP)
                );
    return 0;
}

/* create_pcb()
 * Functionality: helper function for open
 *				  used in execute system call
 * Input: filename (pointer to buffer with file name)
 *		  file_dentry (dentry for the file)
 *		  pcb_addr (address of pcb for current process)
 * Return value: None
 * Side effect: as described in functionality
 */
void create_pcb(const uint8_t* filename, dentry_t file_dentry, pcb_t * pcb_addr, uint32_t process_num)
{
    pcb_addr->process_number = process_num;
    //Default value for p_process_num = process_num;
    pcb_addr->p_process_number = process_num;
    if(process_num > 2){
      pcb_t * p_pcb_addr = (pcb_t *) (tss.esp0 + 4 - STACK_SIZE);
      pcb_addr->p_process_number = p_pcb_addr->process_number;
    }
    pcb_addr->files_open[2].fops_t_ptr=file_fops;
    pcb_addr->files_open[2].inode=file_dentry.innodes;
}

/* get_filename()
 * Functionality: gets in file name from command in execute
 * Input: command (space-separated sequence of words)
 *		  filename (file name is stored here)
 *		  com_buf (command name is stored here)
 * Return value: None
 * Side effect: com_buf and filename are filled/updated
 */
void get_filename(const uint8_t* command, uint8_t* filename, uint8_t* com_buf)
{
	uint8_t i = 0;
	uint8_t f_count = 0;
	uint8_t c_count = 0;
	uint8_t space_flag = 0;
	uint8_t cmd_len = strlen((int8_t *)command);

	// Assumning that the command and everything else is null terminated
	for(i = 0; i < cmd_len; i++ )
	{
		//Reached end of the filename
		if((command[i] == ' ' || command[i] == '\n' || command[i] == '\0') && space_flag == 0)
		{
			space_flag = 1;
			filename[f_count] = '\0';
		}
		else if(space_flag == 0)
		{
			filename[f_count++] = command[i];
		} //-------------------------------Assumption: Spaces are allowed in command which is probably not true
		else if(space_flag == 1)
		{
			if(command[i] == ' ')
			{
				continue; // Need to skip the spaces in the command
			}
			else
			{
				com_buf[c_count++] = command[i];
			}
		}
	}
	if(space_flag == 0)
	{
		filename[f_count] = '\0';
	}
	com_buf[c_count] = '\0';
	file_name_len = f_count;  // Counts the last null value in lenght. Needed??
	com_name_len = c_count;  // Counts the last null value in lenght. Needed??
}

/* calc_curr_process()
 * Functionality: calculates process number based on process numbers that are already allocated
 * Input: None
 * Return value: i (current process number)
 *				-1 if all process numbers are taken
 * Side effect: updates the process number array at current process number
 */
uint32_t calc_curr_process()
{
	int i;
	//iterating through process_number array
	for(i = 0; i < 6; i++ )
	{
		//returning first available process number
		if(process_number_array[i] == 0)
		{
		process_number_array[i] = 1;
		return i;
		}
	}
	return -1;
}

/* init_stdin()
 * Functionality: initializing pcb for stdin
 * Input: fd (file descriptor)
 * Return value: 0
 * Side effect: as described in functionality
 */
int32_t init_stdin(int32_t fd)
{
	//getting pcb to current process
  //masking the upper few bits
	pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);
	pcb->files_open[fd].fops_t_ptr.open=fail;
	pcb->files_open[fd].fops_t_ptr.read=read_terminal;
	pcb->files_open[fd].fops_t_ptr.write=fail;
	pcb->files_open[fd].fops_t_ptr.close=fail;
	pcb->files_open[fd].flags=1;//1 means in use
	return 0;
}

/* init_stdout()
 * Functionality: initializing pcb for stdout
 * Input: fd (file descriptor)
 * Return value: 0
 * Side effect: as described in functionality
 */
int32_t init_stdout(int32_t fd)
{
	//getting pcb to current process
  //masking the upper few bits
	pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);
	pcb->files_open[fd].fops_t_ptr.open=fail;
	pcb->files_open[fd].fops_t_ptr.read=fail;
	pcb->files_open[fd].fops_t_ptr.write=write_terminal;
	pcb->files_open[fd].fops_t_ptr.close=fail;
	pcb->files_open[fd].flags=1;// 1 means in use
	return 0;
}

/* halt()
 * Functionality: terminates a process
 *				  returns the specified value to its parent process
 * Input: status (8 bit value)
 * Return value: 0 if successful
 *				 -1 for error
 * Side effect: as described in functionality
 */
int32_t halt (uint8_t status)
{
	int i;
	//getting pcb to current process
  //masking the upper few bits
  pcb_t * pcb_halt=(pcb_t *)(tss.esp0 & 0xFFFFE000);

	//getting pcb to parent process
	uint32_t process_i = pcb_halt->process_number;
	uint32_t p_process_i = pcb_halt->p_process_number;
  uint32_t p_esp = pcb_halt->esp;
  uint32_t p_ebp = pcb_halt->ebp;

	process_number_array[process_i] = 0;

	//close any relevant FDs
	for (i=2; i<MAX_FILE_COUNT; i++)
	{
    if(pcb_halt->files_open[i].flags != 0){
      close(i);
  		pcb_halt->files_open[i].flags = 0;
    }
	}

  current_pid[pit_val] = p_process_i;

  pcb_t empty_pcb;
  pcb_t * pcb_addr = (pcb_t *) (tss.esp0 + 4 - STACK_SIZE);
  (*pcb_addr) = empty_pcb;


	//for last process, execute shell again
	if (process_i == p_process_i)
	{
		execute((uint8_t*)"shell");
	}
  else{
  	//restore parent paging
  	map_va2pa(V_ADDR_128MB, EIGHTMB + p_process_i*FOURMB );
  	flush_tlb();

    uint32_t NEW_TSS_ESP0 = KERNEL_BLOCK_BASE - (p_process_i * STACK_SIZE) - 4;
    tss.ss0 = KERNEL_DS;
    tss.esp0 = NEW_TSS_ESP0;
  }
	//restore parent data

	asm volatile("                \n\
                movl %0, %%esp    \n\
                movl %1, %%ebp    \n\
                "
                :
                :"g" (p_esp), "g" (p_ebp)
				);
	//jump to execute return
	asm volatile(	"jmp execute_return;"
				);
	return 0;
}

/* read()
 * Functionality: terminates a process
 *				  returns the specified value to its parent process
 * Input: fd (file descriptor)
 * 		  buf (buffer that will be updated)
 *		  nbytes (number of bytes)
 * Return value: number of bytes copies if successful
 *				 -1 for error
 * Side effect: as described in functionality
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
	//getting pcb to current process

	pcb_t * pcb=(pcb_t *)(tss.esp0 & 0xFFFFE000);
	if(buf==NULL || nbytes<0 || fd<0){
		return -1;
  }
  // Removing the following return 0 or changing it to return -1 makes sense but creates a bug in ls function call
  if(fd > 7){
    return -1;
  }
  if(pcb->files_open[fd].flags!=1){
		return -1;
  }
	int32_t read_bytes = (int32_t)pcb->files_open[fd].fops_t_ptr.read(fd, (char*)buf, nbytes);

  return read_bytes;
}

/* write()
 * Functionality: writes data to the terminal or to a device(RTC)
 * Input: fd (file descriptor)
 * 		  buf (buffer that will be updated)
 *		  nbytes (number of bytes)
 * Return value: number of bytes copies if successful
 *				 -1 if error due to read-only file system
 * Side effect: as described in functionality
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
// printf("FD value before write: %d\n", fd);
	if(buf==NULL || nbytes<0 || fd<0 || fd>7)
  {

		return -1;
  }
	//getting pcb to current process
	pcb_t * pcb=(pcb_t *)(tss.esp0 & 0xFFFFE000);

	if(pcb->files_open[fd].flags!=1)
  {

		return -1;
  }

  int32_t read_bytes = (int32_t)pcb->files_open[fd].fops_t_ptr.write(fd, (char*)buf, nbytes);


  return read_bytes;
}

/* open()
 * Functionality:  provides access to the file system
 * Input: filename (pointer to buffer with file name)
 * Return value: i (index of file opened)
 *				 -1 for error
 * Side effect: pcb is set up for the opened file
 */
int32_t open (const uint8_t* filename)
{
	int i;
	dentry_t dentry;

	//getting pcb to current process
	pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);// top 15

	if(strlen((const int8_t *)filename) == 0)
	{
		return -1;
	}
  if(read_dentry_by_name((const uint8_t*) filename, &dentry)==-1){
    return -1;
  }

	if(strncmp((const int8_t*)filename, (const int8_t*)"stdin",5)==0)
	{
		init_stdin(0);
		return 0;
	}

	if(strncmp((const int8_t*)filename, (const int8_t*)"stdout",6)==0)
	{
		init_stdout(1);
		return 1;
	}

	for(i=2;i<=7;i++)
	{
		//rtc type
		if(pcb->files_open[i].flags!=1)
		{
			if(dentry.file_type==0)
			{
				if(open_rtc((const uint8_t*)dentry.file_name)==-1)
					return -1;
				pcb->files_open[i].fops_t_ptr=rtc_fops;
				pcb->files_open[i].inode=dentry.innodes;
				pcb->files_open[i].flags = 1;
        pcb->files_open[i].file_position=0x0000;
      strcpy((int8_t*)  pcb->file_names[i], (int8_t*)filename);
				return i;
			}

			//dir type
			else if(dentry.file_type==1)
			{
				pcb->files_open[i].fops_t_ptr=dir_fops;
				pcb->files_open[i].inode=dentry.innodes;
				pcb->files_open[i].flags = 1;
        pcb->files_open[i].file_position=0x0000;
        //  pcb->file_names[i]=(uint8_t )filename;
      strcpy((int8_t*)  pcb->file_names[i], (int8_t*)filename);
				return i;
			}

			//regular file type
			else if(dentry.file_type==2)
			{
				pcb->files_open[i].fops_t_ptr=file_fops;
				pcb->files_open[i].inode=dentry.innodes;
				pcb->files_open[i].flags = 1;
        pcb->files_open[i].file_position=0x0000;
      strcpy((int8_t*)  pcb->file_names[i], (int8_t*)filename);
				return i;
			}
		}
	}

    //   printf("FAILING HERE 3\n" );
    return -1;
}

/* close()
 * Functionality: closes the specified file descriptor
 *				  makes it available for return from later calls to open
 * Input: fd (file descriptor)
 * Return value: 0 if successful
 *				 -1 for error
 * Side effect: as described in functionality
 */
int32_t close (int32_t fd)
{
	if(fd==0 || fd==1 || fd>7 || fd<0)
		return -1;
	//getting pcb to current process
	pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);// top 15

	if(pcb->files_open[fd].flags!=1)
		return -1;

  int32_t r=pcb->files_open[fd].fops_t_ptr.close(fd);
  pcb->files_open[fd].flags=0;
  pcb->files_open[fd].inode=0;
  pcb->files_open[fd].fops_t_ptr.read=NULL;
  pcb->files_open[fd].fops_t_ptr.write=NULL;
  pcb->files_open[fd].fops_t_ptr.open=NULL;
  pcb->files_open[fd].fops_t_ptr.close=NULL;
  pcb->files_open[fd].file_position=0;
  strcpy((int8_t*)  pcb->file_names[fd],(int8_t*) "");
	return r;
}

/* getargs()
 * Functionality: reads the programâ€™s command line arguments into a user-level buffer
 *				  NOTHING FOR NOW
 * Input: buf (buffer that will be updated)
 *		  nbytes (number of bytes)
 * Return value: 0
 * Side effect: none for now
 */
 int32_t getargs (uint8_t* buf, int32_t nbytes)
 {

 	//error check for empty buffer
 	if (nbytes == 0 || buf == NULL){
    return -1;
  }

 	//getting pcb to current process
 	pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);// top 15

  if (strlen((int8_t*)pcb->arg_buf)==0 ){
 	//error check for argument length of 0
 		return -1;
  }

 	//copying from arg_buf of current pcb to buf
 	strcpy((int8_t*)buf, (int8_t*)pcb->arg_buf);
 	return 0;
 }

/* vidmap()
 * Functionality: maps the text-mode video memory into user space at a pre-set virtual address
 *				  NOTHING FOR NOW
 * Input: screen_start
 * Return value: 0
 * Side effect: none for now
 */
 int32_t vidmap (uint8_t** screen_start){

   if(screen_start < (uint8_t**)V_ADDR_128MB || screen_start> (uint8_t**)V_ADDR_132MB)
      return -1;

      map_va2pa_video_mem(V_ADDR_136MB, VID_MEM_PTR);
      flush_tlb();
      *screen_start = (uint8_t *)V_ADDR_136MB;
   return V_ADDR_136MB;
   }

/* set_handler()
 * Functionality: related to signal handling
 *				  NOTHING FOR NOW
 * Input: signum, handler_address
 * Return value: 0
 * Side effect: none for now
 */
int32_t set_handler (int32_t signum, void* handler_address)
{

  return 0;
}

/* sigreturn()
 * Functionality: related to signal handling
 *				  NOTHING FOR NOW
 * Input: NONE
 * Return value: 0
 * Side effect: none for now
 */
int32_t sigreturn (void)
{
  return 0;
}

/* fail()
 * Functionality: returns error
 *				  NOTHING FOR NOW
 * Input: NONE
 * Return value: -1
 * Side effect: none for now
 */
int32_t fail()
{
  return -1;
}
