#ifndef PAGING_H
#define PAGING_H
#ifndef ASM

#include "types.h"

#define oneK 1024
#define fourK 4096
#define fourMB 0x400000
#define v_mem 0xB8
#define TERM_1_VID_MEM 0xBA
#define TERM_2_VID_MEM 0xBB
#define TERM_3_VID_MEM 0xBC
#define TERM_1_VID_MEM_PTR 0xBA000
#define TERM_2_VID_MEM_PTR 0xBB000
#define TERM_3_VID_MEM_PTR 0xBC000


void paging_initialize();
void enable_paging();
void map_va2pa(uint32_t va, uint32_t pa);
void map_va2pa_video_mem(uint32_t va, uint32_t pa);
void flush_tlb();

#endif //ASM
#endif
