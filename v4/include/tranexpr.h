#ifndef TRAN_EXPR_H
#define TRAN_EXPR_H

#include <assert.h>

Symbol Addressof(Symbol sym);
Symbol Deref(Type ty, Symbol addr);
Symbol Offset(Type ty, Symbol addr, int voff, int coff);

Symbol TranslateUnaryExpression(AstExpression expr);
Symbol TranslatePrimaryExpression(AstExpression expr);
Symbol TranslateCastExpression(AstExpression expr);
Symbol TranslateIncDecExpression(AstExpression expr);
Symbol TranslateMemberAccess(AstExpression expr);
Symbol TranslateExpression(AstExpression expr);
#endif
