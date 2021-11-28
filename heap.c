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
//    memory_manager.tail = NULL;
    return 0;
}

void heap_clean(void){
    custom_sbrk(memory_manager.memory_size * (-1));
    memory_manager.memory_size = 0;
    memory_manager.memory_start = NULL;
    memory_manager.setup = 0;
    memory_manager.first_memory_chunk = NULL;
//    memory_manager.tail = NULL;
    return;
}

void* heap_malloc(size_t size){
    if(size == 0 || heap_validate() != 0)
        return NULL;

    if(memory_manager.first_memory_chunk == NULL){
        return first_chunk(size);
    }
    else{
        struct memory_chunk_t *temp = memory_manager.first_memory_chunk;
        while(1){
            if((size + (2 * FENCE)) <= temp->size && temp->free == 1){
                return insert_into_chunk(temp, size);
            }
            if(temp->next == NULL)
                break;
            temp = temp->next;
        }
        if(temp->free == 1){
            return extend_and_insert_chunk(temp, size);
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
    if(heap_validate() != 0)
        return NULL;
    if(memblock == NULL){
        return heap_malloc(count);
    }
    if(get_pointer_type(memblock) != pointer_valid){
        return NULL;
    }
    if(count == 0){
        heap_free(memblock);
        return NULL;
    }

    struct memory_chunk_t* temp = (struct memory_chunk_t*)((unsigned char*)memblock - sizeof(struct memory_chunk_t) - FENCE);
    struct memory_chunk_t* last = find_last_chunk();

    if(temp == last){
        return realloc_extend_and_insert_chunk(last, count - last->size);
    }
    if(temp->size == count)
        return memblock;
    else if(temp->size > count){
        return realloc_at_place(temp, count);
    }
    else{
        if(((unsigned char*)temp->next - (unsigned char*)temp) >= (long)(sizeof(struct memory_chunk_t) + (2 * FENCE) + count)){
            return realloc_at_place(temp, count);
        }
        else if(temp->next->next != NULL){
            if(temp->next->free == 1 && ((unsigned char*)temp->next->next - (unsigned char*)temp) >= (long)(sizeof(struct memory_chunk_t) + (2 * FENCE) + count))
                return realloc_by_merging(temp, count);
            else
                return resize_by_increasing_memory(temp, count);
        }
        else{
            return resize_by_increasing_memory(temp, count);
        }
    }
}

void heap_free(void* memblock){
    if(get_pointer_type(memblock) != pointer_valid)
        return;
    struct memory_chunk_t* chunk = (struct memory_chunk_t*)((unsigned char*)memblock - FENCE - sizeof(struct memory_chunk_t));
    chunk->free = 1;
    chunk = check_and_merge(chunk);
    chunk->checksum = checksum_count(chunk);
//    if(memory_manager.first_memory_chunk == chunk && chunk->next == NULL)
//        memory_manager.first_memory_chunk = NULL;
}

void* check_and_merge(struct memory_chunk_t* chunk){
    int flag = 0;
    if(chunk->next != NULL){
        if(chunk->next->free == 1){
            chunk->size = (unsigned char*)chunk->next - (unsigned char*)chunk + chunk->next->size + FENCE;
            chunk->next = chunk->next->next;
            if(chunk->next != NULL)
                chunk->next->prev = chunk;
            flag = 1;
        }
    }
    if(chunk->prev != NULL){
        if(chunk->prev->free == 1){
            size_t temp_size;
            temp_size = (unsigned char*)chunk - (unsigned char*)chunk->prev + chunk->size + FENCE;
            chunk = chunk->prev;
            if(chunk->next->next != NULL)
                chunk->next->next->prev = chunk;
            chunk->next = chunk->next->next;
            chunk->size = temp_size;
            flag = 1;
        }
    }
    if(flag == 0){
        if(chunk->next != NULL)
            chunk->size = (unsigned char*)chunk->next - (unsigned char*)chunk - sizeof(struct memory_chunk_t);
    }
    if(chunk->next != NULL)
        chunk->next->checksum = checksum_count(chunk->next);
    if(chunk->prev != NULL)
        chunk->prev->checksum = checksum_count(chunk->prev);
    return chunk;
}

size_t heap_get_largest_used_block_size(void){
    if(check_fences() != 0 || memory_manager.setup == 0 || memory_manager.first_memory_chunk == NULL)
        return 0;
    struct memory_chunk_t* temp = memory_manager.first_memory_chunk;
    size_t max_size = 0;
    while(temp != NULL){
        if(max_size < temp->size && temp->free == 0)
            max_size = temp->size;
        temp = temp->next;
    }
    return max_size;
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
    if(checksum_check() != 0)
        return 3;
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
    first->checksum = checksum_count(first);
//    memory_manager.tail = first;
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
    new_chunk->checksum = checksum_count(new_chunk);
    prev->checksum = checksum_count(prev);
//    memory_manager.tail = new_chunk;
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
    chunk->checksum = checksum_count(chunk);
    return (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
}

void* extend_and_insert_chunk(struct memory_chunk_t* chunk, size_t size){
    if(increase_memory(((sizeof(struct memory_chunk_t) + (2 * FENCE) + size) - chunk->size)) == -1)
        return NULL;
    chunk->size = size;
    chunk->free = 0;
    set_fence(chunk);
    chunk->checksum = checksum_count(chunk);
    return (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
}

unsigned int checksum_count(struct memory_chunk_t *chunk){
    unsigned int sum = 0;
    unsigned char* pointer = (unsigned char*)chunk;
    for(int i = 0; i < (int)(sizeof(struct memory_chunk_t) - sizeof(unsigned int)); i++){
        sum += *(pointer + i);
    }
    return sum;
}

int checksum_check(){
    struct memory_chunk_t* temp = memory_manager.first_memory_chunk;
    while(temp != NULL){
        if(temp->checksum != checksum_count(temp)){
            return -1;
        }
        temp = temp->next;
    }
    return 0;
}

void* realloc_at_place(struct memory_chunk_t* chunk, size_t new_size){
    chunk->size = new_size;
    set_fence(chunk);
    chunk->checksum = checksum_count(chunk);
    return (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
}

void* realloc_by_merging(struct memory_chunk_t* chunk, size_t new_size){
    chunk->size = new_size;
    chunk->next = chunk->next->next;
    chunk->next->prev = chunk;
    set_fence(chunk);
    chunk->checksum = checksum_count(chunk);
    chunk->next->checksum = checksum_count(chunk->next);
    return (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
}

void* resize_by_increasing_memory(struct memory_chunk_t* chunk, size_t size){
    void* new_chunk = create_new_chunk(find_last_chunk(), size);
    if(new_chunk == NULL)
        return NULL;

    void* old_chunk = (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
    memcpy(new_chunk, old_chunk, chunk->size);
    heap_free(old_chunk);
    return new_chunk;
}

void* realloc_extend_and_insert_chunk(struct memory_chunk_t* chunk, size_t size){
    if(increase_memory(size) == -1)
        return NULL;
    chunk->size = size + chunk->size;
    chunk->free = 0;
    set_fence(chunk);
    chunk->checksum = checksum_count(chunk);
    return (unsigned char*)chunk + sizeof(struct memory_chunk_t) + FENCE;
}

void* find_last_chunk(){
    struct memory_chunk_t* temp = memory_manager.first_memory_chunk;
    while(temp->next != NULL){
        temp = temp->next;
    }
    return temp;
}
