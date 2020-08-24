#ifndef GC_H
#define GC_H

void *stackBottom();
void gc();
void mark(int *p);
void sweep();


#endif
