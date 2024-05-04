/**
 * @file malloc.c
 * @provides malloc.c.
 * This file is deprecated in favor of mem/malloc.c and the user heap
 * allocator.  However, it remains here for backup purposes.
 *
 * $Id: malloc.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>
#include <kernel.h>

/**
 * Request heap storage, record accounting information, returning pointer
 * to assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, NULL on failure
 */


struct kmalloc_chunk *head = 0;

void malloc_init(void *start, int length)
{
  head = (struct kmalloc_chunk *) start;
  head->state = KMALLOC_STATE_FREE;
  head->length = length;
  head->next = 0;
  head->prev = 0;
  
}

static void ksplit(struct kmalloc_chunk *c, int length)
{
 // uint32 mask= disable();

  struct kmalloc_chunk *n = (struct kmalloc_chunk *) ((char *) c + length);

  n->state = KMALLOC_STATE_FREE;
  n->length = c->length - length;
  n->prev = c;
  n->next = c->next;

  if(c->next)
    c->next->prev = n;

  c->next = n;
  c->length = length;
  //restore(mask);
}

 

void *malloc(uint32 length)
{
  // round up length to a multiple of KUNIT
  int extra = length % KUNIT;
  uint32 mask= disable();

  if(extra)
    length += (KUNIT - extra);

  // then add one more unit to accommodate the chunk header
  length += KUNIT;

  struct kmalloc_chunk *c = head;

  while(1) {
    if(!c) {
      kprintf("kmalloc: out of memory!\n");
      restore(mask);
      return NULL;
    }
    if(c->state == KMALLOC_STATE_FREE && c->length >= length)
      break;
    c = c->next;
  }

  // split the chunk if the remainder is greater than two units
  if(length - c->length > 2 * KUNIT) {
    ksplit(c, length);
  }

  c->state = KMALLOC_STATE_USED;

  restore(mask);
  // return a pointer to the memory following the chunk header
  return (c + 1);
}

 
static void kmerge(struct kmalloc_chunk *c)
{

 // uint32 mask= disable();
  if(!c){
    // restore(mask);
     return;
  }

  if(c->state != KMALLOC_STATE_FREE){
  //    restore(mask);
    return;
  }

  if(c->next && c->next->state == KMALLOC_STATE_FREE) {
    c->length += c->next->length;
    if(c->next->next) {
      c->next->next->prev = c;
    }
    c->next = c->next->next;
  }
 // restore(mask);
}

 
void free(void *ptr)
{

  uint32 mask= disable();
  struct kmalloc_chunk *c = (struct kmalloc_chunk *) ptr;
  c--;

  if(c->state != KMALLOC_STATE_USED) {
    kprintf("invalid kfree(%x)\n", ptr);
    restore(mask);
    return;
  }

  c->state = KMALLOC_STATE_FREE;

  kmerge(c);
  kmerge(c->prev);
  restore(mask);
}

void malloc_debug()
{
   uint32 mask= disable();
   struct kmalloc_chunk *c;

  

  //kprintf("state ptr      prev     next     length  ptr(%x)\n",KMALLOC_START);

  for(c = head; c; c = c->next) {
    if(c->state == KMALLOC_STATE_FREE) {
      kprintf("Free");
    } else if(c->state == KMALLOC_STATE_USED) {
      kprintf("Used");
    } else {
      kprintf("kmalloc list corrupted at %x!\n", c);
      return;
    }
    //kprintf("  %x         %d       %x       %x       %d\n", c,c, c->prev, c->next, c->length);
    kprintf("\t%x ", c);
    kprintf("\t%d bytes", c->length);
    kprintf("\n");
  }
  restore(mask);
}

  
void testmem(){
char *a[11];   
for (int i = 0; i < 10; ++i)
{
 a[i]=malloc(2000);
}

malloc_debug();


for (int i = 0; i < 10; ++i)
{
    free(a[i]);
}

malloc_debug();
}