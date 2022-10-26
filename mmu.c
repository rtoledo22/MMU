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
  vmsim_addr_t real_addr; // THIS IS WHAT WILL BE RETURNED
  vmsim_addr_t sim_copy = sim_addr; // A copy, just so that I can work with it
  uint32_t UPT_index = sim_copy >> 22; // This should be the most significant 10 bits NOTE: THIS WILL GET THE NUMBER AND A BUNCH OF 0's: GET JUST THE INDEX;
  UPT_index = UPT_index & 0x3ff; // 0x3ff is 10 1's, allowing for a bit mask to enable all the index's

  vmsim_addr_t UPT_entry = (UPT_index + upper_pt_addr) * sizeof(pt_entry_t); // Gets the starting address of the index of the UPT
  vmsim_addr_t LPT_addr;
  vmsim_read_real (&LPT_addr, UPT_entry, sizeof(vmsim_addr_t));

  if (LPT_addr == 0){
    vmsim_map_fault(LPT_addr);
    vmsim_read_real(&LPT_addr, UPT_entry, sizeof(vmsim_addr_t));
  }
  
  
  uint32_t LPT_index = sim_copy >> 10;
  LPT_index = LPT_index << 22;
  LPT_index = LPT_index & 0x3ff;
  vmsim_addr_t LPT_entry = (LPT_index + LPT_addr) * sizeof(pt_entry_t);
  vmsim_read_real (&real_addr, LPT_entry, sizeof(vmsim_addr_t));

  if (real_addr == 0){
    vmsim_map_fault(real_addr);
    vmsim_read_real (&real_addr, LPT_entry, sizeof(vmsim_addr_t));
  }
  // This should be the next 10 bits after UPTindex; combine with the LPT address
  uint32_t offset = sim_copy & 0xfff; // Offset

  real_addr = real_addr + offset;

  // 0x3FF = 1111111111
  
  /*

    So what we need to do: UPTindex -> UPTentry -> LPTaddress + LPTindex -> LPTentry -> real address + offset

   */

  
  // Offset is page size minus 1? (Heard from class) = 0xFFF
  // Develop how to break virtual address into pieces
  // Check in C code how to isolate stuff before going into vcm
  // Use GDB to do the stuff above
  // Important to debug statements and find what things do
  // After that, THEN do stuff
  // REPLACE THIS WITH YOUR OWN, FULL IMPLEMENTATION OF THIS FUNCTION.
  return real_addr;
  
}
// ==============================================================================
