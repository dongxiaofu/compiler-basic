#ifndef OPINFO
#error "You must define OPINFO macro before include this file"
#endif 

OPINFO(OP_ASSIGN,-1, "=",      Assignment,     NOP)
OPINFO(OP_ADD_ASSIGN,-1,"+=",      Assignment,     NOP)
OPINFO(OP_MINUS_ASSIGN,-1,"-=",      Assignment,     NOP)
OPINFO(OP_OR_ASSIGN,-1, "|=",      Assignment,     NOP)
OPINFO(OP_XOR_ASSIGN,-1, "^=",      Assignment,     NOP)
OPINFO(OP_MUL_ASSIGN,-1,"*=",      Assignment,     NOP)
OPINFO(OP_DIV_ASSIGN,-1,"/=",      Assignment,     NOP)
OPINFO(OP_MOD_ASSIGN,-1,"%=",      Assignment,     NOP)
OPINFO(OP_LEFT_SHIFT_ASSIGN,-1, "<<=",      Assignment,     NOP)
OPINFO(OP_RIGHT_SHIFT_ASSIGN,-1, ">>=",      Assignment,     NOP)
OPINFO(OP_AND_ASSIGN,-1, "&=",      Assignment,     NOP)
OPINFO(OP_BIT_CLEAR_ASSIGN,-1,"&^=",      Assignment,     NOP)
OPINFO(OP_INIT_ASSIGN,-1, ":=",      Assignment,     NOP)

OPINFO(OP_CONDITIONAL_OR,1,"||",      Binary,     NOP)
OPINFO(OP_CONDITIONAL_AND,2,"&&",      Binary,     NOP)
OPINFO(OP_EQUAL,3,"==",      Binary,     JE)
OPINFO(OP_NOT_EQUAL,3,"!=",      Binary,     JNE)
OPINFO(OP_LESS,3,"<",      Binary,     JL)
OPINFO(OP_LESS_OR_EQUAL,3,"<=",      Binary,     JLE)
OPINFO(OP_GREATER,3,">",      Binary,     JG)
OPINFO(OP_GREATER_OR_EQUAL,3,">=",      Binary,     JGE)
OPINFO(OP_ADD,4,"+",      Binary,     ADD)
OPINFO(OP_MINUS,4,"-",      Binary,     SUB)
OPINFO(OP_BINARY_BITWISE_OR,4,"|",      Binary,     NOP)
OPINFO(OP_BINARY_BITWISE_XOR,4,"^",      Binary,     BXOR)
OPINFO(OP_MUL,5,"*",      Binary,     NOP)
OPINFO(OP_DIV,5,"/",      Binary,     NOP)
OPINFO(OP_MOD,5,"%",      Binary,     NOP)
OPINFO(OP_LEFT_SHIFT,5,"<<",      Binary,     NOP)
OPINFO(OP_RIGHT_SHIFT,5,">>",      Binary,     NOP)
OPINFO(OP_BITWISE_AND,5,"&",      Binary,     NOP)
OPINFO(OP_BITWISE_AND_NOT,5,"&^",      Binary,     NOP)

OPINFO(OP_SEND,-1, "<-",      Assignment,     NOP)


OPINFO(OP_POS,6,"+",      Unary,     NOP)
OPINFO(OP_NEG,6,"-",      Unary,     NEG)
OPINFO(OP_BITWISE_XOR,6,"^",      Unary,     BCOM)
OPINFO(OP_DEREF,6,"*",      Unary,     NOP)
OPINFO(OP_NOT,6, "!",      Unary,     NOP)
OPINFO(OP_ADDRESS,6, "!",      Unary,     NOP)
OPINFO(OP_INC,7, "++",      IncDec,     NOP)
OPINFO(OP_DEC,7, "--",      IncDec,     NOP)
// TODO 不知道优先级
OPINFO(OP_RECEIVE,8, "<-",      Assignment,     NOP)

// 新加的运算符。不知道放到哪里合适，先放在这里。start
// OP_INDEX和OP_ARRAY有什么不一样？
OPINFO(OP_INDEX,          17,   "nop",    Postfix,          NOP)
OPINFO(OP_ID,          17,   "",    	  Primary,          NOP)
OPINFO(OP_ARRAY,          17,   "nop",    Primary,          NOP)
OPINFO(OP_SLICE,          17,   "nop",    Primary,          NOP)
OPINFO(OP_DOT,          17,   "nop",    Postfix,          NOP)

OPINFO(OP_CONST,          17,   "nop",    Primary,          NOP)
OPINFO(OP_STR,          17,   "nop",    Primary,          NOP)

OPINFO(OP_COMPOSITELIT,          17,   "nop",    Primary,          NOP)
OPINFO(OP_FUNC_LIT,          17,   "nop",    Primary,          NOP)
OPINFO(OP_PACKAGE,          17,   "nop",    Primary,          NOP)


OPINFO(OP_MEMBER,          17,   "nop",    Postfix,          NOP)
OPINFO(OP_CALL,          17,   "nop",    Postfix,          CALL)
OPINFO(OP_METHOD,          17,   "nop",    Postfix,          NOP)
OPINFO(OP_TYPE_ASSERT,          17,   "nop",    Postfix,          NOP)
OPINFO(OP_CAST,          17,   "nop",    Postfix,          NOP)
// OPINFO(OP_PACKAGE,          17,   "nop",    NOP,          NOP)
// OPINFO(OP_PACKAGE,          17,   "nop",    NOP,          NOP)
// OPINFO(OP_PACKAGE,          17,   "nop",    NOP,          NOP)

// 新加的运算符。不知道放到哪里合适，先放在这里。end

OPINFO(OP_NONE,          17,   "nop",    Assignment,          NOP)
