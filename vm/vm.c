#include "vm.h"

#include "instructions.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

Vm *newVm(){
    Vm *vm = malloc(sizeof(Vm));
    vm->loadedGFiles = newArrayList_PGuegFile(8);
    vm->callStack = newStack_PStackFrame(UINT16_MAX);

    return vm;
}

void freeVm(Vm *vm){
    freeArrayList_PGuegFile(vm->loadedGFiles);
    freeStack_PStackFrame(vm->callStack);
    free(vm);
}

LoadGFileResult loadGFile(Vm *vm, GuegFile *gfile){
    LoadGFileResult res = {1};

    res.success &= appendArrayList_PGuegFile(vm->loadedGFiles, gfile);

    return res;
}

void callFunction(Vm *vm, uint32_t gfileIndex, uint16_t index){
    GuegFile *gfile = getArrayList_PGuegFile(vm->loadedGFiles, gfileIndex);
    StackFrame *prevFrame = peekStack_PStackFrame(vm->callStack);

    StackFrame *frame = malloc(sizeof(StackFrame));

    FnInfo info = gfile->functions[index];

    frame->stack8  = newStack_uint8_t(info.maxStackDepth8);
    frame->stack16 = newStack_uint16_t(info.maxStackDepth16);
    frame->stack32 = newStack_uint32_t(info.maxStackDepth32);
    frame->stack64 = newStack_uint64_t(info.maxStackDepth64);

    frame->local8  = calloc(info.numLocals8, sizeof(uint8_t));
    frame->local16 = calloc(info.numLocals16, sizeof(uint16_t));
    frame->local32 = calloc(info.numLocals32, sizeof(uint32_t));
    frame->local64 = calloc(info.numLocals64, sizeof(uint64_t));

    frame->ip = info.code;

    frame->gfileIndex = gfileIndex;

    frame->retType = info.returnSize;

    pushStack_PStackFrame(vm->callStack, frame);

    uint8_t local8ParamCount = 0;
    uint8_t local16ParamCount = 0;
    uint8_t local32ParamCount = 0;
    uint8_t local64ParamCount = 0;
    for(uint8_t i = 0; i < info.paramSizesLen; i++){
        switch(info.paramSizes[i]){
            case 0: break;
            case 1:
                frame->local8[local8ParamCount++] = popStack_uint8_t(prevFrame->stack8);
                break;
            case 2:
                frame->local16[local16ParamCount++] = popStack_uint16_t(prevFrame->stack16);
                break;
            case 3:
                frame->local32[local32ParamCount++] = popStack_uint32_t(prevFrame->stack32);
                break;
            case 4:
                frame->local64[local64ParamCount++] = popStack_uint64_t(prevFrame->stack64);
                break;
        }
    }
}

void freeStackFrame(StackFrame *frame){
    freeStack_uint8_t(frame->stack8);
    freeStack_uint16_t(frame->stack16);
    freeStack_uint32_t(frame->stack32);
    freeStack_uint64_t(frame->stack64);

    free(frame->local8);
    free(frame->local16);
    free(frame->local32);
    free(frame->local64);

    free(frame);
}

InterpResult execute(Vm *vm){
    InterpResult res = { 0 };

    PStackFrame topFrame = peekStack_PStackFrame(vm->callStack);
    printf("Begin exec!\n");
    while(true){
        //printf("op: %hx\n", *topFrame->ip);
        switch(*(topFrame->ip++)){
            case NOP: break;

            case PUSH8:
                pushStack_uint8_t(topFrame->stack8, *topFrame->ip);
                topFrame->ip++;
                break;
            
            case DUP8:
                dupStack_uint8_t(topFrame->stack8);
                break;
            
            case POP8:
                popStack_uint8_t(topFrame->stack8);
                break;
            
            case STORE8:
                topFrame->local8[*(uint16_t*)topFrame->ip] = popStack_uint8_t(topFrame->stack8);
                topFrame->ip += 2;
                break;
            
            case LOAD8:
                pushStack_uint8_t(topFrame->stack8,
                    topFrame->local8[*(uint16_t*)topFrame->ip]
                );
                topFrame->ip += 2;
                break;
            
            case ADD8:
                STACK_BIN_OP(topFrame->stack8, +)
                break;
            
            case SUB8:
                STACK_BIN_OP(topFrame->stack8, -)
                break;
            
            case MUL8:
                STACK_BIN_OP(topFrame->stack8, *)
                break;
            
            case DIV8:
                STACK_BIN_OP(topFrame->stack8, /)
                break;
            
            case MOD8:
                STACK_BIN_OP(topFrame->stack8, %)
                break;
            
            case SHIFTR8:
                STACK_BIN_OP(topFrame->stack8, >>)
                break;
            
            case SHIFTL8:
                STACK_BIN_OP(topFrame->stack8, <<)
                break;
            
            case AND8:
                STACK_BIN_OP(topFrame->stack8, &)
                break;
            
            case OR8:
                STACK_BIN_OP(topFrame->stack8, |)
                break;
            
            case XOR8:
                STACK_BIN_OP(topFrame->stack8, ^)
                break;
            
            case GREATER8:
                STACK_BIN_OP(topFrame->stack8, >)
                break;
            
            case LESS8:
                STACK_BIN_OP(topFrame->stack8, <)
                break;
            
            case GREATEREQ8:
                STACK_BIN_OP(topFrame->stack8, >=)
                break;
            
            case LESSEQ8:
                STACK_BIN_OP(topFrame->stack8, <=)
                break;
            
            case EQ8:
                STACK_BIN_OP(topFrame->stack8, ==)
                break;
            
            case NOTEQ8:
                STACK_BIN_OP(topFrame->stack8, !=)
                break;

            case JMP_IF_FALSE:
                {
                    int16_t jmp = *(int16_t*)topFrame->ip; topFrame->ip += 2;
                    uint8_t b = popStack_uint8_t(topFrame->stack8);
                    topFrame->ip += jmp * (b == 0);
                    break;
                }
            
            //-------------------------------------
            
            case PUSH16:
                pushStack_uint16_t(topFrame->stack16, *(uint16_t*)topFrame->ip);
                topFrame->ip += 2;
                break;
            
            case DUP16:
                dupStack_uint16_t(topFrame->stack16);
                break;
            
            case POP16:
                popStack_uint16_t(topFrame->stack16);
                break;
            
            case STORE16:
                topFrame->local16[*(uint16_t*)topFrame->ip] = popStack_uint16_t(topFrame->stack16);
                topFrame->ip += 2;
                break;
            
            case LOAD16:
                pushStack_uint16_t(topFrame->stack16,
                    topFrame->local16[*(uint16_t*)topFrame->ip]
                );
                topFrame->ip += 2;
                break;
            
            case ADD16:
                STACK_BIN_OP(topFrame->stack16, +)
                break;
            
            case SUB16:
                STACK_BIN_OP(topFrame->stack16, -)
                break;
            
            case MUL16:
                STACK_BIN_OP(topFrame->stack16, *)
                break;
            
            case DIV16:
                STACK_BIN_OP(topFrame->stack16, /)
                break;
            
            case MOD16:
                STACK_BIN_OP(topFrame->stack16, %)
                break;
            
            case SHIFTR16:
                pushStack_uint16_t(topFrame->stack16,
                    popStack_uint16_t(topFrame->stack16) >> popStack_uint8_t(topFrame->stack8)
                );
                break;
            
            case SHIFTL16:
                pushStack_uint16_t(topFrame->stack16,
                    popStack_uint16_t(topFrame->stack16) << popStack_uint8_t(topFrame->stack8)
                );
                break;
            
            case AND16:
                STACK_BIN_OP(topFrame->stack16, &)
                break;
            
            case OR16:
                STACK_BIN_OP(topFrame->stack16, |)
                break;
            
            case XOR16:
                STACK_BIN_OP(topFrame->stack16, ^)
                break;
            
            case GREATER16:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint16_t(topFrame->stack16) >= popStack_uint16_t(topFrame->stack16)
                );
                break;
            
            case LESS16:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint16_t(topFrame->stack16) <= popStack_uint16_t(topFrame->stack16)
                );
                break;
            
            case GREATEREQ16:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint16_t(topFrame->stack16) > popStack_uint16_t(topFrame->stack16)
                );
                break;
            
            case LESSEQ16:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint16_t(topFrame->stack16) < popStack_uint16_t(topFrame->stack16)
                );
                break;
            
            case EQ16:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint16_t(topFrame->stack16) == popStack_uint16_t(topFrame->stack16)
                );
                break;
            
            case NOTEQ16:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint16_t(topFrame->stack16) != popStack_uint16_t(topFrame->stack16)
                );
                break;

            //-------------------------------------

            case PUSH32:
                pushStack_uint32_t(topFrame->stack32,
                    getArrayList_PGuegFile(vm->loadedGFiles, topFrame->gfileIndex)->
                    consts32[*(uint16_t*)topFrame->ip]
                );
                topFrame->ip += 2;
                break;
            
            case DUP32:
                dupStack_uint32_t(topFrame->stack32);
                break;
            
            case POP32:
                popStack_uint32_t(topFrame->stack32);
                break;
            
            case STORE32:
                topFrame->local32[*(uint16_t*)topFrame->ip] = popStack_uint32_t(topFrame->stack32);
                topFrame->ip += 2;
                break;
            
            case LOAD32:
                pushStack_uint32_t(topFrame->stack32,
                    topFrame->local32[*(uint16_t*)topFrame->ip]
                );
                topFrame->ip += 2;
                break;
            
            case ADD32:
                STACK_BIN_OP(topFrame->stack32, +)
                break;
            
            case SUB32:
                STACK_BIN_OP(topFrame->stack32, -)
                break;
            
            case MUL32:
                STACK_BIN_OP(topFrame->stack32, *)
                break;
            
            case DIV32:
                STACK_BIN_OP(topFrame->stack32, /)
                break;
            
            case MOD32:
                STACK_BIN_OP(topFrame->stack32, %)
                break;
            
            case SHIFTR32:
                pushStack_uint32_t(topFrame->stack32,
                    popStack_uint32_t(topFrame->stack32) >> popStack_uint8_t(topFrame->stack8)
                );
                break;
            
            case SHIFTL32:
                pushStack_uint32_t(topFrame->stack32,
                    popStack_uint32_t(topFrame->stack32) << popStack_uint8_t(topFrame->stack8)
                );
                break;
            
            case AND32:
                STACK_BIN_OP(topFrame->stack32, &)
                break;
            
            case OR32:
                STACK_BIN_OP(topFrame->stack32, |)
                break;
            
            case XOR32:
                STACK_BIN_OP(topFrame->stack32, ^)
                break;
            
            case GREATER32:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint32_t(topFrame->stack32) >= popStack_uint32_t(topFrame->stack32)
                );
                break;
            
            case LESS32:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint32_t(topFrame->stack32) <= popStack_uint32_t(topFrame->stack32)
                );
                break;
            
            case GREATEREQ32:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint32_t(topFrame->stack32) > popStack_uint32_t(topFrame->stack32)
                );
                break;
            
            case LESSEQ32:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint32_t(topFrame->stack32) < popStack_uint32_t(topFrame->stack32)
                );
                break;
            
            case EQ32:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint32_t(topFrame->stack32) == popStack_uint32_t(topFrame->stack32)
                );
                break;
            
            case NOTEQ32:
                pushStack_uint8_t(topFrame->stack8,
                    popStack_uint32_t(topFrame->stack32) != popStack_uint32_t(topFrame->stack32)
                );
                break;

            case ADDF32:
                STACK_BIN_OP_REINTERP(topFrame->stack32, +, f32)
                break;
            
            case SUBF32:
                STACK_BIN_OP_REINTERP(topFrame->stack32, -, f32)
                break;

            case MULF32:
                STACK_BIN_OP_REINTERP(topFrame->stack32, *, f32)
                break;
            
            case DIVF32:
                STACK_BIN_OP_REINTERP(topFrame->stack32, /, f32)
                break;
            
            case POWF32:
                {
                    f32 b = popStack_f32((Stack_f32*)topFrame->stack32);
                    f32 a = popStack_f32((Stack_f32*)topFrame->stack32);
                    pushStack_f32((Stack_f32*)topFrame->stack32, powf(a, b));
                    break;
                }
            
            case GREATERF32 :
                {
                    f32 b = popStack_f32((Stack_f32*)topFrame->stack32);
                    f32 a = popStack_f32((Stack_f32*)topFrame->stack32);
                    pushStack_uint8_t(topFrame->stack8, a >= b);
                    break;
                }
            
            case LESSF32:
                {
                    f32 b = popStack_f32((Stack_f32*)topFrame->stack32);
                    f32 a = popStack_f32((Stack_f32*)topFrame->stack32);
                    pushStack_uint8_t(topFrame->stack8, a <= b);
                    break;
                }
            
            case GREATEREQF32:
                {
                    f32 b = popStack_f32((Stack_f32*)topFrame->stack32);
                    f32 a = popStack_f32((Stack_f32*)topFrame->stack32);
                    pushStack_uint8_t(topFrame->stack8, a > b);
                    break;
                }
            
            case LESSEQF32:
                {
                    f32 b = popStack_f32((Stack_f32*)topFrame->stack32);
                    f32 a = popStack_f32((Stack_f32*)topFrame->stack32);
                    pushStack_uint8_t(topFrame->stack8, a < b);
                    break;
                }
            
            //-------------------------------------

            case PUSHFN:
                pushStack_uint64_t(topFrame->stack64,
                    (uint64_t)topFrame->gfileIndex << 32 | *(uint16_t*)topFrame->ip
                );
                topFrame->ip += 2;
                break;
                
            case CALL:
                {
                    uint64_t fn = popStack_uint64_t(topFrame->stack64);
                    callFunction(vm, fn >> 32, fn & 0xFFFF);
                    topFrame = peekStack_PStackFrame(vm->callStack);
                    break;
                }

            case RET:
                {
                    uint64_t retVal = 0;
                    uint8_t retType = topFrame->retType;
                    switch (retType) {
                        case 0: break;
                        case 1:
                            retVal = popStack_uint8_t(topFrame->stack8);
                            break;
                        case 2:
                            retVal = popStack_uint16_t(topFrame->stack16);
                            break;
                        case 3:
                            retVal = popStack_uint32_t(topFrame->stack32);
                            break;
                        case 4:
                            retVal = popStack_uint64_t(topFrame->stack64);
                            break;
                    }
                    popStack_PStackFrame(vm->callStack);
                    freeStackFrame(topFrame);
                    topFrame = peekStack_PStackFrame(vm->callStack);
                    if(topFrame == NULL){
                        return res;
                    }

                    switch (retType) {
                        case 0: break;
                        case 1:
                            pushStack_uint8_t(topFrame->stack8, retVal);
                            break;
                        case 2:
                            pushStack_uint16_t(topFrame->stack16, retVal);
                            break;
                        case 3:
                            pushStack_uint32_t(topFrame->stack32, retVal);
                            break;
                        case 4:
                            pushStack_uint64_t(topFrame->stack64, retVal);
                            break;
                    }
                    break;
                }

            default:
                {
                    printf("Unknown opcode: %hhx\n", *(topFrame->ip - 1));
                    res.code = 1;
                    return res;
                    break;
                }

        }
    }

    

    return res;
}