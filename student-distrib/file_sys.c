
#include "file_sys.h"


/*
*read_dentry_by_name
*DESCRIPTION: function reads the file name given and populates dentry with
* it's values when it finds the file name with its values
*INPUTS:
*   fname-- name of file to be found
*   dentry-- dentry to be populated if the name exists
*OUTPUTS: 0 if successful and -1 if not
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry )
{
  uint32_t i=0;
  //invalid input
uint32_t len=strlen(( int8_t *)fname);
if(len>32)
return -1;
else
len=strlen(( int8_t *)fname);

  //iterate for length
  for(i=0;i<(num_files-1);i++)
  {
    //equals 0 if the file is found
    if( strlen((int8_t *)file_dentry[i].file_name)<=32){
      if(strncmp((int8_t *)file_dentry[i].file_name,(int8_t *)fname, len)==0 && strncmp((int8_t *)file_dentry[i].file_name,(int8_t *)fname, strlen((int8_t *)file_dentry[i].file_name))==0)
      {
        //copy all values of the file to dentry
        dentry->file_type=file_dentry[i].file_type;
        dentry->innodes=file_dentry[i].innodes;
        strcpy((int8_t *)dentry->file_name,(const int8_t *)file_dentry[i].file_name);
        return 0;
      }
  }
  //to handle casses hwere length>32 
    else if (strlen((int8_t *)file_dentry[i].file_name)>32)
    {
      if(strncmp((int8_t *)file_dentry[i].file_name,(int8_t *)fname, len)==0)
      {
        //copy all values of the file to dentry
        dentry->file_type=file_dentry[i].file_type;
        dentry->innodes=file_dentry[i].innodes;
        strcpy((int8_t *)dentry->file_name,(const int8_t *)file_dentry[i].file_name);
        return 0;
      }
    }
}
  //if unsuccessful
  return -1;
}


/*
*read_dentry_by_index
*DESCRIPTION: function finds the file coressponding to index and populates dentry with
* it's values when it finds the file name with its values
*INPUTS:
*   index-- index of file to be found
*   dentry-- dentry to be populated if the name exists
*OUTPUTS: 0 if successful and -1 if not
*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
  //63 since there user program files allowed to be there
  if(index>63 || index <0)
  return -1;
  //populate dentry with the index of the file found
  dentry->file_type=file_dentry[index].file_type;
  dentry->innodes=file_dentry[index].innodes;

  strncpy((int8_t*)dentry->file_name,(const int8_t *)file_dentry[index].file_name, 32);
  return 0;
}
/*
*read_data
*DESCRIPTION: it reads from the file with inode equla to the inode given, from
* the offset to the length and copies the data of the file into buf
*INPUTS:
*   inode-- inode of file to be read
*   offset-- starting posiution to read from
*   buf -- buffer to copy the data definition
*   length -- till where the data needs to be copied
*OUTPUTS: 0 if successful and -1 if not
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length)
{
  int32_t successful=0;
  uint32_t curr_block=0;
  uint32_t curr_location_in_block=0;
  uint8_t * addr=0;

  //finds which block you are in
  curr_block=offset/file_len;
  //finds location in block
  curr_location_in_block=offset%file_len;
  //addr of where data needs to be read`
  addr=(uint8_t *)(start_addr +(innode[inode].data[curr_block])*file_len+curr_location_in_block);
  //invalid
  if(inode< 0 || inode >b_b.innodes || innode[inode].data[curr_block]>= b_b.data_blocks)
  return -1;

  if(offset>=innode[inode].length )
  return 0;

  while(successful< length)
  {
    if(curr_location_in_block>=file_len)
    {
      //next block reached so reset location in block
      curr_block++;
      curr_location_in_block=0;
      if(innode[inode].data[curr_block] >= b_b.data_blocks)
      return -1;

      addr=(uint8_t *)(start_addr +(innode[inode].data[curr_block])*file_len);
    }
    //reached end of file
    if((successful+offset)>= innode[inode].length)
    {
    return successful;
    }
    buf[successful]=*addr;
    successful++;
    curr_location_in_block++;
    addr++;
    }
  return successful;
}
/*
*get_file_start
*DESCRIPTION: returns starting address to init
*INPUTS:
*   none
*OUTPUTS: returns starting address
*/
uint32_t get_file_start(void)
{
  return file_start;
}


/*
*get_file_start
*DESCRIPTION: returns starting address to init
*INPUTS:
*   none
*OUTPUTS: returns ending address
*/
uint32_t get_file_end(void)
{
  return file_end;
}
/*
*file_init
*DESCRIPTION: initializes all variables when file_open is called
*INPUTS:
*   none
*OUTPUTS: initilaizes all the structures
*/
int32_t file_init(void)
{
  //copy all the files
  memcpy(&b_b,(void*)get_file_start(),num_files);
  file_dentry=(dentry_t *)(get_file_start() +num_files);
  innode=(data_block_t *)(get_file_start()+ file_len);
  start_addr=get_file_start()+ (b_b.innodes+1)*file_len;
  dir_ctr=0;
  return 0;
}

/*
*file_write
*DESCRIPTION: when write to file is called
*INPUTS:
*   none
*OUTPUTS: returns -1
*/
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
  return -1;
}


/*
*file_read
*DESCRIPTION: reads the file that is provided to the function
*INPUTS:
*   none
*OUTPUTS: returns -1
*/

int32_t file_read(int32_t fd, void* buf, int32_t nbytes)
{
	int ret;
	uint32_t inode_index;
	//getting pcb to current process
 	pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);
	inode_index = pcb->files_open[fd].inode;
	ret = read_data(inode_index, pcb->files_open[fd].file_position, (uint8_t*)buf, nbytes);

	pcb->files_open[fd].file_position += ret;
	return ret;
}


/*
*file_close
*DESCRIPTION: closes the file if a file is already opened
*INPUTS:
*   none
*OUTPUTS: returns -1 if unsuccessful or 0 is successful
*/
int32_t file_close(int32_t fd){
  /*if(file_is_open==0)
  return -1;

  file_is_open=0; */
  return 0;
}

/*
*file_open
*DESCRIPTION: open the file if a file is closed
*INPUTS:
*   none
*OUTPUTS: returns -1 if unsucc
essful or 0 is successful
*/
int32_t file_open(const uint8_t* filename)
{
  file_init();
  return 0;
}

/*
*find_free_file(()
*DESCRIPTION: finds the next empty spot for a file
*INPUTS:
*   none
*OUTPUTS: returns -1 if unsucc
essful or the file position that is empty i
*/
int32_t find_free_file(){
  int32_t i = 0;
  pcb_t * pcb=(pcb_t*)(tss.esp0 & 0xFFFFE000);
  for(i = 2; i < 8; i++){
    	if(pcb->files_open[i].flags == 0) return i;
  }
  return -1;
}


/*
*dir_write
*DESCRIPTION:writes to the dir
*INPUTS:
*   none
*OUTPUTS: returns -1
*/

int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
  return -1;
}


/*
*dir_read
*DESCRIPTION:reads data into the buf
* INPUTS: none
*OUTPUTS: returns length of dtaa successfully copied into buf
*/
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes)
{
  //check boundary conditions
  if(dir_ctr >= b_b.dir_enteries )
  {
    dir_ctr=0;
  }

  else
  {
    //copy the file name to the buf
    strncpy((int8_t *) buf, (const int8_t *) file_dentry[dir_ctr].file_name, 32);
    uint32_t len=0;
    len = strlen((int8_t*)buf);
    ((uint8_t *) buf)[32] = '\0';
    dir_ctr++;
    return len;
  }
  return 0;
}


/*
*dir_close
*DESCRIPTION:closes dir
* INPUTS: none
*OUTPUTS: returns 0
*/
int32_t dir_close(int32_t fd)
{
  return 0;
}


/*
*dir_open
*DESCRIPTION:opens dir
* INPUTS: none
*OUTPUTS: returns 0
*/
int32_t dir_open(const uint8_t* filename)
{

  return 0;
}


/*
*r_in
*DESCRIPTION:helper function to get the length of the file
* INPUTS:
*     n-- dentry.innodes that corresponds to the file for which the
*          length of file is returned
*OUTPUTS: returns 0
*/

int32_t r_in(uint32_t n)
{
  return innode[n].length;
}
