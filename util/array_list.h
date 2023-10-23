#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdlib.h>
#include <stdint.h>

#define ARRAY_LIST_RESIZE_FACTOR 2
#define ARRAY_LIST_IMPL(type)                                                       \
typedef struct {                                                                    \
    type *arr;                                                                      \
    uint32_t cap;                                                                   \
    uint32_t size;                                                                  \
} ArrayList_##type;                                                                 \
                                                                                    \
static inline ArrayList_##type *newArrayList_##type(uint32_t initCap){              \
    ArrayList_##type *list = malloc(sizeof(ArrayList_##type));                      \
    list->arr = calloc(initCap, sizeof(type));                                      \
    list->cap = initCap;                                                            \
    list->size = 0;                                                                 \
    return list;                                                                    \
}                                                                                   \
                                                                                    \
static inline type getArrayList_##type(ArrayList_##type *list, uint32_t index){     \
    if(index >= list->size) return (type)0;                                         \
                                                                                    \
    return *(list->arr + index);                                                    \
}                                                                                   \
                                                                                    \
static inline uint8_t appendArrayList_##type(ArrayList_##type *list, type val){     \
    if(list->size >= list->cap){                                                    \
        type *arr = realloc(list->arr, list->cap * ARRAY_LIST_RESIZE_FACTOR);       \
        if(arr == NULL) return 0;                                                   \
        list->cap = list->cap * ARRAY_LIST_RESIZE_FACTOR;                           \
        list->arr = arr;                                                            \
    }                                                                               \
    *(list->arr + list->size++) = val;                                              \
    return 1;                                                                       \
}                                                                                   \
                                                                                    \
static inline void freeArrayList_##type(ArrayList_##type *list){                    \
    free(list->arr);                                                                \
    free(list);                                                                     \
}

#endif
