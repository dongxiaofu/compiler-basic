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

AstArguments CheckArguments(AstArguments args, SignatureElement *params, int paramCount)
{
	// 检查参数的数据类型
	AstExpression oneArg = args->args;
	int cn = 0;
	for(int i = 0; i < paramCount; i++){
		if(oneArg == NULL){
			break;
		}

		oneArg = CheckExpression(oneArg);
		// TODO 怎么判断oneArg能不能赋值给params[i]？

		oneArg = oneArg->next;
		cn++;
	}

	if(cn == paramCount && oneArg == NULL){
		printf("paramCount = %d, %s\n", paramCount, "传参正确");
	}else{
		if(oneArg == NULL && cn < paramCount){
			printf("paramCount = %d, cn = %d, %s\n", paramCount, cn, "实参太少");
		}	
		if(oneArg != NULL && cn == paramCount){
			printf("paramCount = %d, cn = %d, %s\n", paramCount, cn, "实参太多");
		}
	} 

	return args;
}

AstExpression CheckFunctionCall(AstExpression expr)
{
	char *funcName = (char *)(expr->kids[0]->val.p);
	AstArguments args = (AstArguments)expr->kids[1];
	int op = expr->kids[0]->op;
	char *prefix = NULL;
	if(op == OP_ID){

	}else if(op == OP_MEMBER || op == OP_METHOD){
		prefix = (char *)(expr->kids[0]->kids[0]->val.p);
	}else{
		ERROR("%s\n", "CheckFunctionCall ");
	}

	FunctionSymbol fsym = NULL;

	if(prefix){
		VariableSymbol sym = LookupID(prefix);
		char *receiverTypeAlias = sym->typeAlias;
		if(sym == NULL){
			ERROR("%s\n", "CheckFunctionCall 接口变量未定义");
		}
		if(sym->idata == NULL){
			ERROR("%s\n", "CheckFunctionCall 接口变量没有赋值");
		}

		// 检查被调用的方法是不是接口的方法。

		// 查询接口方法的符号
		fsym = LookupMethodID(funcName, receiverTypeAlias);
		if(fsym == NULL){
			ERROR("%s\n", "CheckFunctionCall 没有实现这个方法");
		}

	}else{
		FunctionSymbol fsym = LookupID(funcName);
		if(fsym == NULL){
			ERROR("%s\n", "CheckFunctionCall 没有实现这个函数");
		}

	}

check_call:
	printf("%s\n", "Start to check call");
	if(fsym){
		expr->kids[0] = fsym;	
	}
	Signature sig = ((FunctionType)fsym->ty)->sig;
	SignatureElement *params = sig->params;
	int paramCount = sig->paramSize;
	args = CheckArguments(args, params, paramCount);	
	expr->kids[1] = args;

	return expr;
}

AstExpression CheckFunctionCall2(AstExpression expr)
{
	char *funcName = (char *)(expr->kids[0]->val.p);
	char *prefix = NULL;
	int op = expr->kids[0]->op;
	if(op == OP_ID){
		// TODO 为了逻辑更清晰，保留这个空分支。
	}else if(op == OP_MEMBER || op == OP_METHOD){
		// expr->kids[0]->kids[0] 是一个表达式，不知道怎么处理
		// TODO 暂时只处理只有一个点号的情况。
		prefix = (char *)(expr->kids[0]->kids[0]->val.p);
	}else{
		ERROR("%s\n", "CheckFunctionCall ");
	}

	int type = -1;
	if(prefix == NULL){
		type = 4;
		goto start;	
		// return expr;
	}

	if(IsTypeName(prefix) == 1){
		type = 1;
	}else{
		Symbol sym = LookupID(prefix);
		if(sym == NULL){
			ERROR("%s\n", "CheckFunctionCall 不存在这个接口或接收者");
		}else{
			if(sym->ty == INTERFACE){
				type = 2;
			}else{
				type = 3;
			}
		}
	}

start:

	if(type == 1){

	}else if(type == 2){

	}else if(type == 3){

	}else{
		ERROR("%s\n", "CheckFunctionCall 错误的type");
	}

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
