#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdint.h>
#include <stdint.h>

#include "../util/array_list.h"

#define MAGIC ('a' << 24 | 'l' << 16 | 'a' << 8 | 'l')

typedef struct {
    uint64_t len;
    uint8_t *data;
    char *path;
} File;

typedef struct {
    uint8_t returnSize; //0:void, 1:8, 2:16, 3:32, 4:64

    uint8_t paramSizesLen;
    uint8_t *paramSizes;

    uint16_t numLocals8;
    uint16_t numLocals16;
    uint16_t numLocals32;
    uint16_t numLocals64;

    uint16_t maxStackDepth8;
    uint16_t maxStackDepth16;
    uint16_t maxStackDepth32;
    uint16_t maxStackDepth64;

    uint32_t codeLen;
    uint8_t *code;
} FnInfo;

typedef struct Type Type;
struct Type {
    enum : uint8_t { PRIMITIVE = 1, FN = 2, TUPLE = 3 } code;
    union {
        struct {
            uint8_t code;
        } primitive;

        struct {
            Type *returnType;
            uint8_t paramTypesLen;
            Type **paramTypes;
        } fn;
    } as;
};

typedef struct {
    File *file;

    uint8_t modNameLen;
    char *modName;

    Type *modInterfaceType;

    uint16_t consts32Len;
    uint32_t *consts32;

    uint16_t consts64Len;
    uint64_t *consts64;

    uint16_t functionsLen;
    FnInfo *functions;
} GuegFile;
typedef GuegFile *PGuegFile;
ARRAY_LIST_IMPL(PGuegFile)

File *readFileFromPath(char *path);
void freeFile(File *file);

GuegFile *parseGuegFile(File *file);
void freeGuegFile(GuegFile *gfile);

#endif