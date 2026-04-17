#include <stdio.h>
#include <stdlib.h>
#include "vmm.lvagtben.h"
#include <string.h>
#define BUFLEN 256
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

int getFrameNumber(PageTableInfo *pageTableInfo, int logicalPageNumber,
int accessTime, int *pageFault){
	int i, haveFreeFrame = 0, freeFrameIndex = 0, frameNumber,
	minAccessTime = pageTableInfo->accessTime[0], minAccessTimeFrameNumber = -1;
	//check if the page is in the page table
	frameNumber = pageTableInfo->pageTable[logicalPageNumber];
	if(frameNumber != -1){
		pageTableInfo->accessTime[frameNumber] = accessTime;
		pageTableInfo->freeFrame[frameNumber] = 0;
		*pageFault = 0;
		return frameNumber;
	}
	// Page fault, we need to find a free frame or replace one
	*pageFault = 1;
	// Check for free frame
	for(int i=0; i<NUM_FRAMES; i++){
		if(pageTableInfo->freeFrame[i] == 1){
			//if we find a free frame, use it
			freeFrameIndex = i;
			pageTableInfo->pageTable[logicalPageNumber] = freeFrameIndex;
			pageTableInfo->accessTime[freeFrameIndex] = accessTime;
			pageTableInfo->freeFrame[freeFrameIndex] = 0;
			return freeFrameIndex;
		}
		if(pageTableInfo->accessTime[i] < minAccessTime){
			minAccessTime = pageTableInfo->accessTime[i];
			minAccessTimeFrameNumber = i;
		}
	}

/*
=> reference to page 113
=> EVICT! oldest frame is 0 (access time = 0)
=> the page mapped to frame 0 is 66: page 66 is now unmapped (not in memory)
*/



	//If we don't have a free frame, replace the one used longest ago.
	// Set the new page to the frame we are replacing
	printf("=> reference to page %d\n", logicalPageNumber);
	pageTableInfo->pageTable[logicalPageNumber] = minAccessTimeFrameNumber;
	//update the access time for the frame we are replacing
	pageTableInfo->accessTime[minAccessTimeFrameNumber] = accessTime;
	printf("=> EVICT! oldest frame is %d (access time = %d)\n", minAccessTimeFrameNumber, minAccessTime);
	// Set the old page to -1 since it is no longer in memory
	//to do this we find the page that is currently using the frame we are replacing and set it to -1
	for(int i=0; i<NUM_PAGES; i++){
		if(pageTableInfo->pageTable[i] == minAccessTimeFrameNumber){
			pageTableInfo->pageTable[i] = -1;
			printf("=> the page mapped to frame %d is %d: page %d is now unmapped (not in memory)\n",
				 minAccessTimeFrameNumber, i, i);
			return minAccessTimeFrameNumber;
		}
	}
	

}



int main(int argc, char *argv[]){
	int accessTime = 0, virtualAddress, len, pageNumber, pageOffset,
	pageFault, frameNumber, byteIndex;
  	char byteToFind, *chp;
  	char bufferAddress[BUFLEN], bufferBacking[BUFLEN], physicalMemory[NUM_FRAMES * PAGE_SIZE];
	FILE *addresses, *backingStore;
	PageTableInfo pageTableInfo;
	addresses = fopen("addresses.txt", "r");
	backingStore = fopen("BACKING_STORE.dat", "rb");

 	if (addresses == NULL) {
    	fprintf(stderr, "cannot read file 'addresses.txt'\n");
   		return 8;
 	}
	if(backingStore == NULL){
		fprintf(stderr, "cannot read file 'BACKING_STORE.bin'\n");
		return 8;
	}
	
	for(int i = 0; i < NUM_FRAMES; i++){

		pageTableInfo.freeFrame[i] = 1;
		pageTableInfo.accessTime[i] = 0;
	}
	for(int i=0; i < NUM_PAGES; i++){
		pageTableInfo.pageTable[i] = -1;
	}


	// Read a virtual address from addresses.txt
    chp = fgets(bufferAddress, BUFLEN, addresses);
	// While read is successful
	while(chp !=NULL){
	    bufferAddress[strlen(bufferAddress)-1] = '\0';
		// Convert text address to int address
		virtualAddress = atoi(bufferAddress);
		// Decode the address
		decodeAddress(virtualAddress, &pageNumber, &pageOffset);
		// Get frame number using decoded address
		frameNumber = getFrameNumber(&pageTableInfo, pageNumber, accessTime, &pageFault);
		// If page fault, read from backing store and save those bytes in memory
		if(pageFault){
			printf("Page fault for page number %d\n", pageNumber);
			readFromBackingStore(backingStore, bufferBacking, pageNumber);
			
			for(int i = 0; i<PAGE_SIZE; i++){
				physicalMemory[frameNumber * PAGE_SIZE + i] = bufferBacking[i];
			}
		}
		
		// Get the actual bytes from memory[frameNumber * PAGE_SIZE + PAGE_OFFSET]

		byteIndex = frameNumber * PAGE_SIZE + pageOffset;
		byteToFind = physicalMemory[byteIndex];
		// Print info in format
		if(pageFault){
			printf("* Virtual address: %d [%d, %d] Physical address: %d [%d, %d] Value: %d\n",
				virtualAddress, pageNumber, pageOffset, byteIndex,
				frameNumber, pageOffset, byteToFind);
		}
		else {
			printf("  Virtual address: %d [%d, %d] Physical address: %d [%d, %d] Value: %d\n",
				virtualAddress, pageNumber, pageOffset, byteIndex,
				frameNumber, pageOffset, byteToFind);
		}
		// Increment Access time
		printf("accessTime: %d\n", accessTime);
		// Read next address
		chp = fgets(bufferAddress, BUFLEN, addresses);
	}
	fclose(addresses);
	fclose(backingStore);



	return 0;
}