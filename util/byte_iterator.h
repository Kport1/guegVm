#ifndef BYTE_ITERATOR_H
#define BYTE_ITERATOR_H

#define NEXT_BYTES(index, type, ptr) *(type*)(ptr + index); index += sizeof(type)
#define HAS_BYTES(index, amnt, total) (amnt <= total - index)



#endif