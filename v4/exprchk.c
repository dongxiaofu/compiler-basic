#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
// OPINFO(OP_INDEX,          17,   "nop",    NOP,          NOP)

static AstExpression (*ExperCheckers[])(AstExpression) = {
#define OPINFO(op, prec, name, func, opcode)	Check##func##Expression,	
#include "opinfo.h"
#undef OPINFO
};

// static AstExpression CheckAssignmentExpression(AstExpression expr)
AstExpression CheckAssignmentExpression(AstExpression expr)
{
	AstExpression expr2;
	return expr2;
}
static AstExpression CheckPostfixExpression(AstExpression expr)
{
	AstExpression expr2;
	return expr2;

}

// TODO 占位符，暂时无作用。
static AstExpression CheckNOPExpression(AstExpression expr)
{
	AstExpression expr2;
	return expr2;
}

AstExpression CheckExpression(AstExpression expr)
{

	return ExperCheckers[expr->op];
}

AstExpression CheckExpressionList(AstExpression expr)
{
	AstExpression current = expr;
	while(current){
		CheckExpression(current);
		current = current->next;
	}
	return expr;
}

AstExpression CheckIdentifierList(AstExpression expr)
{
	CheckExpressionList(expr);

	return expr;
}
