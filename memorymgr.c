#include "memorymgr.h"
#include <stdio.h>
#include <stdlib.h>

void  initmemory(int size);
void *myalloc(int length);
void  myfree(void *ptr);
void  coalesce();
void  printallocation();

int  isAllocated(int *p);  // is the block at location p allocated?
int *nextBlock(int *p);    // return a pointer to the block that follows p
int *firstBlock();         // return a pointer to the first block on the heap
int *lastBlock();          // return a pointer to the sentinel block
int formatBlockSize(int size);

int *sentinel;
int *first;
int bit32BlockSize = 8;
int mask =   0x0001;

int *firstBlock() { return first; }
int *lastBlock() { return sentinel; }

int formatBlockSize(int size) {
  int temp = (size + 4) % 8;
  if (temp == 0) { return size + 4; }
  else { return size + 4 + 8 - temp; }
}

void initmemory(int size) {
  int userBlockSize = formatBlockSize(size); 
  int bytes = userBlockSize + 4 + 4;
  int *p = (int *) malloc(bytes);
  *(p + 1) = userBlockSize;
  first = p + 2; //points to just after first header
  sentinel = p + 1 + (userBlockSize / 4);
  *sentinel = 0;
}

//takes in pointer to first user word  of block, then goes back to the header
//returns pointer to first word
int *nextBlock(int *p) {
  int *next;
  int *header = p - 1;
  next = p + (((*header | mask) - 1) / 4);
  return next;
}

//returns 1 if true
int isAllocated(int *p) { return *(p - 1) & mask; }

void *myalloc(int length) {
  int *block = firstBlock();
  length = formatBlockSize(length);
  while ((isAllocated(block) != 0) || (*(block - 1) < length)) {
    if (block - 1 == lastBlock()) {return NULL;}
	block = nextBlock(block);
  }
  int totalLength = *(block - 1);
  *(block - 1) = length | mask;
  *(block - 1 + (length / 4)) = totalLength - length;
  return block;
}

void myfree(void *ptr) {
    int *ptr2 = (int *) ptr;
   *(ptr2 - 1) = *(ptr2 - 1) - 1; 
 }

void coalesce() {
  int *prev = firstBlock();
  int prevLength = *(prev - 1); //with allocated flag still there
  int length;
  int *block = nextBlock(prev);
  while (block - 1 != lastBlock()) {
    length = *(block - 1);
    if ((prevLength % 2 == 0) && (length %2 == 0)) {
      prevLength = prevLength + length;
      *(prev - 1) = prevLength;
      block = nextBlock(block);
    }
    else {
      prev = block;
      block = nextBlock(block);
      prevLength = length;
    }
  }
}

void printallocation() {
  int *block = firstBlock();
  int count = 1;
  int size;
  while (block - 1 != lastBlock()) {
    if (*(block - 1) % 2 == 0) {
      size = *(block -1);
      printf("Block %d is of size %d and it is unallocated\n", count, size);
    }
    else {
      size = *(block -1) - 1;
      printf("Block %d is of size %d and it is allocated\n", count, size);
    }
    count ++;
    block = nextBlock(block);
  }
}
