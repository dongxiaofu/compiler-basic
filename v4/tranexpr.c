#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "gen.h"
#include "expr.h"
#include "declchk.h"
#include "exprchk.h"
#include "tranexpr.h"

static int OPMAPS[] = {
#define OPINFO(op, prec, name, func, opcode)    opcode,
#include "opinfo.h"
#undef OPINFO
};

// Symbol (*ExprCheckers)[](AstExpression) = {
//Symbol (*ExprCheckers[])(AstExpression) = {
// tranexpr.c:19:10: error: declaration of 'ExprCheckers' as array of functions
// Symbol *(ExprCheckers[])(AstExpression) = {
static Symbol (*ExprCheckers[])(AstExpression) = {
// static Symbol (* ExprTrans[])(AstExpression)
#define OPINFO(op, prec, name, func, opcode)    Translate##func##Expression,
#include "opinfo.h"
#undef OPINFO
};

Symbol TranslatePrimaryExpression(AstExpression expr)
{
	if(expr->op == OP_CONST){
//		Symbol tmp = CreateTemp(expr->ty);
//		tmp->kind = SK_CONSTANT;
//		tmp->val = expr->val;
//
//		return tmp;
		return IntConstant(expr->val.i[0]);
	}

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
//	assert(expr != NULL);
	return (ExprCheckers[expr->op])(expr);
}

Symbol Addressof(Symbol sym)
{
	// 内地地址的type是指针。
	Symbol tmp = CreateTemp(T(POINTER));
	// 生成一条中间码，tmp是dst，OP是ADDR，src1是sym。
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = T(POINTER);
	irinst->opcode = ADDR;
	irinst->opds[0] = tmp;
	irinst->opds[1] = sym;

	AppendIRInst(irinst);

	return tmp;
}

Symbol Deref(Type ty, Symbol addr)
{
	Symbol tmp = CreateTemp(ty);
	// 生成一条中间码：
	// 1. dst--tmp
	// 2. OP--Deref
	// 3. src1--addr
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = ty;
	irinst->opcode = DEREF;
	irinst->opds[0] = tmp;
	irinst->opds[1] = addr;

	AppendIRInst(irinst);	
	
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
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = T(POINTER);
	irinst->opcode = ADD;
	irinst->opds[0] = t1;
	irinst->opds[1] = addr;
	union value val;
	val.i[0] = coff;
	val.i[1] = 0; 
	irinst->opds[2] = Constant(T(INT), val);
	AppendIRInst(irinst);
	// 获取对应内存中的数据
	Symbol t2 = CreateTemp(ty);
	// 生成一条中间码：
	// 1. dst--t2
	// 2. OP--Deref
	// 3. src1--addr or t1?
	IRInst irinst2 = (IRInst)MALLOC(sizeof(struct irinst));
	irinst2->ty = ty;
	irinst2->opcode = DEREF;
	irinst2->opds[0] = t2;
	irinst2->opds[1] = t1;
	AppendIRInst(irinst2);
	
	return t2;
}

/**
 * golang支持a++，不支持++a，不支持 c := a++。
 * a++只能作为stmt，不能作为expr。什么意思？
 * stmt不能作为赋值语句的一部分。
 * a++在本函数中的处理方法如下：
 * 1. a++
 * 2. a = a+1
 * 3. 先计算a+1，然后把结果赋值给ret。
 * 4. 再把ret赋值给a。
 */
Symbol TranslateIncDecExpression(AstExpression expr)
{
	AstExpression canAsign = expr->kids[0];
	// 写成AstExpression c = canAsign->kids[0]，行不行？
	AstExpression c = TranslateExpression(canAsign->kids[0]);

	Symbol ret = CreateTemp(canAsign->ty);
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = canAsign->ty;
	irinst->opcode = ADD;
	irinst->opds[0] = ret;
	irinst->opds[1] = c;
	irinst->opds[2] = canAsign->kids[1];
	AppendIRInst(irinst);
	// 生成一条中间码：
	// 1. dst--c
	// 2. op--MOV
	// 3. src1--ret
	IRInst irinst2 = (IRInst)MALLOC(sizeof(struct irinst));
	irinst2->ty = canAsign->ty;
	irinst2->opcode = MOV;
	irinst2->opds[0] = c;
	irinst2->opds[1] = ret;
	AppendIRInst(irinst2);
	// 翻译canAsign
	// 有必要吗？
//	TranslateExpression(canAsign);

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
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = ty;
	irinst->opcode = MOV;
	irinst->opds[0] = t;
	irinst->opds[0] = src;
	AppendIRInst(irinst);
	
	return t;
}

Symbol TranslateUnaryExpression(AstExpression expr)
{
	Symbol sym;
	Symbol src = TranslateExpression(expr->kids[0]);
	int op = expr->op;
	expr->ty = expr->kids[0]->ty;
	
	if(op == OP_NOT){
		return TranslateBranchExpression(expr);
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
			{
				int opcode = OPMAPS[expr->op];
				sym = CreateTemp(expr->ty);
				GenerateAssign(expr->ty,opcode,sym, src, NULL);  	
				break;
			}
		default:
			printf("%s\n", "TranslateUnaryExpression default");
			break;
	}

	return sym;
}

Symbol TranslateArrayIndex(AstExpression expr)
{
//	AstExpression p = TranslateExpression(expr->kids[1]);
	AstExpression p = expr->kids[1];
	int coff = 0;

	while(p){
		coff += p->kids[0]->val.i[0];
		p = p->kids[1];
	}

	Symbol addr = TranslateExpression(p);	
	Symbol dst = Offset(expr->ty, addr, NULL, coff);

	return expr->isarray ? Addressof(dst) : dst;
}

Symbol TranslateAssignmentExpression(AstExpression expr)
{
	Symbol dst = TranslateExpression(expr->kids[0]);
	Symbol src = TranslateExpression(expr->kids[1]);

	// 生成一条中间码
	// 1. op--MOV
	// 2. dst--dst
	// 3. src1--src
	GenerateMov(expr->ty, dst, src);

	return dst;
}

Symbol TranslateFunctionCall(AstExpression expr)
{
	Symbol dst,src1,src2;
//	无返回值的函数。
	dst = NULL;
	src1 = TranslateExpression(expr->kids[0]);

	Symbol param;
	Symbol paramHead = (Symbol)MALLOC(sizeof(struct symbol));
	Symbol *lastParam = &(paramHead->next);

	AstExpression arg = expr->kids[1];
	while(arg != NULL){
		param = (Symbol)TranslateExpression(arg);
		*lastParam = param;
		lastParam = &(param->next);

		arg = arg->next;
	}

	AstExpression result = expr->receiver;
	VariableSymbol resultHead = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
	VariableSymbol resultSymbol;
	VariableSymbol *resultSymbolNextPtr = &(resultHead->next);

	FunctionSymbol fsym = (FunctionSymbol)src1;
//	fsym->results = (Symbol)resultHead->next;
	// 这是一个因粗心导致的错误。也不完全是因为粗心，写到这里，忘记了FSYM的含义。
//	AstExpression receiver = FSYM->receivers;
	Symbol receiver = fsym->results;
	Symbol receiverSym;
	while(receiver != NULL){
		receiverSym = CreateParam(receiver->ty);
		receiverSym->kind = SK_Variable;
		*lastParam = receiverSym;
		lastParam = &(receiverSym->next);

		resultSymbol = CreateTemp(receiver->ty);
		resultSymbol->kind = SK_Variable;
		resultSymbol->name = (char *)(result->val.p);
		*resultSymbolNextPtr = resultSymbol;
		resultSymbolNextPtr = &(resultSymbol->next);

		result = result->next;
		receiver = receiver->next;
	}
	fsym->results = (Symbol)resultHead->next;

	GenerateFunctionCall(T(INT), dst, src1, paramHead->next);

	// 无返回值的函数的调用，返回值应该是什么？
	return dst;
}

AstExpression Not(AstExpression expr)
{
	int rops[] = {OP_NOT_EQUAL, OP_EQUAL, OP_GREATER_OR_EQUAL, OP_GREATER, OP_LESS_OR_EQUAL, OP_LESS};

	switch(expr->op){
		case OP_CONDITIONAL_AND:
			{
				expr->kids[0] = Not(expr->kids[0]);				
				expr->kids[1] = Not(expr->kids[1]);				
				expr->op = OP_CONDITIONAL_OR;
				break;
			}
		case OP_CONDITIONAL_OR:
			{
				expr->kids[0] = Not(expr->kids[0]);				
				expr->kids[1] = Not(expr->kids[1]);				
				expr->op = OP_CONDITIONAL_AND;
				break;
			}
		case OP_EQUAL: 
		case OP_NOT_EQUAL:
		case OP_LESS:
		case OP_LESS_OR_EQUAL:
		case OP_GREATER:
		case OP_GREATER_OR_EQUAL:
			{
				expr->op = rops[expr->op - OP_EQUAL];
				break;
			}
		case OP_NOT:
			{
				expr = expr->kids[0];	
				break;
			}
		default:
			{
				// TODO 为什么不是expr->ty == BOOLEAN？
				// 这里不能确定是expr->kids[0]->ty还是expr->ty。
				// 到时候看本函数的expr的具体数值。
				if(expr->kids[0]->ty == BOOLEAN){
					expr->kids[0]->val.boolean = !(expr->kids[0]->val.boolean);
				}
				break;
			}
	}

	return expr;
}

void TranslateBranch(AstExpression expr, BBlock trueBB, BBlock falseBB)
{
	BBlock restBB;
	Symbol src1, src2;

	switch(expr->op){
		case OP_CONDITIONAL_AND:
			{
				restBB = CreateBBlock();
				TranslateBranch(Not(expr->kids[0]), falseBB, restBB);
				StartBBlock(restBB);
				TranslateBranch(expr->kids[1], trueBB, falseBB);
				break;
			}
		case OP_CONDITIONAL_OR:
			{
				restBB = CreateBBlock();
				TranslateBranch(expr->kids[0], trueBB, restBB);
				StartBBlock(restBB);
				TranslateBranch(expr->kids[1], trueBB, falseBB);
				break;
			}
		case OP_NOT:
			{
				AstExpression kid0 = expr->kids[0];
				int count = 0;
				while(kid0 != NULL && kid0->op == OP_NOT){
					count++;
					kid0 = expr->kids[0];
				}	
				src1 = TranslateExpression(kid0);
				if(count % 2 == 0){
					GenerateBranch(kid0->ty, trueBB, JNZ, src1, NULL);
				}else{
					GenerateBranch(kid0->ty, trueBB, JZ, src1, NULL);
				}

				break;
			}
		case OP_EQUAL:
		case OP_NOT_EQUAL:
		case OP_LESS:
		case OP_LESS_OR_EQUAL:
		case OP_GREATER:
		case OP_GREATER_OR_EQUAL:
			{
				src1 = TranslateExpression(expr->kids[0]);
				src2 = TranslateExpression(expr->kids[1]);
				GenerateBranch(T(BOOLEAN), trueBB, OPMAPS[expr->op], src1, src2);
				break;
			}
		default:
			{
				src1 = TranslateExpression(expr->kids[0]);
				if(src1->kind == SK_BOOLEAN){
					if(src1->val.boolean == 1){
						// TODO 把参数写成src1，这是一个多么可笑的错误啊。
						// GenerateJmp(src1);
						GenerateJmp(trueBB);
					}
				}
				break;
			}
	}
}

Symbol TranslateBranchExpression(AstExpression expr)
{
	Symbol tmp = CreateTemp(T(POINTER));
	BBlock trueBB = CreateBBlock();
	BBlock falseBB = CreateBBlock();
	BBlock nextBB = CreateBBlock();

	TranslateBranch(expr, trueBB, falseBB);
	StartBBlock(falseBB);
	Symbol falseSym = (Symbol)MALLOC(sizeof(struct symbol));
	union value val;
	val.boolean = 0;
	falseSym->val = val;
	GenerateMov(T(BOOLEAN), tmp, falseSym);
	GenerateJmp(nextBB);

	StartBBlock(trueBB);
	Symbol trueSym = (Symbol)MALLOC(sizeof(struct symbol));
	union value val2;
	val2.boolean = 1;
	trueSym->val = val2;
	GenerateMov(T(BOOLEAN), tmp, trueSym);

	StartBBlock(nextBB);

	return tmp;
}

Symbol TranslateBinaryExpression(AstExpression expr)
{
	if(expr->op == OP_CONDITIONAL_OR || expr->op == OP_CONDITIONAL_AND || OP_EQUAL <= expr->op && expr->op <= OP_GREATER_OR_EQUAL){
		return TranslateBranchExpression(expr);
	}

	Symbol src1, src2, dst;
	src1 = TranslateExpression(expr->kids[0]);
	src2 = TranslateExpression(expr->kids[1]);

	dst = CreateTemp(expr->ty);
	int opcode = OPMAPS[expr->op];
	GenerateAssign(expr->ty, opcode, dst, src1, src2);	

	return dst;
}

Symbol TranslatePostfixExpression(AstExpression expr)
{
	Symbol sym;
	int op = expr->op;
	switch(op){
		case OP_INDEX:
			sym = TranslateArrayIndex(expr);
			break;	
		case OP_CALL:
			sym = TranslateFunctionCall(expr);
			break;
		default:
			{
				ERROR("%s\n", "还不知道怎么处理\n");

				break;
			}
		
	}

}
