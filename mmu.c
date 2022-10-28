// ==============================================================================
/**
 * mmu.c
 */
// ==============================================================================



// ==============================================================================
// INCLUDES

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mmu.h"
#include "vmsim.h"
// ==============================================================================



// ==============================================================================
// MACROS AND GLOBALS

/** 
 * The (real) address of the upper page table.  Initialized by a call
 * to `mmu_init()`.
 */
static vmsim_addr_t upper_pt_addr = 0;
// ==============================================================================



// ==============================================================================
void
mmu_init (vmsim_addr_t new_upper_pt_addr) {

  upper_pt_addr = new_upper_pt_addr;
  
}
// ==============================================================================



// ==============================================================================
vmsim_addr_t
mmu_translate (vmsim_addr_t sim_addr) {
  vmsim_addr_t real_addr; // This is the returned address
  uint32_t UPT_index; // Index of the UPT to be used to find entry
  vmsim_addr_t UPT_entry; // Entry in the UPT
  vmsim_addr_t LPT_addr; // Address to read the LPT address into; to be combined with its index

  uint32_t LPT_index; // Index of the LPT to be used to find entry
  vmsim_addr_t LPT_entry; // Entry in the LPT  
  
  UPT_index = (sim_addr >> 22) & 0x3ff; // Move the UPT index bits to the right and mask the bits
  UPT_entry = upper_pt_addr + (UPT_index * sizeof(pt_entry_t)); // Gets the starting address of the index of the UPT
  vmsim_read_real (&LPT_addr, UPT_entry, sizeof(vmsim_addr_t)); // The LPT_addr now points to the value of UPT_entry
  if (LPT_addr == 0){
    //printf("Map fault detected at UPT! No LPT address here!\n");
    vmsim_map_fault(sim_addr); // If there is no address in the UPT index, retry the process
    vmsim_read_real (&LPT_addr, UPT_entry, sizeof(vmsim_addr_t));
    }
 

  LPT_index = sim_addr << 10; // Remove the first 10 bits
  LPT_index = LPT_index >> 22; // Move the LPT index to the right 
  LPT_index = LPT_index & 0x3ff; // Mask the LPT bits
  LPT_entry = LPT_addr + (LPT_index * sizeof(pt_entry_t)); // Get the address of the LPT entry
  vmsim_read_real (&real_addr, LPT_entry, sizeof(vmsim_addr_t)); // The real_addr now points to the value of the LPT_entry
  if (real_addr == 0){
    //printf("Map fault detected at LPT! No real page here!\n");
    vmsim_map_fault(sim_addr); // If there is no address in the LPT index, retry the process
    vmsim_read_real (&real_addr, LPT_entry, sizeof(vmsim_addr_t));
  }
 
  
  uint32_t offset = sim_addr & 0xfff; // Offset

  real_addr = real_addr + offset; // Combine the address with the offset to get the address needed

  return real_addr;
  
}
// ==============================================================================
