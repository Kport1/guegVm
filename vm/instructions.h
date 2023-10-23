#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define NOP                 0x00

#define PUSH8               0x01
#define DUP8                0x02
#define POP8                0x03
#define STORE8              0x04
#define LOAD8               0x05
#define ADD8                0x06
#define SUB8                0x07
#define MUL8                0x08
#define DIV8                0x09
#define MOD8                0x0A
#define SHIFTR8             0x0B
#define SHIFTL8             0x0C
#define AND8                0x0D
#define OR8                 0x0E
#define XOR8                0x0F
#define GREATER8            0x10
#define LESS8               0x11
#define GREATEREQ8          0x12
#define LESSEQ8             0x13
#define EQ8                 0x14
#define NOTEQ8              0x15
#define JMP_IF_FALSE        0x16

//-----------------------------------------------------------

#define PUSH16              0x31
#define DUP16               0x32
#define POP16               0x33
#define STORE16             0x34
#define LOAD16              0x35
#define ADD16               0x36
#define SUB16               0x37
#define MUL16               0x38
#define DIV16               0x39
#define MOD16               0x3A
#define SHIFTR16            0x3B
#define SHIFTL16            0x3C
#define AND16               0x3D
#define OR16                0x3E
#define XOR16               0x3F
#define GREATER16           0x40
#define LESS16              0x41
#define GREATEREQ16         0x42
#define LESSEQ16            0x43
#define EQ16                0x44
#define NOTEQ16             0x45

//-----------------------------------------------------------

#define PUSH32              0x61
#define DUP32               0x62
#define POP32               0x63
#define STORE32             0x64
#define LOAD32              0x65
#define ADD32               0x66
#define SUB32               0x67
#define MUL32               0x68
#define DIV32               0x69
#define MOD32               0x6A
#define SHIFTR32            0x6B
#define SHIFTL32            0x6C
#define AND32               0x6D
#define OR32                0x6E
#define XOR32               0x6F
#define GREATER32           0x70
#define LESS32              0x71
#define GREATEREQ32         0x72
#define LESSEQ32            0x73
#define EQ32                0x74
#define NOTEQ32             0x75
#define ADDF32              0x76
#define SUBF32              0x77
#define MULF32              0x78
#define DIVF32              0x79
#define POWF32              0x7A
#define GREATERF32          0x7B
#define LESSF32             0x7C
#define GREATEREQF32        0x7D
#define LESSEQF32           0x7E

//-----------------------------------------------------------

#define PUSH64              0x91
#define DUP64               0x92
#define POP64               0x93
#define STORE64             0x94
#define LOAD64              0x95
#define ADD64               0x96
#define SUB64               0x97
#define MUL64               0x98
#define DIV64               0x99
#define MOD64               0x9A
#define SHIFTR64            0x9B
#define SHIFTL64            0x9C
#define AND64               0x9D
#define OR64                0x9E
#define XOR64               0x9F
#define GREATER64           0xA0
#define LESS64              0xA1
#define GREATEREQ64         0xA2
#define LESSEQ64            0xA3
#define EQ64                0xA4
#define NOTEQ64             0xA5
#define ADDF64              0xA6
#define SUBF64              0xA7
#define MULF64              0xA8
#define DIVF64              0xA9
#define POWF64              0xAA
#define GREATERF64          0xAB
#define LESSF64             0xAC
#define GREATEREQF64        0xAD
#define LESSEQF64           0xAE
#define PUSHFN              0xB1
#define CALL                0xB2

//---------------------------------------------------------------------

#define JMP                 0xC1
#define RET                 0xC2


#endif