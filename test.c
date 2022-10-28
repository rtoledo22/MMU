#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vmsim.h"

int
main (int argc, char** argv){
  vmsim_addr_t test_var = 0x00000001;
  vmsim_addr_t test_var2 = 0x00000001;
  vmsim_addr_t test_var3 = 0xffffffff;
  vmsim_addr_t answer = 1;

  printf("Before vmsim_read (test_var): %x\n", answer);
  vmsim_read (&answer, test_var, sizeof(vmsim_addr_t)); // There should be a map fault here, because this address has never been initialized; initializes both the LPT and the page mapped to said LPT
  printf("After vmsim_read (test_var): %x\n", answer); // This should be 0, which would mean the real address has been found (though it contains no value)

  answer = 1;
  printf("Before vmsim_read (test_var): %x\n", answer);
  vmsim_read (&answer, test_var2, sizeof(vmsim_addr_t)); // Because this has the same simulated address, there shouldn't be code to say there's been a map fault
  printf("After vmsim_read (test_var2): %x\n", answer);

  answer = 1;
  printf("Before vmsim_read (test_var3): %x\n", answer);
  vmsim_read (&answer, test_var3, sizeof(vmsim_addr_t)); // There should be a map fault here, because this address has never been initialized; initializes both the LPT and the page mapped to said LPT
  printf ("After vmsim_read (test_var3) : %x\n", answer);

}

// NOTES: The current method of putting vmsim_read_real after a map_fault is resulting in good stuff when it comes to iterative-walk, as it works properly. 
