#ifndef OPINFO
#error "You must define OPINFO macro before include this file"
#endif 
//

OPINFO(OP_ASSIGN,        2,    "=",      Assignment,     NOP)

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

// todo 有很多多余的数据，暂时不知道哪些是多余的。
OPINFO(OP_CAST,          9,   "cast",   Unary,          NOP)
OPINFO(OP_PREINC,        9,   "++",     Unary,          NOP)
OPINFO(OP_PREDEC,        9,   "--",     Unary,          NOP)
OPINFO(OP_ADDRESS,       9,   "&",      Unary,          ADDR)
OPINFO(OP_DEREF,         9,   "*",      Unary,          DEREF)
OPINFO(OP_POS,           9,   "+",      Unary,          NOP)
OPINFO(OP_NEG,           9,   "-",      Unary,          NEG)
OPINFO(OP_COMP,          9,   "~",      Unary,          BCOM)
OPINFO(OP_NOT,           9,   "!",      Unary,          NOP)
OPINFO(OP_SIZEOF,        9,   "sizeof", Unary,          NOP)
OPINFO(OP_INDEX,         10,   "[]",     Postfix,        NOP)
OPINFO(OP_CALL,          10,   "call",   Postfix,        NOP)
OPINFO(OP_MEMBER,        10,   ".",      Postfix,        NOP)
OPINFO(OP_PTR_MEMBER,    10,   "->",     Postfix,        NOP)
OPINFO(OP_POSTINC,       10,   "++",     Postfix,        INC)
OPINFO(OP_POSTDEC,       10,   "--",     Postfix,        DEC)
OPINFO(OP_ID,            11,   "id",     Primary,        NOP)
OPINFO(OP_CONST,         11,   "const",  Primary,        NOP)
OPINFO(OP_STR,           11,   "str",    Primary,        NOP)
OPINFO(OP_NONE,          12,   "nop",    Error,          NOP)

