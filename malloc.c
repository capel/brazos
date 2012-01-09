/*
 * Copyright (c) 2005-2007, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "malloc.h"
#include "kio.h"
#define PAGE_SIZE 4096

static struct header *more_core(size_t size);

static vm_data *d;

//static struct header* free_list;         /* start of free list */
//static struct header *scan_head;        /* start point to scan */

void set_vm_base(vm_data *_d) {
  d = _d;
}

vm_data* get_vm_base() {
  return d;
}

/*
 * Simple memory allocator from K&R
 */
void *
malloc(size_t size)
{
        struct header *p, *prev;

        if (size == 0)          /* sanity check */
                return NULL;
        size = ROUNDUP(size + sizeof(struct header));

        MALLOC_LOCK();

        if (d->scan_head == NULL) {
                /* Initialize */
                d->free_list.next = &d->free_list;
                d->free_list.size = 0;
                d->free_list.vm_size = 0;
                d->scan_head = &d->free_list;
        }
        prev = d->scan_head;
        for (p = prev->next;; prev = p, p = p->next) {
                if (p->size >= size) {  /* big enough */
                        if (p->size == size)    /* exactly */
                                prev->next = p->next;
                        else {                  /* allocate tail end */
                                p->size -= size;
                                p = (struct header *)((char *)p + p->size);
                                p->size = size;
                                p->vm_size = 0;
                        }
#ifdef DEBUG_MALLOC
                        p->magic = MALLOC_MAGIC;
#endif
                        d->scan_head = prev;
                        break;
                }
                if (p == d->scan_head) {
                        if ((p = more_core(10)) == NULL)
                                break;
                }
        }
        MALLOC_UNLOCK();

        if (p == NULL) {
#ifdef DEBUG_MALLOC
                panic("malloc: out of memory");
#endif
                return NULL;
        }
        return (void *)(p + 1);
}

void* kget_pages(size_t num);

/*
 * Create new block and insert it to the free list.
 */
static struct header *more_core(size_t pages)
{
        struct header *p, *prev;

        size_t size = pages * PAGE_SIZE;
        p = kget_pages(pages);
        p->size = size;
        p->vm_size = size;

        /* Insert to free list */
        for (prev = d->scan_head; !(p > prev && p < prev->next); prev = prev->next) {
                if (prev >= prev->next && (p > prev || p < prev->next))
                        break;
        }
        p->next = prev->next;
        prev->next = p;
        d->scan_head = prev;
        return prev;
}

void
free(void *addr)
{
        struct header *p, *prev;

        if (addr == NULL)
                return;

        MALLOC_LOCK();
        p = (struct header *)addr - 1;
#ifdef DEBUG_MALLOC
        if (p->magic != MALLOC_MAGIC)
                panic("free: invalid pointer");
        p->magic = 0;
#endif
        for (prev = d->scan_head; !(p > prev && p < prev->next); prev = prev->next) {
                if (prev >= prev->next && (p > prev || p < prev->next))
                        break;
        }
        if ((prev->next->vm_size == 0) &&       /* join to upper block */
            ((char *)p + p->size == (char *)prev->next)) {
                p->size += prev->next->size;
                p->next = prev->next->next;
        } else {
                p->next = prev->next;
        }
        if ((p->vm_size == 0) &&        /* join to lower block */
            ((char *)prev + prev->size == (char *)p)) {
                prev->size += p->size;
                prev->next = p->next;
        } else {
                prev->next = p;
        }
        /* Deallocate pool */
        if (p->size == p->vm_size) {
                prev->next = p->next;
        }
        d->scan_head = prev;
        MALLOC_UNLOCK();
}

#ifdef DEBUG_MALLOC
void
mstat(void)
{
        struct header *p;

        for (p = d->free_list.next; p != &d->free_list; p = p->next) {
                printk("mstat: addr=%x size=%d next=%x\n", p, p->size, p->next);
        }
}
#endif

void* calloc(size_t num, size_t size) {
  void* p = malloc(num * size);
  memset(p, 0, num * size);
  return p;
}
