
#define PAGE_SIZE 256
#define NUM_PAGES 256
#define NUM_FRAMES 64
typedef struct {
int pageTable[NUM_PAGES]; // the actual page table
int accessTime[NUM_FRAMES]; // the most recent time that a frame was accessed
unsigned char freeFrame[NUM_FRAMES]; // 0 if a frame is occupied; otherwise 1
} PageTableInfo;
