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
	return expr;
}

AstExpression Constant(Type ty, union value val)
{
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);

	expr->op = OP_CONST;
	expr->val = val;
	// TODO 常量为啥需要ty？
	expr->ty = ty;

	return expr;
}

AstExpression ScalePointerOffset(AstExpression offset, int scale)
{
	AstExpression expr;
	CREATE_AST_NODE(expr, Expression);
	
	expr->op = OP_MUL;
	// TODO 本函数中的expr的ty有用吗？
	expr->ty = offset->ty;
	expr->kids[0] = offset;
	
	union value val;
	val.i[0] = scale;
	val.i[1] = 0;
	expr->kids[1] = Constant(offset->ty, val);	

	return expr;
}

AstExpression Adjust(AstExpression expr)
{
	if(expr->ty->categ == ARRAY){
		expr->ty = PointerTo(expr->ty->bty);
		expr->ty->isarray = 1;
	}


	return expr;
}

AstExpression CheckPrimaryExpression(AstExpression expr)
{
	if(expr->op == OP_CONST){
		return expr;
	}

	Symbol p;
	
	p = LookupID(expr->val.p);	
	if(p == NULL){
		ERROR("%s\n", "CheckPrimaryExpression error");
	}else{
		expr->ty = p->ty;
		expr->val.p = p;
	}


	return expr;
}

AstExpression CheckMemberAccess(AstExpression expr)
{
	expr->kids[0] = CheckExpression(expr->kids[0]);
	
	Field fld = LookupField(expr->val.p, expr->kids[0]->val.p);	
	if(fld == NULL){
		ERROR("%s\n", CheckMemberAccess);
	}
	expr->ty = fld->ty;
	expr->val.p = fld;

	return expr;
}

AstExpression CheckTypeAssert(AstExpression expr)
{


	return expr;
}

AstExpression CheckFunctionCall(AstExpression expr)
{


	return expr;
}

static AstExpression CheckPostfixExpression(AstExpression expr)
{
	switch(expr->op){
		case OP_INDEX:
			expr->kids[0] = Adjust(CheckExpression(expr->kids[0]));
			expr->kids[1] = Adjust(CheckExpression(expr->kids[1]));
			if(IsObjectPtr(expr->kids[0]->ty) == 1 && IsIntegType(expr->kids[1]->ty) == 1){
				expr->ty = expr->kids[0]->ty->bty;
				expr->kids[1] = ScalePointerOffset(expr->kids[0],expr->ty->size);
				return expr;
			}
			
			ERROR("line:%d\n", __LINE__);
		case OP_DOT:	
		case OP_MEMBER:
			expr = CheckMemberAccess(expr);
		case OP_TYPE_ASSERT:
			expr = CheckTypeAssert(expr);
		case OP_CALL:
			expr = CheckFunctionCall(expr);
		default:
			break;
	}

	return expr;
}

// TODO 占位符，暂时无作用。
static AstExpression CheckNOPExpression(AstExpression expr)
{
	return expr;
}

AstExpression CheckExpression(AstExpression expr)
{

	return ExperCheckers[expr->op](expr);
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


void AssignInterfaceVariable(VariableSymbol sym, InitData idata)
{
	// TODO 检查两个参数的数据类型是否匹配
	if(1 == 1){
		sym->typeAlias = idata->typeAlias;
		sym->idata = idata;
	}
}
