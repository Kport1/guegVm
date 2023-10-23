#include <stdio.h>
#include <stdlib.h>

#include "vm/vm.h"
#include "file/file_reader.h"
#include "util/array_list.h"

int main(int argc, char *argv[]){
    
    File *file = readFileFromPath("");
    if(file == NULL) return 1;

    GuegFile *gfile = parseGuegFile(file);
    if(gfile == NULL){
        free(file);
        return 1;
    }

    Vm *vm = newVm();
    loadGFile(vm, gfile);
    callFunction(vm, 0, 1);
    InterpResult res = execute(vm);

    freeGuegFile(gfile);

    return res.code;
}