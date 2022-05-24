#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"

Symbol TranslatePrimaryExpression(AstExpression expr)
{

	return expr->val.p;
}

Symbol TranslateMemberAccess(AstExpression expr)
{
	// 获取结构体成员的内存地址。
	AstExpression p = expr;
	int coff = 0;
	Field fld;

	while(p->op == OP_MEMBER){
		fld = expr->val.p;
		coff += fld->offset;
		p = p->kids[0];
	}
	Symbol baseAddr = Addressof(TranslateExpression(p));

	// 获取对应内存中的数据。
	Symbol dst = Offset(expr->ty, baseAddr, 0, coff);

	return dst;
}

Symbol TranslateExpression(AstExpression expr)
{

	return expr;
}

Symbol Addressof(Symbol sym)
{
	// 内地地址的type是指针。
	Symbol tmp = CreateTemp(T(POINTER));
	// 生成一条中间码，tmp是dst，OP是ADDR，src1是sym。

	return tmp;
}

Symbol Deref(Type ty, Symbol addr)
{
	Symbol tmp = CreateTemp(ty);
	// 生成一条中间码：
	// 1. dst--tmp
	// 2. OP--Deref
	// 3. src1--addr
	
	return tmp;
}

Symbol Offset(Type ty, Symbol addr, int voff, int coff)
{
	// 计算最终地址
	Symbol t1 = CreateTemp(T(POINTER));
	// 生成一条中间码
	// 1. dst--tmp
	// 2. OP--Add
	// 3. src1--addr
	// 4. src2--coff
	// 获取对应内存中的数据
	Symbol t2 = CreateTemp(ty);
	// 生成一条中间码：
	// 1. dst--t2
	// 2. OP--Deref
	// 3. src1--addr
	
	return t2;
}

Symbol TranslateIncDecExpression(AstExpression expr)
{
	AstExpression canAsign = expr->kids[0];
	// 写成AstExpression c = canAsign->kids[0]，行不行？
	AstExpression c = TranslateExpression(canAsign->kids[0]);

	Symbol ret = CreateTemp(canAsign->ty);
	// 生成一条中间码：
	// 1. dst--ret
	// 2. op--Assign
	// 3. src1--c
	// 翻译canAsign
	TranslateExpression(canAsign);

	return ret;
}

Symbol TranslateCastExpression(AstExpression expr)
{
	Type ty = expr->ty;
	Symbol t = CreateTemp(ty);
	Symbol src = TranslateExpression(expr->kids[0]);
	// 生成一条中间码：
	// 1. dst--t
	// 2. op--Assign
	// 3. src1--src
	
	return t;
}

Symbol TranslateUnaryExpression(AstExpression expr)
{
	Symbol sym;
	Symbol src = TranslateExpression(expr->kids[0]);
	int op = expr->op;
	
	if(op == OP_NOT){

	}

	// +a -> 0 + a
	// -a -> 0 - a
	if(op == OP_POS || op == OP_NEG){
		AstExpression p;
		CREATE_AST_NODE(p, Expression);
		p->op = (op == OP_POS ? OP_ADD : OP_MINUS);
		p->kids[1] = expr->kids[0];
		union value *val = (union value *)MALLOC(sizeof(union value));
		val->i[0] = 0;
		val->i[1] = 0;
		p->kids[0] = Constant(T(INT), *val);

		sym = TranslateExpression(expr);

		return sym;
	}

	switch(op){
		case OP_DEREF:
			sym = Deref(expr->ty, src);
			break;
		case OP_ADDRESS:
			sym = Addressof(src);
			break;
		case OP_BITWISE_XOR:        // ^
			printf("%s\n", "不知道怎么实现TranslateUnaryExpression"); 
			break;
		default:
			printf("%s\n", "TranslateUnaryExpression default");
			break;
	}

	return sym;
}

Symbol TranslateArrayIndex(AstExpression expr)
{
	AstExpression p = expr->kids[1];
	int coff = 0;

	while(p){
		coff += p->kids[0]->val.i[0];
		p = p->kids[1];
	}

	Symbol addr = (Symbol)p;	
	Symbol dst = Offset(expr->ty, addr, NULL, coff);

	return expr->isarray ? Addressof(dst) : dst;
}
