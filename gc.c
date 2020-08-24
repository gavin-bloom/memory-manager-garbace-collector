#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "memorymgr.h"

void *stackBottom();
void gc();
void mark(int *p);
void sweep();


void gc() {
  int **botStack = (int **) stackBottom(); // the address of the bottom of the stack
  int *dummy;
  int **topStack = &dummy; // the address of the top of the stack
  //mark
  while(topStack <= botStack) {
    mark(*topStack);
    topStack++;
  }
  sweep();
  coalesce();
}

void mark(int *p) {
  int *first = firstBlock();
  int *last = lastBlock();
  //return if p is not an address in heap/points to heap
  if((p < first) || (p - 1 > last)) {
    return;
  }
  //use next block to determine block of heap that p is in

  int *prev = first;
  while(p > nextBlock(prev)) {
    prev = nextBlock(prev);
  }
  //check if block is allocated and mark
  if(isAllocated(prev) == 1) {
    *(prev-1) = *(prev-1) + 2;
  }
  //turn rest of block into pointers and call mark
  int userSize = (*(prev-1) / 4) - 1;
  int *ptr;
  int i;
  for (i =  0; i < userSize; i++) {
    ptr = (int*)(long)(*(prev + i));
    mark(ptr);
  }
  return;
}

void sweep() {
  int *block = firstBlock();
  while (block -1 != lastBlock()) {
    if((*(block-1) % 4 != 3) && (*(block - 1) % 2 == 1)) {
      myfree(block);
    }
    block = nextBlock(block);
  }
  return;
}





void *stackBottom() {
	unsigned long bottom;
    FILE *statfp = fopen("/proc/self/stat", "r");
    fscanf(statfp,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u "
           "%*u %*u %*u %*u %*u %*u %*d %*d "
           "%*d %*d %*d %*d %*u %*u %*d "
           "%*u %*u %*u %lu", &bottom);
    fclose(statfp);
	return (void *) bottom;
}
