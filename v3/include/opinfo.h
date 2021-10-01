#ifndef OPINFO
#error "You must define OPINFO macro before include this file"
#endif 
//

OPINFO(OP_OR,            4,    "||",     Binary,         NOP)
OPINFO(OP_AND,           5,    "&&",     Binary,         NOP)

OPINFO(OP_EQUAL,         6,    "==",     Binary,         JE)
OPINFO(OP_UNEQUAL,       6,    "!=",     Binary,         JNE)
OPINFO(OP_GREAT,         6,   ">",      Binary,         JG)
OPINFO(OP_LESS,          6,   "<",      Binary,         JL)
OPINFO(OP_GREAT_EQ,      6,   ">=",     Binary,         JGE)
OPINFO(OP_LESS_EQ,       6,   "<=",     Binary,         JLE)

OPINFO(OP_ADD,           7,   "+",      Binary,         ADD)
OPINFO(OP_SUB,           7,   "-",      Binary,         SUB)
OPINFO(OP_BITOR,         7,    "|",      Binary,         BOR)
OPINFO(OP_BITXOR,        7,    "^",      Binary,         BXOR)

OPINFO(OP_MUL,           8,   "*",      Binary,         MUL)
OPINFO(OP_DIV,           8,   "/",      Binary,         DIV)
OPINFO(OP_MOD,           8,   "%",      Binary,         MOD)
OPINFO(OP_LSHIFT,        8,   "<<",     Binary,         LSH)
OPINFO(OP_RSHIFT,        8,   ">>",     Binary,         RSH)
OPINFO(OP_BITAND,        8,    "&",      Binary,         BAND)
OPINFO(OP_BITAND_NOT,    8,    "&^",     Binary,         BAND)
