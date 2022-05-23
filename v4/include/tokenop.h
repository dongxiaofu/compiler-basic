#ifndef TOKENOP
#error "You must define TOKENOP macro before include this file"
#endif

// assign_op
/**
 * add_op     = "+" | "-" | "|" | "^" .
mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .
 */
TOKENOP(TK_ASSIGN,      OP_ASSIGN,      OP_NONE)
TOKENOP(TK_ADD_ASSIGN,      OP_ADD_ASSIGN,      OP_NONE)
TOKENOP(TK_MINUS_ASSIGN,      OP_MINUS_ASSIGN,      OP_NONE)
TOKENOP(TK_OR_ASSIGN,      OP_OR_ASSIGN,      OP_NONE)
TOKENOP(TK_XOR_ASSIGN,      OP_XOR_ASSIGN,      OP_NONE)
TOKENOP(TK_MUL_ASSIGN,      OP_MUL_ASSIGN,      OP_NONE)
TOKENOP(TK_DIV_ASSIGN,      OP_DIV_ASSIGN,      OP_NONE)
TOKENOP(TK_MOD_ASSIGN,      OP_MOD_ASSIGN,      OP_NONE)
TOKENOP(TK_LEFT_SHIFT_ASSIGN,      OP_LEFT_SHIFT_ASSIGN,      OP_NONE)
TOKENOP(TK_RIGHT_SHIFT_ASSIGN,      OP_RIGHT_SHIFT_ASSIGN,      OP_NONE)
TOKENOP(TK_AND_ASSIGN,      OP_AND_ASSIGN,      OP_NONE)
// https://zhuanlan.zhihu.com/p/86765504
TOKENOP(TK_BIT_CLEAR_ASSIGN,      OP_BIT_CLEAR_ASSIGN,      OP_NONE)
// 没找到合适的英语名称
// todo 是否需要为这个符号创建一个token
// TODO := 在声明中出现，是否应该放在这里？
TOKENOP(TK_INIT_ASSIGN,      OP_INIT_ASSIGN,      OP_NONE)

// binary_op
/**
 * binary_op  = "||" | "&&" | rel_op | add_op | mul_op .
rel_op     = "==" | "!=" | "<" | "<=" | ">" | ">=" .
add_op     = "+" | "-" | "|" | "^" .
mul_op     = "*" | "/" | "%" | "<<" | ">>" | "&" | "&^" .
 */
TOKENOP(TK_CONDITIONAL_OR,      OP_CONDITIONAL_OR,      OP_NONE)
TOKENOP(TK_CONDITIONAL_AND,      OP_CONDITIONAL_AND,      OP_NONE)
// rel_op
TOKENOP(TK_EQUAL,      OP_EQUAL,      OP_NONE)
TOKENOP(TK_NOT_EQUAL,      OP_NOT_EQUAL,      OP_NONE)
TOKENOP(TK_LESS,      OP_LESS,      OP_NONE)
TOKENOP(TK_LESS_OR_EQUAL,      OP_LESS_OR_EQUAL,      OP_NONE)
TOKENOP(TK_GREATER,      OP_GREATER,      OP_NONE)
TOKENOP(TK_GREATER_OR_EQUAL,      OP_GREATER_OR_EQUAL,      OP_NONE)

// add_op
TOKENOP(TK_ADD,      OP_ADD,      OP_POS)
TOKENOP(TK_MINUS,      OP_MINUS,      OP_NEG)
TOKENOP(TK_BINARY_BITWISE_OR,      OP_BINARY_BITWISE_OR,      OP_NONE)
TOKENOP(TK_BINARY_BITWISE_XOR,      OP_BINARY_BITWISE_XOR,    OP_BITWISE_XOR)

// mul_op
TOKENOP(TK_MUL,      OP_MUL,      OP_DEREF)
TOKENOP(TK_DIV,      OP_DIV,      OP_NONE)
TOKENOP(TK_MOD,      OP_MOD,      OP_NONE)
TOKENOP(TK_LEFT_SHIFT,      OP_LEFT_SHIFT,      OP_NONE)
TOKENOP(TK_RIGHT_SHIFT,      OP_RIGHT_SHIFT,      OP_NONE)
TOKENOP(TK_BITWISE_AND,      OP_BITWISE_AND,      OP_ADDRESS)
TOKENOP(TK_BITWISE_AND_NOT,      OP_BITWISE_AND_NOT,      OP_NONE)

// todo 怎么区分a++还是++a？
// 不用区分。只支持a++。
TOKENOP(TK_INC,      OP_INC,      OP_NONE)
TOKENOP(TK_DEC,      OP_DEC,      OP_NONE)

// 为了更方便地判断一元运算符，把这些符号集中到一起。
// todo 像*这种两种用途的符号，怎么处理？
// unary_op
/**
 * unary_op   = "+" | "-" | "!" | "^" | "*" | "&" | "<-" .
 */
// todo 不知道怎么处理这几个符号。如果保留它们，计算运算符的优先级时会出错。
// TOKENOP(TK_POSITIVE,      OP_POSITIVE,      OP_NONE)
// TOKENOP(TK_NEGATIVE,      OP_NEGATIVE,      OP_NONE)
TOKENOP(TK_NOT,      OP_NONE,      OP_NOT)
// TOKENOP(TK_BITWISE_XOR,      OP_BITWISE_XOR,      OP_NONE)
// TOKENOP(TK_POINTER,      OP_POINTER,      OP_NONE)
// TOKENOP(TK_ADDRESS,      OP_ADDRESS,      OP_NONE)
// TODO 需留意。可能不正确。
TOKENOP(TK_RECEIVE,      OP_SEND,	OP_RECEIVE)

// TOKENOP(TK_DOT,      OP_NONE,	OP_MEMBER)
