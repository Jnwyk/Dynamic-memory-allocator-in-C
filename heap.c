//
// Created by jnwyk on 19.11.2021.
//

#include "heap.h"


int heap_setup(void){
    memory_manager.memory_start = custom_sbrk(0);
    if(memory_manager.memory_start == (void*) -1)
        return -1;
    memory_manager.memory_size = 0;
    memory_manager.setup = 1;
    memory_manager.first_memory_chunk = NULL;
    return 0;
}

void heap_clean(void){
    custom_sbrk(memory_manager.memory_size * (-1));
    memory_manager.memory_size = 0;
    memory_manager.memory_start = NULL;
    memory_manager.setup = 0;
    memory_manager.first_memory_chunk = NULL;
    return;
}

void* heap_malloc(size_t size){
    if(size <= 0 || heap_validate() != 0)
        return NULL;

    if(memory_manager.first_memory_chunk == NULL){
        return first_chunk(size);
    }
    else{
        struct memory_chunk_t *temp = memory_manager.first_memory_chunk;
        while(temp->next != NULL){
            if((size + (2 * FENCE)) <= temp->size && temp->free == 1){
                return insert_into_chunk(temp, size);
            }
            temp = temp->next;
        }
        return create_new_chunk(temp, size);
    }
}

void* heap_calloc(size_t number, size_t size){
    if(number <= 0 || size <= 0)
        return NULL;
    void* chunk = heap_malloc(number * size);
    if(chunk == NULL)
        return NULL;
    memset(chunk, 0, number * size);
    return chunk;
}

void* heap_realloc(void* memblock, size_t count){
    return NULL;
}

void heap_free(void* memblock){ //add clearing freed memory from the end
    if(memblock == NULL && heap_validate() != 0)
        return;
    struct memory_chunk_t* chunk = (struct memory_chunk_t*)((unsigned char*)memblock - FENCE - sizeof(struct memory_chunk_t));
    chunk->free = 1;
    chunk->size += 2 * FENCE;
    check_and_merge(chunk);
}

size_t heap_get_largest_used_block_size(void){
    return 0;
}

enum pointer_type_t get_pointer_type(const void* const pointer){
    if(pointer == NULL)
        return pointer_null;
    if(heap_validate() != 0)
        return pointer_heap_corrupted;
    struct memory_chunk_t* temp = memory_manager.first_memory_chunk;
    enum pointer_type_t location = pointer_unallocated;
    while(temp != NULL){
        if(((unsigned char*)temp + sizeof(struct memory_chunk_t) + FENCE) - (unsigned char*)pointer == 0) { //pointer valid
            location =  pointer_valid;
            break;
        }
        if(((unsigned char*)pointer - (unsigned char*)temp < (long int)sizeof(struct memory_chunk_t)) //pointer in control block
                && (((unsigned char*)pointer - (unsigned char*)temp) >= 0)) {
            location = pointer_control_block;
            break;
        }
        if(((unsigned char*)pointer - (unsigned char*)temp < (long int)(sizeof(struct memory_chunk_t) + FENCE)) //pointer inside fences
                && ((unsigned char*)pointer - (unsigned char*)temp >= (long int)sizeof(struct memory_chunk_t))) {
            location = pointer_inside_fences;
            break;
        }
        if(((unsigned char*)pointer - (unsigned char*)temp < (long int)(sizeof(struct memory_chunk_t) + temp->size + (2 * FENCE))) //pointer inside fences
                && ((unsigned char*)pointer - (unsigned char*)temp >= (long int)(sizeof(struct memory_chunk_t) + temp->size + FENCE))) {
            location = pointer_inside_fences;
            break;
        }
        if(((unsigned char*)pointer - (unsigned char*)temp < (long int)(sizeof(struct memory_chunk_t) + temp->size + FENCE)) //pointer in data block
                && ((unsigned char*)pointer - (unsigned char*)temp >= (long int)(sizeof(struct memory_chunk_t) + FENCE) + 1)) {
            location = pointer_inside_data_block;
            break;
        }
        temp = temp->next;
    }
    if(location != pointer_unallocated){
        if(temp->free == 0)
            return location;
    }
    return pointer_unallocated;
}

int heap_validate(void){
    if(memory_manager.setup == 0)
        return 2;
    if(check_fences() != 0)
        return 1;
    return 0;
}

void* heap_malloc_aligned(size_t count){
    return NULL;
}

void* heap_calloc_aligned(size_t number, size_t size){
    return NULL;
}

void* heap_realloc_aligned(void* memblock, size_t size){
    return NULL;
}

int increase_memory(size_t new_memory) {
    void *new_size = custom_sbrk(new_memory);
    if (new_size == (void *) -1)
        return -1;
    memory_manager.memory_size += new_memory;
    return 0;
}

void* first_chunk(size_t size){
    if(increase_memory(size + (2 * FENCE) + sizeof(struct memory_chunk_t)) == -1)
        return NULL;
    struct memory_chunk_t* first = memory_manager.memory_start;
    memory_manager.first_memory_chunk = first;
    first->prev = NULL;
    first->next = NULL;
    first->size = size;
    first->free = 0;
    set_fence(first);
    return (unsigned char*)first + sizeof(struct memory_chunk_t) + FENCE;
}

void set_fence(struct memory_chunk_t *chunk){
    memset((unsigned char*)chunk + sizeof(struct memory_chunk_t), '#', FENCE);
    memset((unsigned char*)chunk + sizeof(struct memory_chunk_t) + chunk->size + FENCE, '#', FENCE);
}

void* create_new_chunk(struct memory_chunk_t *prev, size_t size){
    if(increase_memory(size + (2 * FENCE) + sizeof(struct memory_chunk_t)) == -1)
        return NULL;
    struct memory_chunk_t *new_chunk =
            (struct memory_chunk_t*)((unsigned char*)prev + sizeof(struct memory_chunk_t) + prev->size + (2 * FENCE));
    prev->next = new_chunk;
    new_chunk->prev = prev;
    new_chunk->next = NULL;
    new_chunk->size = size;
    new_chunk->free = 0;
    set_fence(new_chunk);
    return (unsigned char*)new_chunk + sizeof(struct memory_chunk_t) + FENCE;
}

void display_info(){
    struct memory_chunk_t* temp = memory_manager.first_memory_chunk;
    for(int i = 1; temp; temp = temp->next, i++){
        printf("\nBlock %d:\n\tsize: %lu\n\t", i, temp->size);
        if(temp->free == 0)
            printf("condition: OCCUPIED");
        else
            printf("condition: FREE");
    }
}

void check_and_merge(struct memory_chunk_t* chunk){
    if(chunk->next != NULL){
        if(chunk->next->free == 1){
            chunk->size += chunk->next->size + sizeof(struct memory_chunk_t);
            chunk->next = chunk->next->next;
            chunk->next->prev = chunk;
        }
    }
    if(chunk->prev != NULL){
        if(chunk->prev->free == 1){
            chunk->prev->size += chunk->size + sizeof(struct memory_chunk_t);
            chunk->prev->next = chunk->next;
            chunk = chunk->prev;
            if(chunk->next != NULL)
                chunk->next->prev = chunk;
        }
    }
    if(chunk->prev == NULL)
        memory_manager.first_memory_chunk = chunk;
}

int check_fences(){
    struct memory_chunk_t* temp = memory_manager.first_memory_chunk;
    while(temp != NULL){
        void *check_start = (unsigned char*)temp + sizeof(struct memory_chunk_t);
        void *check_end = (unsigned char*)temp + sizeof(struct memory_chunk_t) + temp->size + FENCE;
        if(temp->free == 0){
            if (memcmp(check_start, "########", 8) != 0 || memcmp(check_end, "########", 8) != 0)
                return -1;
        }
        temp = temp->next;
    }
    return 0;
}

void* insert_into_chunk(struct memory_chunk_t* chunk, size_t size){
    chunk->size = size;
    chunk->free = 0;
    set_fence(chunk);
    return (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
}
