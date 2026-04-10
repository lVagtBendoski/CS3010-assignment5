#include <stdio.h>
#include <stdlib.h>
#define PAGE_SIZE 256
#define NUM_PAGES 256
// HEY JASON. We are working together on this assignment. 
//Luke Vagt-Bendoski and Oliver Baker
//:)

int decodeAddress(int address, int *pageNumber, int *pageOffset){

	if(0 > address || 256*256-1 < address)
		return 1;

	*pageNumber = address / NUM_PAGES; //the page number
	*pageOffset = address % PAGE_SIZE; //the offset

	return 0;

};


int readFromBackingStore(FILE *fp, char *buffer, int pageNumber){

	if(fp == NULL)
		return 1; // bad file pointer
	
	int n = PAGE_SIZE * pageNumber;

	if(fseek(fp, 0, n) != 0)
		return 1; // error during seek


	if(fread(buffer, sizeof(char), PAGE_SIZE, fp) != PAGE_SIZE) {
		return 1; // error during read
	}
	
	return 0; // no errors :)

};