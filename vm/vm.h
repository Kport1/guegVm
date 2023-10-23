#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "../util/stack.h"
#include "../file/file_reader.h"

typedef struct {
    uint8_t code;
} InterpResult;

typedef struct {
    uint8_t success : 1;
} LoadGFileResult;

STACK_IMPL(uint8_t)
STACK_IMPL(uint16_t)
STACK_IMPL(uint32_t)
STACK_IMPL(uint64_t)

typedef float f32;
STACK_IMPL(f32)

typedef double f64;
STACK_IMPL(f64)

typedef struct {
    Stack_uint8_t  *stack8;
    Stack_uint16_t *stack16;
    Stack_uint32_t *stack32;
    Stack_uint64_t *stack64;

    uint8_t  *local8;
    uint16_t *local16;
    uint32_t *local32;
    uint64_t *local64;

    uint8_t *ip;

    uint32_t gfileIndex;

    uint8_t retType;
} StackFrame;
typedef StackFrame *PStackFrame;
STACK_IMPL(PStackFrame)

typedef struct {
    ArrayList_PGuegFile *loadedGFiles;
    Stack_PStackFrame *callStack;
} Vm;

Vm *newVm();
void freeVm(Vm *vm);

LoadGFileResult loadGFile(Vm *vm, GuegFile *gfile);

void callFunction(Vm *vm, uint32_t gfileIndex, uint16_t index);
void freeStackFrame(StackFrame *frame);

InterpResult execute(Vm *vm);


#endif