#include <stdio.h>
#include <stdlib.h>
#include "vmm.lvagtben.h"
// HEY JASON. We are working together on this assignment. 
//Luke Vagt-Bendoski and Oliver Baker
//:)

//code written by jason from the class gitlab repo
int decodeAddress(int addr, int *pageNumber, int *pageOffset){
  if (addr >= PAGE_SIZE * NUM_PAGES)
    return 8;
  if (addr < 0)
    return 8;

  *pageNumber = addr / PAGE_SIZE; // bits 8-15
  *pageOffset = addr % PAGE_SIZE; // bits 0-7
  return 0;

};

//code written by jason from the class gitlab repo
int readFromBackingStore(FILE *fp, unsigned char *buffer, int pageNumber) {
  int rtnval;
  long fileOffset;

  // page n is located in bytes n * PAGE_SIZE to n * PAGE_SIZE + 255
  fileOffset = PAGE_SIZE * pageNumber;
  rtnval = fseek(fp, fileOffset, SEEK_SET);
  if (rtnval != 0)
    return rtnval;

  // read PAGE_SIZE elements, and each element is 1 byte in size
  rtnval = fread(buffer, 1, PAGE_SIZE, fp);
  if (rtnval != PAGE_SIZE)
    return 8;
  else
    return 0;
}


int main(int argc, char *argv[]){

	return 0;
}