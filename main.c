#include <stdio.h>
#include "heap.h"

int main() {
//    srand (time(NULL));
//
//    int status = heap_setup();
//
//    char *ptr[2611];
//    int ptr_state[2611] = {0};
//
//    int is_allocated = 0;
//
//    for (int i = 0; i < 2611; ++i)
//    {
//        int rand_value = rand() % 100;
//        if (rand_value < 11)
//        {
//            for (int j = 0; j < 2611; ++j)
//                if (ptr_state[j] == 0)
//                {
//                    ptr_state[j] = 1;
//                    ptr[j] = heap_realloc_aligned(NULL, rand() % 1000 + 500);
//                    if(((intptr_t)ptr[j] & (intptr_t)(PAGE_SIZE - 1)) != 0)
//                        printf("Funkcja heap_realloc_aligned() powinien zwrĂłciÄ adres zaczynajÄcy siÄ na poczÄtku strony\n");
//                    is_allocated++;
//                    break;
//                }
//        }
//        else if (rand_value < 22)
//        {
//            for (int j = 0; j < 2611; ++j)
//                if (ptr_state[j] == 0)
//                {
//                    ptr_state[j] = 1;
//                    ptr[j] = heap_calloc_aligned(rand() % 1000 + 500, rand() % 4 + 1);
//                    if(((intptr_t)ptr[j] & (intptr_t)(PAGE_SIZE - 1)) != 0)
//                        printf("Funkcja heap_calloc_aligned() powinien zwrĂłciÄ adres zaczynajÄcy siÄ na poczÄtku strony\n");
//                    is_allocated++;
//                    break;
//                }
//        }
//        else if (rand_value < 33)
//        {
//            for (int j = 0; j < 2611; ++j)
//                if (ptr_state[j] == 0)
//                {
//                    ptr_state[j] = 1;
//                    ptr[j] = heap_malloc_aligned(rand() % 1000 + 500);
//                    if(((intptr_t)ptr[j] & (intptr_t)(PAGE_SIZE - 1)) != 0)
//                        printf("Funkcja heap_malloc_aligned() powinien zwrĂłciÄ adres zaczynajÄcy siÄ na poczÄtku strony\n");
//                    is_allocated++;
//                    break;
//                }
//        }
//        else if (rand_value < 44)
//        {
//            for (int j = 0; j < 2611; ++j)
//                if (ptr_state[j] == 0)
//                {
//                    ptr_state[j] = 1;
//                    ptr[j] = heap_realloc(NULL, rand() % 1000 + 500);
//                    is_allocated++;
//                    break;
//                }
//        }
//        else if (rand_value < 55)
//        {
//            for (int j = 0; j < 2611; ++j)
//                if (ptr_state[j] == 0)
//                {
//                    ptr_state[j] = 1;
//                    ptr[j] = heap_calloc(rand() % 1000 + 500, rand() % 4 + 1);
//                    is_allocated++;
//                    break;
//                }
//        }
//        else if (rand_value < 66)
//        {
//            for (int j = 0; j < 2611; ++j)
//                if (ptr_state[j] == 0)
//                {
//                    ptr_state[j] = 1;
//                    ptr[j] = heap_malloc(rand() % 1000 + 500);
//                    is_allocated++;
//                    break;
//                }
//        }
//        else if (rand_value < 77)
//        {
//            if (is_allocated)
//            {
//                int to_reallocate = rand() % is_allocated;
//                for (int j = 0; j < 2611; ++j)
//                {
//                    if (ptr_state[j] == 1 && !to_reallocate)
//                    {
//                        if (rand() % 100 < 50)
//                            ptr[j] = heap_realloc(ptr[j], rand() % 100 + 50);
//                        else
//                            ptr[j] = heap_realloc(ptr[j], rand() % 1000 + 500);
//                        break;
//                    }
//                    to_reallocate--;
//                }
//            }
//        }
//        else if (rand_value < 88)
//        {
//            if (is_allocated)
//            {
//                int to_free = rand() % is_allocated;
//                for (int j = 0; j < 2611; ++j)
//                {
//                    if (ptr_state[j] == 1 && !to_free)
//                    {
//                        ptr_state[j] = 0;
//                        is_allocated--;
//                        heap_realloc_aligned(ptr[j], 0);
//                        break;
//                    }
//                    to_free--;
//                }
//            }
//        }
//        else if (rand_value < 99)
//        {
//            if (is_allocated)
//            {
//                int to_free = rand() % is_allocated;
//                for (int j = 0; j < 2611; ++j)
//                {
//                    if (ptr_state[j] == 1 && !to_free)
//                    {
//                        ptr_state[j] = 0;
//                        is_allocated--;
//                        heap_realloc(ptr[j], 0);
//                        break;
//                    }
//                    to_free--;
//                }
//            }
//        }
//        else
//        {
//            if (is_allocated)
//            {
//                int to_free = rand() % is_allocated;
//                for (int j = 0; j < 2611; ++j)
//                {
//                    if (ptr_state[j] == 1 && !to_free)
//                    {
//                        ptr_state[j] = 0;
//                        is_allocated--;
//                        heap_free(ptr[j]);
//                        break;
//                    }
//                    to_free--;
//                }
//            }
//        }
//
//        status = heap_validate();
//
//    }
//
//    for (int j = 0; j < 2611; ++j)
//        if (ptr_state[j] == 1)
//            heap_realloc_aligned(ptr[j], 0);
//
//
//
//    heap_clean();

    return 0;
}


