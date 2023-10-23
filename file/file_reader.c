#include "file_reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>

#include "../error/error.h"
#include "../util/byte_iterator.h"

File *readFileFromPath(char *path){
    FILE *file = fopen(path, "r");
    
    if(!file){
        errorf(u8"File \"%s\" couldn't be opened", path);
        return NULL;
    }
    
    fseek(file, 0L, SEEK_END);
    long flen = ftell(file);
    rewind(file);

    File *f = malloc(sizeof(File));
    f->len = flen;
    f->data = malloc(flen);
    f->path = path;

    fread(f->data, 1, flen, file);

    fclose(file);

    return f;
}

void freeFile(File *file){
    free(file->data);
    free(file);
}

void freeType(Type *type){
    switch (type->code) {
        case PRIMITIVE:
            break;
        case FN:
            freeType(type->as.fn.returnType);
            for(int i = 0; i < type->as.fn.paramTypesLen; i++){
                freeType(type->as.fn.paramTypes[i]);
            }
            free(type->as.fn.paramTypes);
            break;
        case TUPLE:
            break;
    }
    free(type);
}

Type *parseType(uint64_t *index, uint8_t *data, uint64_t len);

Type *parseFnType(uint64_t *index, uint8_t *data, uint64_t len){
    Type *ret = malloc(sizeof(Type));

    Type *fnRetType = parseType(index, data, len);
    if(fnRetType == NULL) { free(ret); return NULL; }
    ret->as.fn.returnType = fnRetType;

    if(!HAS_BYTES(*index, 1, len)) {
        freeType(ret->as.fn.returnType);
        free(ret);
        return NULL;
    }
    ret->as.fn.paramTypesLen = NEXT_BYTES(*index, uint8_t, data);
    ret->as.fn.paramTypes = malloc(sizeof(Type*) * ret->as.fn.paramTypesLen);

    for(int i = 0; i < ret->as.fn.paramTypesLen; i++){
        Type *paramType = parseType(index, data, len);
        if(paramType == NULL) {
            freeType(ret->as.fn.returnType);
            while(i --> 0){
                freeType(ret->as.fn.paramTypes[i]);
            }
            free(ret->as.fn.paramTypes);
            free(ret);
            return NULL;
        }
        ret->as.fn.paramTypes[i] = paramType;
    }

    ret->code = FN;
    return ret;
}

Type *parseType(uint64_t *index, uint8_t *data, uint64_t len){
    Type *ret;
    if(!HAS_BYTES(*index, 1, len)) { freeType(ret); return NULL; }
    uint8_t code = NEXT_BYTES(*index, uint8_t, data);
    switch (code) {
        case PRIMITIVE:
            {
                ret = malloc(sizeof(Type));
                if(!HAS_BYTES(*index, 1, len)) { freeType(ret); return NULL; }
                ret->as.primitive.code = NEXT_BYTES(*index, uint8_t, data);
                break;
            }
        
        case FN:
            {
                ret = parseFnType(index, data, len);
                break;
            }
        
        case TUPLE:
            break;
        default:
            return NULL;
    }
    ret->code = code;
    return ret;
}



GuegFile *parseGuegFile(File *file){
    #define PARSE_ERR { errorf(u8"Failed to parse file: \"%s\".", file->path); free(gfile); return NULL; }
    #define PARSE_ERR_2 {\
        freeType(gfile->modInterfaceType);\
        errorf(u8"Failed to parse file: \"%s\".", file->path);\
        free(gfile);\
        return NULL;\
    }
    #define PARSE_ERR_3 {\
        free(gfile->functions);\
        freeType(gfile->modInterfaceType);\
        errorf(u8"Failed to parse file: \"%s\".", file->path);\
        free(gfile);\
        return NULL;\
    }

    uint64_t index = 0;
    uint8_t *data = file->data;
    uint64_t len = file->len;

    GuegFile *gfile = malloc(sizeof(GuegFile));

    //MAGIC
    if(!HAS_BYTES(index, 4, len)) PARSE_ERR
    uint32_t magic = NEXT_BYTES(index, uint32_t, data);
    if(magic != MAGIC) PARSE_ERR

    //MODULE NAME
    if(!HAS_BYTES(index, 1, len)) PARSE_ERR
    gfile->modNameLen = NEXT_BYTES(index, uint8_t, data);
    if(!HAS_BYTES(index, sizeof(char) * gfile->modNameLen, len)) PARSE_ERR
    gfile->modName = (char*)(data + index); index += gfile->modNameLen * sizeof(char);

    //MODULE INTERFACE TYPE
    gfile->modInterfaceType = parseFnType(&index, data, len);
    if(gfile->modInterfaceType == NULL) PARSE_ERR


    //32-bit CONSTANTS
    if(!HAS_BYTES(index, 2, len)) PARSE_ERR_2
    gfile->consts32Len = NEXT_BYTES(index, uint16_t, data);
    if(!HAS_BYTES(index, sizeof(uint32_t) * gfile->consts32Len, len)) PARSE_ERR_2
    gfile->consts32 = (uint32_t*)(data + index); index += gfile->consts32Len * sizeof(uint32_t);

    //64-bit CONSTANTS
    if(!HAS_BYTES(index, 2, len)) PARSE_ERR_2
    gfile->consts64Len = NEXT_BYTES(index, uint16_t, data);
    if(!HAS_BYTES(index, sizeof(uint64_t) * gfile->consts64Len, len)) PARSE_ERR_2
    gfile->consts64 = (uint64_t*)(data + index); index += gfile->consts64Len * sizeof(uint64_t);

    //FUNCTIONS
    if(!HAS_BYTES(index, 4, len)) PARSE_ERR_2
    gfile->functionsLen = NEXT_BYTES(index, uint16_t, data);
    gfile->functions = malloc(sizeof(FnInfo) * gfile->functionsLen);
    for(uint32_t i = 0; i < gfile->functionsLen; i++){

        //RETURN SIZE
        if(!HAS_BYTES(index, 1, len)) PARSE_ERR_3
        gfile->functions[i].returnSize = NEXT_BYTES(index, uint8_t, data);

        //PARAMETER SIZES
        if(!HAS_BYTES(index, 1, len)) PARSE_ERR_3
        gfile->functions[i].paramSizesLen = NEXT_BYTES(index, uint8_t, data);
        if(!HAS_BYTES(index, sizeof(uint8_t) * gfile->functions[i].paramSizesLen, len)) PARSE_ERR_3
        gfile->functions[i].paramSizes = data + index; index += gfile->functions[i].paramSizesLen;

        //AMOUNT LOCALS / STACK DEPTH
        if(!HAS_BYTES(index, sizeof(uint16_t) * 8, len)) PARSE_ERR_3
        gfile->functions[i].numLocals8  = NEXT_BYTES(index, uint16_t, data);
        gfile->functions[i].numLocals16 = NEXT_BYTES(index, uint16_t, data);
        gfile->functions[i].numLocals32 = NEXT_BYTES(index, uint16_t, data);
        gfile->functions[i].numLocals64 = NEXT_BYTES(index, uint16_t, data);

        gfile->functions[i].maxStackDepth8  = NEXT_BYTES(index, uint16_t, data);
        gfile->functions[i].maxStackDepth16 = NEXT_BYTES(index, uint16_t, data);
        gfile->functions[i].maxStackDepth32 = NEXT_BYTES(index, uint16_t, data);
        gfile->functions[i].maxStackDepth64 = NEXT_BYTES(index, uint16_t, data);

        //FUNCTION CODE
        if(!HAS_BYTES(index, sizeof(uint32_t), len)) PARSE_ERR_3
        gfile->functions[i].codeLen = NEXT_BYTES(index, uint32_t, data);
        if(!HAS_BYTES(index, gfile->functions[i].codeLen, len)) PARSE_ERR_3
        gfile->functions[i].code = (data + index); index += gfile->functions[i].codeLen;
    }

    gfile->file = file;
    return gfile;

    #undef PARSE_ERR
    #undef PARSE_ERR_2
    #undef PARSE_ERR_3
}

void freeGuegFile(GuegFile *gfile){
    freeFile(gfile->file);
    free(gfile->functions);
    free(gfile);
}