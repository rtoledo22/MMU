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
  vmsim_addr_t sim_copy = sim_addr; // A copy, just so that I can work with it
  
  uint32_t UPT_index =(sim_copy >> 22) & 0x3ff; // Move the UPT index bits to the right

  // Right now the UPT_index is showing up as 0x0; probably a problem manipulating bits
  // Simulated address is currently 17 bits...? Can I use that?
  // The simulated address for random-hop 100000 is 0x15d27; this is not the total 32 bits, as going 22 to the right leads to 0x0; 
  
  // UPT_index = UPT_index & 0x3ff; // 0x3ff is 10 1's, allowing for a bit mask to enable all the necessary bits

  vmsim_addr_t UPT_entry = (UPT_index + upper_pt_addr) * sizeof(pt_entry_t); // Gets the starting address of the index of the UPT
  vmsim_addr_t LPT_addr; // Address to read the LPT address into; to be combined with its index
  vmsim_read_real (&LPT_addr, UPT_entry, sizeof(vmsim_addr_t)); // The LPT_addr now points to the value of UPT_entry

  if (LPT_addr == 0){
    vmsim_map_fault(sim_addr); // If there is no address in the UPT index, retry the process
    vmsim_read_real(&LPT_addr, UPT_entry, sizeof(vmsim_addr_t));
  }
  
  
  uint32_t LPT_index = sim_copy << 10; // Remove the first 10 bits
  LPT_index = LPT_index >> 22; // Move the LPT index to the right 
  LPT_index = LPT_index & 0x3ff; // Mask the LPT bits
  vmsim_addr_t LPT_entry = (LPT_index + LPT_addr) * sizeof(pt_entry_t); // Get the address of the LPT entry
  vmsim_read_real (&real_addr, LPT_entry, sizeof(vmsim_addr_t)); // The real_addr now points to the value of the LPT_entry

  if (real_addr == 0){
    vmsim_map_fault(sim_addr); // If there is no address in the LPT index, retry the process
    vmsim_read_real (&real_addr, LPT_entry, sizeof(vmsim_addr_t));
  }
  
  uint32_t offset = sim_copy & 0xfff; // Offset

  real_addr = real_addr + offset; // Combine the address with the offset to get the address needed

  return real_addr;
  
}
// ==============================================================================
