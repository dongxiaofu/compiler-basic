#ifndef EXPRCHK_H
#define EXPRCHK_H

#include "ast.h"
#include "lex.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"


void AssignInterfaceVariable(VariableSymbol sym, InitData idata);

AstExpression Constant(Type ty, union value val);
AstExpression ScalePointerOffset(AstExpression offset, int scale);

AstExpression CheckUnaryExpression(AstExpression expr);
AstExpression CheckIncDecExpression(AstExpression expr);
AstExpression CheckDeccExpression(AstExpression expr);
AstExpression CheckPrimaryExpression(AstExpression expr);
AstExpression CheckAssignmentExpression(AstExpression expr);

AstExpression CheckMemberAccess(AstExpression expr);
AstExpression CheckTypeAssert(AstExpression expr);
AstArguments CheckArguments(AstArguments args, SignatureElement *params, int paramCount);
AstExpression CheckFunctionCall(AstExpression expr);
AstExpression CheckCastExpression(AstExpression expr);

static AstExpression CheckPostfixExpression(AstExpression expr);

// TODO 占位符，暂时无作用。
static AstExpression CheckNOPExpression(AstExpression expr);

// Start
AstExpression CheckCONDITIONAL_OR(AstExpression expr);
AstExpression CheckCONDITIONAL_AND(AstExpression expr);
AstExpression CheckEQUAL(AstExpression expr);
AstExpression CheckNOT_EQUAL(AstExpression expr);
AstExpression CheckLESS(AstExpression expr);
AstExpression CheckLESS_OR_EQUAL(AstExpression expr);
AstExpression CheckGREATER(AstExpression expr);
AstExpression CheckGREATER_OR_EQUAL(AstExpression expr);
AstExpression CheckADD(AstExpression expr);
AstExpression CheckMINUS(AstExpression expr);
AstExpression CheckBINARY_BITWISE_OR(AstExpression expr);
AstExpression CheckBINARY_BITWISE_XOR(AstExpression expr);
AstExpression CheckMUL(AstExpression expr);
AstExpression CheckDIV(AstExpression expr);
AstExpression CheckMOD(AstExpression expr);
AstExpression CheckLEFT_SHIFT(AstExpression expr);
AstExpression CheckRIGHT_SHIFT(AstExpression expr);
AstExpression CheckBITWISE_AND(AstExpression expr);
AstExpression CheckBITWISE_AND_NOT(AstExpression expr);
// End

AstExpression CheckBinaryExpression(AstExpression expr);
AstExpression CheckExpression(AstExpression expr);
// TODO 暂时放在这里。
AstExpression CheckExpressionList(AstExpression expr);
AstExpression CheckIdentifierList(AstExpression expr);
AstExpression CheckDeferExpr(AstExpression expr);

#endif
