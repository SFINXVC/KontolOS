/*
 * KontolOS Memory Manager
 */

#include "memory.h"
#include "kernel.h"
#include "vga.h"

/* Memory layout constants */
#define HEAP_START      0x200000    /* Start of heap at 2MB */
#define HEAP_SIZE       0x400000    /* 4MB heap */
#define HEAP_END        (HEAP_START + HEAP_SIZE)

/* Block header structure */
struct mem_block {
    size_t size;            /* Size of the block (including header) */
    bool used;              /* Is this block in use? */
    struct mem_block *next; /* Next block in the list */
};

/* Memory manager state */
static struct mem_block *heap_start = NULL;
static size_t total_memory = 0;
static size_t used_memory = 0;

/*
 * Initialize the memory manager
 */
void memory_init(void)
{
    /* Initialize heap with a single free block */
    heap_start = (struct mem_block *)HEAP_START;
    heap_start->size = HEAP_SIZE;
    heap_start->used = false;
    heap_start->next = NULL;

    total_memory = HEAP_SIZE;
    used_memory = 0;
}

/*
 * Allocate memory
 */
void *kmalloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    /* Align size to 8 bytes */
    size = (size + 7) & ~7;

    /* Total size including header */
    size_t total_size = size + sizeof(struct mem_block);

    /* Find a free block that fits */
    struct mem_block *block = heap_start;
    while (block != NULL) {
        if (!block->used && block->size >= total_size) {
            /* Found a suitable block */

            /* Split the block if it's much larger than needed */
            if (block->size >= total_size + sizeof(struct mem_block) + 16) {
                struct mem_block *new_block = (struct mem_block *)((uint8_t *)block + total_size);
                new_block->size = block->size - total_size;
                new_block->used = false;
                new_block->next = block->next;

                block->size = total_size;
                block->next = new_block;
            }

            block->used = true;
            used_memory += block->size;

            /* Return pointer to usable memory (after header) */
            return (void *)((uint8_t *)block + sizeof(struct mem_block));
        }
        block = block->next;
    }

    /* No suitable block found */
    return NULL;
}

/*
 * Allocate and zero memory
 */
void *kcalloc(size_t num, size_t size)
{
    size_t total = num * size;
    void *ptr = kmalloc(total);

    if (ptr != NULL) {
        memset(ptr, 0, total);
    }

    return ptr;
}

/*
 * Free memory
 */
void kfree(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    /* Get block header */
    struct mem_block *block = (struct mem_block *)((uint8_t *)ptr - sizeof(struct mem_block));

    if (!block->used) {
        /* Double free! */
        return;
    }

    block->used = false;
    used_memory -= block->size;

    /* Coalesce with next block if it's free */
    if (block->next != NULL && !block->next->used) {
        block->size += block->next->size;
        block->next = block->next->next;
    }

    /* Coalesce with previous block if it's free */
    struct mem_block *prev = heap_start;
    while (prev != NULL && prev->next != block) {
        prev = prev->next;
    }
    if (prev != NULL && !prev->used) {
        prev->size += block->size;
        prev->next = block->next;
    }
}

/*
 * Reallocate memory
 */
void *krealloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return kmalloc(size);
    }

    if (size == 0) {
        kfree(ptr);
        return NULL;
    }

    /* Get old block */
    struct mem_block *old_block = (struct mem_block *)((uint8_t *)ptr - sizeof(struct mem_block));
    size_t old_size = old_block->size - sizeof(struct mem_block);

    /* If new size fits in current block, just return */
    if (size <= old_size) {
        return ptr;
    }

    /* Allocate new block */
    void *new_ptr = kmalloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }

    /* Copy old data */
    memcpy(new_ptr, ptr, old_size);

    /* Free old block */
    kfree(ptr);

    return new_ptr;
}

/*
 * Get total memory size
 */
size_t memory_get_total(void)
{
    return total_memory;
}

/*
 * Get used memory size
 */
size_t memory_get_used(void)
{
    return used_memory;
}

/*
 * Get free memory size
 */
size_t memory_get_free(void)
{
    return total_memory - used_memory;
}

/*
 * Memory copy
 */
void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/*
 * Memory set
 */
void *memset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n--) {
        *p++ = (uint8_t)c;
    }

    return s;
}

/*
 * Memory compare
 */
int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }

    return 0;
}

/*
 * Memory move (handles overlapping regions)
 */
void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }

    return dest;
}
