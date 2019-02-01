#include "paging.h"
#include "types.h"
/*
* paging_initialize
* DESCIPTION: initilizes the structs for PDE and PTE
* INPUT: NONE
* OUTPUT: NONE
* RETURN: returns 0
*/
uint32_t page_dir[oneK]__attribute__((aligned(0x1000)));
uint32_t page_table[oneK]__attribute__((aligned(0x1000)));
uint32_t video_page[oneK]__attribute__((aligned(0x1000)));

void paging_initialize(){
  int i;
  for(i = 0; i < oneK; i++){
    page_dir[i] = 0x02; // read/write access
    page_table[i] = ((i * fourK) | 0x02) ; // read/write access
  }
  // activating video memory
  page_dir[0] = ((unsigned int)page_table | 0x03);
  // activating paging for the three terminals
  page_table[TERM_1_VID_MEM] |= 0x03; // mark present and read write
  page_table[TERM_2_VID_MEM] |= 0x03; // mark present and read write
  page_table[TERM_3_VID_MEM] |= 0x03; // mark present and read write
  page_table[v_mem] |= 0x03; // mark present and read write

  //activating kernel memory
  page_dir[1] = fourMB | 0x83; // ignore, present, read and write

  enable_paging();
}

/*
* enable_paging()
* DESCIPTION: enables paging using cr0, cr4, cr3
* INPUT: NONE
* OUTPUT: NONE
* RETURN: returns 0
*/
void enable_paging(){
  //move page directory to  register cr3
  unsigned int cr3;
  asm volatile("mov %%cr3, %0" : "=r"(cr3));
  cr3 |=(uint32_t)page_dir;
  asm volatile("mov %0, %%cr3" :: "r"(cr3));

  //use register cr4 to store value
  unsigned int cr4;
   asm volatile("mov %%cr4, %0" : "=r"(cr4));
  cr4 |= 0x00000010;
  asm volatile("mov %0, %%cr4" :: "r"(cr4));

  //use register cr0 address
  unsigned int cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |=0x80000000;
  asm volatile("mov %0, %%cr0" :: "r"(cr0));

}
/*
* map_va2pa()
* DESCIPTION: maps the virtual address to the physical address in the page directory
* INPUT: va - virtual address of mapping
*        pa - physical address to be mapped to
* OUTPUT: NONE
* RETURN: void
*/

void map_va2pa(uint32_t va, uint32_t pa){
  page_dir[(uint32_t)(va/fourMB)] = pa | 0x87; // ignore, user, present and read and write access;
}
/*
* map_va2pa_video_mem()
* DESCIPTION: maps the virtual address to the physical address in the video page in the page directory
* INPUT: va - virtual address of mapping
*        pa - physical address to be mapped to
* OUTPUT: NONE
* RETURN: void
*/
void map_va2pa_video_mem(uint32_t va, uint32_t pa){
  page_dir[(uint32_t)(va/fourMB)] = ((unsigned int)video_page) | 0x07;//user, present and read and write access;
  video_page[0] = pa | 0x07; //user, present and read and write access;
}


/* flush_tlb()
 * Functionality: flushes tlb
 *				  NOTHING FOR NOW
 * Input: NONE
 * Return value: None
 * Side effect: as described in functionality
 */
void flush_tlb()
{
	asm volatile(
                "mov %%cr3, %%eax;"
                "mov %%eax, %%cr3;"
                :                      /* no outputs */
                :                      /* no inputs */
                :"%eax"                /* clobbered register */
                );
}
