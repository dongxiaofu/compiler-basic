#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"

Symbol TranPrimaryExpression(AstExpression expr)
{

	return expr->val.p;
}

Symbol TranMemberAccess(AstExpression expr)
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
	Symbol baseAddr = Addressof(TranExpression(p));

	// 获取对应内存中的数据。
	Symbol dst = Offset(expr->ty, baseAddr, 0, coff);

	return dst;
}

Symbol TranExpression(AstExpression expr)
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

Symbol TranIncDecExpression(AstExpression expr)
{
	AstExpression canAsign = expr->kids[0];
	// 写成AstExpression c = canAsign->kids[0]，行不行？
	AstExpression c = TranExpression(canAsign->kids[0]);

	Symbol ret = CreateTemp(canAsign->ty);
	// 生成一条中间码：
	// 1. dst--ret
	// 2. op--Assign
	// 3. src1--c
	// 翻译canAsign
	TranExpression(canAsign);

	return ret;
}
