#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>

#define STACK_IMPL(type)                                                            \
typedef struct {                                                                    \
    type *stack;                                                                    \
    uint32_t cap;                                                                   \
    uint32_t size;                                                                  \
} Stack_##type;                                                                     \
                                                                                    \
static inline Stack_##type *newStack_##type(uint32_t cap){                          \
    Stack_##type *stack = malloc(sizeof(Stack_##type));                             \
    stack->stack = calloc(cap, sizeof(type));                                       \
    stack->cap = cap;                                                               \
    stack->size = 0;                                                                \
    return stack;                                                                   \
}                                                                                   \
                                                                                    \
static inline type popStack_##type(Stack_##type *stack){                            \
    if(stack->size == 0) return (type)0;                                            \
    return *(stack->stack + --stack->size);                                         \
}                                                                                   \
                                                                                    \
static inline void pushStack_##type(Stack_##type *stack, type val){                 \
    if(stack->size >= stack->cap){                                                  \
        return;                                                                     \
    }                                                                               \
    *(stack->stack + stack->size++) = val;                                          \
}                                                                                   \
                                                                                    \
static inline type peekStack_##type(Stack_##type *stack){                           \
    if(stack->size == 0) return (type)0;                                            \
    return *(stack->stack + stack->size - 1);                                       \
}                                                                                   \
                                                                                    \
static inline void dupStack_##type(Stack_##type *stack){                            \
    if(stack->size == 0) return;                                                    \
    pushStack_##type(stack, peekStack_##type(stack));                               \
}                                                                                   \
                                                                                    \
static inline void freeStack_##type(Stack_##type *stack){                           \
    free(stack->stack);                                                             \
    free(stack);                                                                    \
}

#define STACK_BIN_OP(stck, op) stck->stack[stck->size - 2] = (stck->stack[stck->size - 2]) op (stck->stack[stck->size - 1]); stck->size--;

#define STACK_BIN_OP_REINTERP(stck, op, type) (*(type*)&stck->stack[stck->size - 2]) = (*(type*)&stck->stack[stck->size - 2]) op (*(type*)&stck->stack[stck->size - 1]); stck->size--;


#endif