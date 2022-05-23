#ifndef TRAN_EXPR_H
#define TRAN_EXPR_H

#include <assert.h>

Symbol Addressof(Symbol sym);
Symbol Deref(Type ty, Symbol addr);
Symbol Offset(Type ty, Symbol addr, int voff, int coff);

Symbol TranPrimaryExpression(AstExpression expr);
Symbol TranIncDecExpression(AstExpression expr);
Symbol TranMemberAccess(AstExpression expr);
Symbol TranExpression(AstExpression expr);
#endif
