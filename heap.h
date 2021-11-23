//
// Created by jnwyk on 19.11.2021.
//

#ifndef PROJECT1_HEAP_H
#define PROJECT1_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "custom_unistd.h"

#define FENCE 8

struct memory_manager_t
{
    void *memory_start;
    size_t memory_size;
    struct memory_chunk_t *first_memory_chunk;
    int setup; // initialized = 1, not initialize = 0
}memory_manager;

struct memory_chunk_t
{
    struct memory_chunk_t* prev;
    struct memory_chunk_t* next;
    size_t size;
    int free; // occupied - 0, free - 1
};

enum pointer_type_t
{
    pointer_null,
    pointer_heap_corrupted,
    pointer_control_block,
    pointer_inside_fences,
    pointer_inside_data_block,
    pointer_unallocated,
    pointer_valid
};


int heap_setup(void);
void heap_clean(void);

void* heap_malloc(size_t size);
void* heap_calloc(size_t number, size_t size);
void* heap_realloc(void* memblock, size_t count);
void  heap_free(void* memblock);

size_t heap_get_largest_used_block_size(void);
enum pointer_type_t get_pointer_type(const void* const pointer);
int heap_validate(void);

void* heap_malloc_aligned(size_t count);
void* heap_calloc_aligned(size_t number, size_t size);
void* heap_realloc_aligned(void* memblock, size_t size);


void* first_chunk(size_t size);
int increase_memory (size_t mem);
void set_fence(struct memory_chunk_t *chunk);
void* create_new_chunk(struct memory_chunk_t *prev, size_t size);
void display_info();
void check_and_merge(struct memory_chunk_t* chunk);
int check_fences();
void* insert_into_chunk(struct memory_chunk_t* chunk, size_t size);

#endif //PROJECT1_HEAP_H
