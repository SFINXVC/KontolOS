/*
 * KontolOS Memory Manager Header
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "../include/types.h"

/* Initialize memory manager */
void memory_init(void);

/* Memory allocation */
void *kmalloc(size_t size);
void *kcalloc(size_t num, size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

/* Memory statistics */
size_t memory_get_total(void);
size_t memory_get_used(void);
size_t memory_get_free(void);

/* Memory manipulation */
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memmove(void *dest, const void *src, size_t n);

#endif /* MEMORY_H */
