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

	// if(expr->op == OP_STR && (expr->isarray == 1)){
	// TODO 存在关于位域的疑难问题。
	// if(expr->op == OP_STR || expr->isarray == 1){
	if(expr->op == OP_STR || expr->isarray){

		return Addressof(expr->val.p);
	}

//	if(expr->op == OP_ID){
//		p = (Symbol)LookupID(expr->val.p);
//		if(p == NULL){
//			// TODO 这种情况怎么办？
//			assert(p != NULL);
//		}else{
//			expr->ty = p->ty;
//		}
//		expr->val.p = p;
//	}

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
//	Symbol baseAddr = TranslateExpression(p);

	// 获取对应内存中的数据。
//	Symbol dst = Offset(expr->ty, baseAddr, 0, coff);
	// TODO UCC不是这样做的。我先这样简化处理吧。
	Symbol dst = Offset(expr->ty, AsVar(baseAddr)->def->src1, 0, coff);
//	Symbol dst = Offset(expr->ty, p, 0, coff);

	return dst;
}

Symbol TranslateExpression(AstExpression expr)
{
//	assert(expr != NULL);
	return (ExprCheckers[expr->op])(expr);
}

Symbol Addressof(Symbol sym)
{
	// 获取def链表
	int h = ((int)sym + (int)ADDR + (int)0) % FSYM_VALUE_DEF_TABLE_SIZE;
	ValueDef def = FSYM->valueDefTable[h];
	// 遍历def链表
	while(def){
		// TODO 我记得UCC不是这样判断的。可是，我根据现在的具体情况写出这样的代码也不错。
		if(def->opcode == ADDR && def->src1 == sym)	break;
		def = def->link;
	}
	
	if(def && def->dst != NULL)	return def->dst;

	// 内地地址的type是指针。
	Symbol tmp = CreateTemp(T(POINTER));
	// 生成一条中间码，tmp是dst，OP是ADDR，src1是sym。
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = T(POINTER);
	irinst->opcode = ADDR;
	irinst->opds[0] = tmp;
	irinst->opds[1] = sym;

	AppendIRInst(irinst);

	DefineTemp(tmp, tmp->ty, ADDR, sym, NULL);

//	AsVar(tmp)->def->link = def;	
//	FSYM->valueDefTable[h] = AsVar(tmp)->def;
	// TODO 注意上面的错误写法。这种操作，我还不是非常熟练。
	AsVar(tmp)->def->link = FSYM->valueDefTable[h];
	FSYM->valueDefTable[h] = AsVar(tmp)->def;

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

	DefineTemp(tmp, ty, DEREF, addr, NULL);
	
	return tmp;
}

Symbol Offset(Type ty, Symbol base, int voff, int coff)
{
	VariableSymbol p = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
	p->kind = SK_Offset;
	p->offset = coff;
	p->ty = ty;
	p->link = base;
	// TODO 以后修改成根据base-name的长度自动分配内存。
	char *name = (char *)MALLOC(16);
	sprintf(name, "%s[%d]", (char *)base->name, coff);
	p->name = name;
//	p->val.p = addr;

	return (Symbol)p;
	// 计算最终地址
//	Symbol t1 = CreateTemp(T(POINTER));
//	// 生成一条中间码
//	// 1. dst--tmp
//	// 2. OP--Add
//	// 3. src1--addr
//	// 4. src2--coff
//	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
//	irinst->ty = T(POINTER);
//	irinst->opcode = ADD;
//	irinst->opds[0] = t1;
//	irinst->opds[1] = addr;
////	union value val;
////	val.i[0] = coff;
////	val.i[1] = 0; 
//	// irinst->opds[2] = Constant(T(INT), val);
//	irinst->opds[2] = IntConstant(coff);
//	AppendIRInst(irinst);
//
////	return t1;
//	// 获取对应内存中的数据
//	Symbol t2 = CreateTemp(ty);
//	// 生成一条中间码：
//	// 1. dst--t2
//	// 2. OP--Deref
//	// 3. src1--addr or t1?
//	IRInst irinst2 = (IRInst)MALLOC(sizeof(struct irinst));
//	irinst2->ty = ty;
//	irinst2->opcode = DEREF;
//	irinst2->opds[0] = t2;
//	irinst2->opds[1] = t1;
//	AppendIRInst(irinst2);
//	
//	return t2;
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
	// AstExpression c = TranslateExpression(canAsign->kids[0]);
	Symbol c = TranslateExpression(canAsign);

	Symbol ret = CreateTemp(canAsign->ty);
	IRInst irinst = (IRInst)MALLOC(sizeof(struct irinst));
	irinst->ty = canAsign->ty;
	irinst->opcode = ADD;
	irinst->opds[0] = ret;
	irinst->opds[1] = c;
	// irinst->opds[2] = canAsign->kids[1];
	irinst->opds[2] = expr->kids[1];
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
			{
				sym = Deref(expr->ty, src);
				break;
			}
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
//	AstExpression p = expr->kids[1];
	AstExpression p = expr;
	int coff = 0;

//	while(p){
//	//	coff += p->kids[0]->val.i[0];
//		coff += p->val.i[0];
//		p = p->kids[1];
//	}

	do{
		coff += p->kids[1]->val.i[0];
		p = p->kids[0];
	}while(p->op == OP_INDEX);	

//	Symbol addr = TranslateExpression(expr->kids[0]);	
	Symbol addr = TranslateExpression(p);	
	Symbol dst = Offset(expr->ty, AsVar(addr)->def->src1, NULL, coff);

	return expr->isarray ? Addressof(dst) : dst;
}

Symbol TranslateAssignmentExpression(AstExpression expr)
{
	Symbol dst = TranslateExpression(expr->kids[0]);
	Symbol src = TranslateExpression(expr->kids[1]);

	if(dst->kind == SK_Temp && AsVar(dst)->def->opcode == DEREF){
		Symbol addr = AsVar(dst)->def->src1;
//		GenerateIndirectMove(expr->ty, addr, src);
//		dst = Deref(expr->ty, addr);
		GenerateIndirectMove(addr->ty, addr, src);
		dst = Deref(expr->kids[0]->ty, addr);
	}else{
		// 生成一条中间码
		// 1. op--MOV
		// 2. dst--dst
		// 3. src1--src
		GenerateMov(expr->ty, dst, src);
	}

	return dst;
}

Symbol TranslateFunctionCall(AstExpression expr)
{
	Symbol dst,src1,src2;
//	无返回值的函数。
	dst = NULL;
	src1 = TranslateExpression(expr->kids[0]);

	FunctionSymbol fsym = (FunctionSymbol)src1;

	Symbol param;
	VariableSymbol *resultSymbolNextPtr;
	int isFirstCall = 0;
	VariableSymbol resultHead;

	Symbol paramHead = (Symbol)MALLOC(sizeof(struct symbol));
	Symbol *lastParam = &(paramHead->next);
	// if(FUNCTION_CURRENT->resultsTemp != NULL){
	if(fsym->resultsTemp != NULL){
		resultSymbolNextPtr = &(fsym->resultsTemp->next);
	}else{
		resultHead = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
		resultSymbolNextPtr = &(resultHead->next);
		isFirstCall = 1;
	}
	
	unsigned int symbol_size = sizeof(struct symbol);

	// 实参。
	AstExpression arg = expr->kids[1];
	while(arg != NULL){
		param = (Symbol)TranslateExpression(arg);
		Symbol outter_param = (Symbol)MALLOC(symbol_size);
		outter_param->inner = param;
		*lastParam = outter_param;
		lastParam = &(outter_param->next);

		arg = arg->next;
	}
//	while(arg != NULL){
//		// void *paramCopy;
//		Symbol paramCopy;
//		unsigned int size = 0;
//		if(arg->op == OP_CONST){
//			size = sizeof(struct symbol);
//			paramCopy = (Symbol)MALLOC(size);
//		}else{
//			size = sizeof(struct variableSymbol);
//			paramCopy = (VariableSymbol)MALLOC(size);
//		}
//		param = (Symbol)TranslateExpression(arg);
//		// TODO 如果是max(a,b)，那么应该VariableSymbol。
//		// 如果是max(1,2)，那么应该用什么数据类型？我不知道。先这样吧。
//		memcpy(paramCopy, param, size);
//		*lastParam = paramCopy; 
//		lastParam = &(paramCopy->next);
//
//		arg = arg->next;
//	}

	// 破坏环形链表。
	// 不在需要了。
//	*lastParam = NULL;

	AstExpression result = expr->receiver;
	VariableSymbol resultSymbol;

//	FunctionSymbol fsym = (FunctionSymbol)src1;
	// 这是一个因粗心导致的错误。也不完全是因为粗心，写到这里，忘记了FSYM的含义。
//	AstExpression receiver = FSYM->receivers;
	// 函数返回值声明
	Symbol receiver = fsym->receivers;
//	while(receiver != NULL){
//		AstNode val = (AstNode)(result->val.p);
//
//		if(val->kind == SK_Variable){
//			resultSymbol = (VariableSymbol)(result->val.p);
//		}else{
//			resultSymbol = CreateTemp(receiver->ty);
//			resultSymbol->kind = SK_Variable;
//			resultSymbol->name = (char *)(result->val.p);
//		}
//		*resultSymbolNextPtr = resultSymbol;
//		resultSymbolNextPtr = &(resultSymbol->next);
//
//		result = (AstExpression)result->next;
//		receiver = receiver->next;
//	}
	
	Symbol returnSym = NULL;
	// 统一用临时变量接收函数的返回值。
	// 这是什么？不添加到实参链表中，是最终接收返回值的临时变量。
	while(receiver != NULL){
		resultSymbol = CreateTemp(receiver->ty);
		if(returnSym == NULL){
			returnSym = resultSymbol;
		}
		resultSymbol->kind = SK_Variable;

		Symbol outter_param = (Symbol)MALLOC(symbol_size);
		outter_param->inner = resultSymbol;

		*resultSymbolNextPtr = outter_param;
		// resultSymbolNextPtr = &(resultSymbol->next);
		resultSymbolNextPtr = &(outter_param->next);

		receiver = receiver->next;
	}

	// TODO 原本用来接收函数的返回值的变量怎么办？
	// TODO 这里很容易弄错。我看了半天才把这里改正确。
//	fsym->resultsTemp = (Symbol)resultsTempHead;
//	fsym->resultsTemp = (Symbol)resultHead->next;
	if(isFirstCall){
		fsym->resultsTemp = (Symbol)resultHead->next;
	}

	// 有多少个返回值就新增多少个变量。
	// 函数返回值声明
	// 这是什么？接收函数返回值的参数。它们本来不是函数的参数，是为了接收函数的返回值新增的参数。
	// 前面那个循环创建的链表，
	receiver = fsym->receivers;
	Symbol receiverSym;
	while(receiver != NULL){
		receiverSym = CreateParam(receiver->ty);
		receiverSym->kind = SK_Variable;

		Symbol outter_param = (Symbol)MALLOC(symbol_size);
		outter_param->inner = receiverSym;
		*lastParam = outter_param;
		lastParam = &(outter_param->next);
		receiver = receiver->next;
	}

	GenerateFunctionCall(T(INT), dst, src1, paramHead->next);

	// 无返回值的函数的调用，返回值应该是什么？
//	return dst;

//	return (Symbol)fsym;
//	不能直接返回函数，而是返回用来接收函数的返回值的那批参数。
//	很需要直接知道这批参数的开头位置。可我不想现在去改，想快点看到结果。
//	TODO 以后再优化。
//	return fsym->resultsTemp->inner;	
	return returnSym;
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
				// 前面被注释的两行，在调试过程中，为了满足后面的模块的需要，改成了第三行。
				// GenerateBranch(T(BOOLEAN), trueBB, OPMAPS[expr->op], src1, src2);
				// GenerateBranch(src1->ty, trueBB, OPMAPS[expr->op], src1, src2);
				// src2和src1的顺序由cmp指令和jge指令的语法决定。
				GenerateBranch(src1->ty, trueBB, OPMAPS[expr->op], src2, src1);
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
		case OP_MEMBER:
			sym = TranslateMemberAccess(expr);
			break;	
		default:
			{
				ERROR("%s\n", "还不知道怎么处理\n");

				break;
			}
		
	}

}
