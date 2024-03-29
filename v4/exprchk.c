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
	Type ty;
//	expr->kids[0] = Adjust(CheckExpression(expr->kids[0]));
//	expr->kids[1] = Adjust(CheckExpression(expr->kids[1]));
	expr->kids[0] = CheckExpression(expr->kids[0]);
	expr->kids[1] = CheckExpression(expr->kids[1]);

	expr->kids[0] = Adjust(expr->kids[0]);
	expr->kids[1] = Adjust(expr->kids[1]);
	expr->ty = expr->kids[0]->ty;

	return expr;
}

AstExpression FoldConstant(AstExpression expr)
{
	union value val;
	AstExpression kid0, kid1;

	val.i[0] = val.i[1] = 0;
	kid0 = expr->kids[0];
	kid1 = expr->kids[1];

	assert(kid0 != NULL);
	assert(kid1 != NULL);

	switch(expr->op){
		case OP_MUL:
			{
				val.i[0] = kid0->val.i[0] * kid1->val.i[0];
				break;
			}
		default:
			{
				printf("%s--%s--%d\n", __FILE__, __BASE_FILE__, __LINE__);
				exit(-1);
			}
	}

	return Constant(expr->kids[0]->ty, val);
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

	return FoldConstant(expr);
}

AstExpression Adjust(AstExpression expr)
{
	// TODO 为什么会有这一段代码？处理数据时出现错误，为了避免错误，我加上的。
	// 从领域逻辑上考虑，我确实不知道为什么expr的ty会是NULL。
	if(expr->ty == NULL){
		return expr;
	}

	if(expr->ty->categ == ARRAY){
		expr->ty = PointerTo(expr->ty->bty);
		// TODO 写出了这样可笑的代码。疏忽。
		// expr->ty->isarray = 1;
		expr->isarray = 1;
	}


	return expr;
}

AstExpression CheckPrimaryExpression(AstExpression expr)
{
//	if(strcmp(expr->val.p, "_") == 0)	return expr;

	if(expr->op == OP_CONST){
		return expr;
	}

	if(expr->op == OP_STR){
		if(strcmp(expr->val.p, "_") == 0)	return expr;
		expr->op = OP_ID;
		expr->val.p = AddString(expr->ty, (String)(expr->val.p));
		// TODO 不知道有什么用。
		expr->lvalue = 1;
	
		return expr;
	}

	Symbol p;
	
	p = LookupID(expr->val.p);	
	if(p == NULL){
//		ERROR("%s:%s\n", "CheckPrimaryExpression error:变量未定义。", expr->val.p);
		printf("%s:%s\n", "CheckPrimaryExpression error:变量未定义。", expr->val.p);
		exit(-2);
	}else{
		expr->ty = p->ty;
		expr->val.p = p;
	}


	return expr;
}

AstExpression CheckMemberAccess(AstExpression expr)
{
	expr->kids[0] = CheckExpression(expr->kids[0]);
	
	// Field fld = LookupField(expr->val.p, expr->kids[0]->val.p);	
	Symbol sym = (Symbol)(expr->kids[0]->val.p);
	if(sym == NULL){
		// TODO 需优化这种错误信息。
		printf("there is no struct in file:%s in line:%d\n", __BASE_FILE__, __LINE__);
		exit(-1);
	}

	Field fld = LookupField(expr->val.p, sym->name);	
	if(fld == NULL){
		// TODO 需优化这种错误信息。
		printf("%s has no member %s in file:%s in line: %d\n", sym->name, expr->val.p, __BASE_FILE__, __LINE__);
		exit(-1);
	}
	expr->ty = fld->ty;
	expr->val.p = fld;

	return expr;
}

AstExpression CheckTypeAssert(AstExpression expr)
{
	printf("start CheckTypeAssert\n");

	int typeCheckResult = 1;

	AstExpression kids0 = expr->kids[0];
	char *valName = (char *)(kids0->val.p);
	VariableSymbol sym = LookupID(valName);
	if(sym == NULL){
		ERROR("%s\n", "CheckTypeAssert 变量未定义");
	}
	if(sym->ty->categ != INTERFACE){
		ERROR("%s\n", "CheckTypeAssert 不是接口变量");
	}	

	// 开始检查i.(T)中的T
	AstSpecifiers spec = expr->kids[1];
	// 各种奇形怪状的数据结构，不翻看以前的代码，很难写出正确的代码。
	AstNode tySpecs = spec->tySpecs;
	if(spec->typeAlias == ""){
		if(tySpecs->kind != NK_TypedefName){
			ERROR("%s\n", "CheckTypeAssert Type只能是alias或基础数据类型");
		}
	}

	char *name;
	if(tySpecs && tySpecs->kind == NK_TypedefName){
		AstTypedefName tname = (AstTypedefName)tySpecs;
		name = tname->id;
		if(IsDataType(name) != 1){
			ERROR("%s\n", "CheckTypeAssert i.(T)中的T必须是数据类型");
		}	
	}else{
		name = spec->typeAlias;
	}
	// 检查T是不是接口名
	InterfaceType ity = INTERFACE_LIST;
	int isInterface = 0;
	while(ity != NULL){
		if(ity->alias && name && strcmp(ity->alias, name) == 1){
			isInterface = 1;
		}
		ity = ity->next;
	}
		
	// 检查T是不是实现了一个接口
	if(isInterface == 0){
		FunctionSymbol fsym = FUNCTION_LIST;
		int hash_table_size = 127 + 1;
		int bucketLinkerSize = sizeof(struct bucketLinker);
		// variable-sized object may not be initialized
		// BucketLinker table[hash_table_size] = (BucketLinker *)MALLOC(bucketLinkerSize * hash_table_size);
		// error: invalid initializer
		// BucketLinker table[128] = (BucketLinker *)MALLOC(bucketLinkerSize * hash_table_size);
		// 检查一个数据结构是否实现了一个接口
		typedef struct plusBucket{
			int count;
			char *receiver;
			BucketLinker linker;
			struct plusBucket *next;
		} *PlusBucket;
		PlusBucket plusBucket = (PlusBucket)MALLOC(sizeof(struct plusBucket));
		PlusBucket headPlusBucket,lastPlusBucket;
		headPlusBucket = lastPlusBucket = plusBucket;
		BucketLinker *table = (BucketLinker *)MALLOC(bucketLinkerSize * hash_table_size);
		int dataTypeCount = 0;
		while(fsym != NULL){
			FunctionType fty = (FunctionType)fsym->ty;
			SignatureElement receiver = fty->sig->receiver;
			BucketLinker linker = (BucketLinker)MALLOC(bucketLinkerSize);
			linker->sym = fsym;
			if(receiver == NULL || receiver->ty == NULL || receiver->ty->alias == NULL){
				fsym = fsym->next;
				 continue;
			}
			if(plusBucket->receiver == NULL){
				plusBucket->count++;
				plusBucket->receiver = receiver->ty->alias;
				plusBucket->linker = linker;
			}else{
				PlusBucket currentPlusBucket = headPlusBucket;	
				while(currentPlusBucket != NULL){
					if(strcmp(plusBucket->receiver, receiver->ty->alias) == 0){
						break;
					}
					currentPlusBucket = currentPlusBucket->next;
				}

				if(currentPlusBucket != NULL){
					plusBucket = currentPlusBucket;
				}else{
					plusBucket = (PlusBucket)MALLOC(sizeof(struct plusBucket));
					lastPlusBucket->next = plusBucket;
					lastPlusBucket = plusBucket;
				}

				plusBucket->count++;
				plusBucket->receiver = receiver->ty->alias;
				plusBucket->linker = linker;
			}

			fsym = fsym->next;
		}

		// 检查一个数据结构是否实现了一个接口
		PlusBucket currentPlusBucket = headPlusBucket;
		InterfaceType currentIty = sym->ty;
		int flag2;
		while(currentPlusBucket != NULL){
			if(strcmp(currentPlusBucket->receiver, name) == 0){
				flag2 = 1;
				if(currentIty->methodCount == currentPlusBucket->count){
					BucketLinker current = currentPlusBucket->linker;
					AstMethodSpec method = currentIty->methods;
					int flag;// = 0;
					while(method != NULL){
						flag = 0;
						while(current != NULL){
							// 比较两个函数是否相同
							AstDeclarator funcName = method->funcName;
							FunctionSymbol fsym = (FunctionSymbol)(current->sym);
							if(funcName == NULL){
								break;
							}
							if(funcName->id = NULL){
								break;
							}
							if(strcmp(funcName->id, fsym->name) == 0){
								flag = 1;
								break;
							}

							current = current->link;
						}
						if(flag == 0){
							flag2 = 0;
							break;
						}
						method = method->next;
					}
				}
			}else{
			//	flag2 = 0;
			}
			// 只需要检查一个数据类型是不是实现了这个接口。所以，只需要找到一个符合条件的就可以了。
			if(flag2 == 1){
				break;
			}	
			currentPlusBucket = currentPlusBucket->next;
		}

		if(flag2 == 0){
	//		ERROR("%s\n", "CheckTypeAssert T没有实现当前接口");
			printf("%s\n", "CheckTypeAssert T没有实现当前接口");
			typeCheckResult = 0;
		}
	}

	union value val;
	val.i[0] = typeCheckResult;
	val.i[1] = 0;
	expr->kids[1] = Constant(T(INT), val);	
	expr->kids[0] = sym;

	printf("end CheckTypeAssert\n");
	return expr;
}

AstArguments CheckArguments(AstArguments args, SignatureElement *params, int paramCount)
{
	// 检查参数的数据类型
	AstExpression oneArg = args->args;
	AstExpression firstArg = NULL;
	AstExpression newArg = NULL;
	int cn = 0;
	for(int i = 0; i < paramCount; i++){
		if(oneArg == NULL){
			break;
		}

		oneArg = CheckExpression(oneArg);
		CREATE_AST_NODE(newArg, Expression);
		*newArg = *oneArg;
	
		if(firstArg == NULL){
			newArg->next = NULL;
		//	firstArg = oneArg;
		}else{
			newArg->next = firstArg;
//			firstArg->next = newArg;
		}
		firstArg = newArg;
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

	args->args = firstArg;

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
		printf("%s, 函数名是：%s\n", "CheckFunctionCall 普通函数调用", funcName);
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
		fsym = (FunctionSymbol)LookupID(funcName);
		if(fsym == NULL || fsym->ty->categ != FUNCTION){
			printf("funcName = %s\n", funcName);
			ERROR("%s\n", "CheckFunctionCall 没有实现这个函数");
		}

		// 对接收返回值的变量进行处理
		Symbol result = fsym->receivers;
		AstExpression receiver = expr->receiver; 
		while(result != NULL && receiver != NULL){
			// 如果用事先声明了变量接收返回值，将满足这个条件。
			if(receiver->ty != NULL)	continue;
			receiver->ty = result->ty;

			result = result->next;
			receiver = receiver->next;
		}	

		if(expr->receiver != NULL  && (receiver != NULL || result != NULL)){
			ERROR("%s\n", "CheckFunctionCall 接收返回值的变量和返回值的数据不匹配");
		}

//		fsym->receivers = expr->receiver;
	}

check_call:
	printf("%s\n", "Start to check call");
	if(fsym){
//		expr->kids[0] = fsym;	
	}
	expr->kids[0] = CheckExpression(expr->kids[0]);
	Signature sig = ((FunctionType)fsym->ty)->sig;
	SignatureElement *params = sig->params;
	int paramCount = sig->paramSize;
	args = CheckArguments(args, params, paramCount);	
	expr->kids[1] = args->args;

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
				// expr->kids[1] = ScalePointerOffset(expr->kids[0],expr->ty->size);
				expr->kids[1] = ScalePointerOffset(expr->kids[1],expr->ty->size);
				return expr;
			}
			
			ERROR("line:%d\n", __LINE__);
		case OP_DOT:	
		case OP_MEMBER:
			expr = CheckMemberAccess(expr);
			break;
		case OP_TYPE_ASSERT:
			expr = CheckTypeAssert(expr);
			break;
		case OP_CALL:
			expr = CheckFunctionCall(expr);
			break;
		case OP_CAST:
			expr = CheckCastExpression(expr);
			break;
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
		current = Adjust(current);
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
// Start
AstExpression CheckCONDITIONAL_OR(AstExpression expr)
{

return expr;
}

AstExpression CheckCONDITIONAL_AND(AstExpression expr)
{

return expr;
}

AstExpression CheckEQUAL(AstExpression expr)
{

return expr;
}

AstExpression CheckNOT_EQUAL(AstExpression expr)
{

return expr;
}

AstExpression CheckLESS(AstExpression expr)
{

return expr;
}

AstExpression CheckLESS_OR_EQUAL(AstExpression expr)
{

return expr;
}

AstExpression CheckGREATER(AstExpression expr)
{

return expr;
}

AstExpression CheckGREATER_OR_EQUAL(AstExpression expr)
{

return expr;
}

AstExpression CheckADD(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

//	if(BothArithType(ty1, ty2)){
//		union value val;
//		val.i[0] = val.i[1] = 0;
//		val.i[0] = expr0->val.i[0] + expr1->val.i[0];
//		return Constant(expr->ty, val);
//	}

	return expr;
}

AstExpression CheckMINUS(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

//	if(BothArithType(ty1, ty2)){
//		union value val;
//		val.i[0] = val.i[1] = 0;
//		val.i[0] = expr0->val.i[0] - expr1->val.i[0];
//		return Constant(expr->ty, val);
//	}

return expr;
}

AstExpression CheckBINARY_BITWISE_OR(AstExpression expr)
{

return expr;
}

AstExpression CheckBINARY_BITWISE_XOR(AstExpression expr)
{

return expr;
}

AstExpression CheckMUL(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

return expr;
}

AstExpression CheckDIV(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

return expr;
}

AstExpression CheckMOD(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

return expr;
}

// a << 5
AstExpression CheckLSF(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

return expr;
}

// a >> 5
AstExpression CheckRSF(AstExpression expr)
{
	Type ty1,ty2;
	AstExpression expr0 = expr->kids[0];
	AstExpression expr1 = expr->kids[1];
	ty1 = expr0->ty;
	ty2 = expr1->ty;
	
	expr->ty = ty1;

return expr;
}

AstExpression CheckBITWISE_AND(AstExpression expr)
{

return expr;
}

AstExpression CheckBITWISE_AND_NOT(AstExpression expr)
{

return expr;
}

// End

AstExpression (*BinaryOPCheckers[])(AstExpression) = {
	CheckCONDITIONAL_OR,
	CheckCONDITIONAL_AND,
	CheckEQUAL,
	CheckNOT_EQUAL,
	CheckLESS,
	CheckLESS_OR_EQUAL,
	CheckGREATER,
	CheckGREATER_OR_EQUAL,
	CheckADD,
	CheckMINUS,
	CheckBINARY_BITWISE_OR,
	CheckBINARY_BITWISE_XOR,
	CheckMUL,
	CheckDIV,
	CheckMOD,
	CheckLSF,
	CheckRSF,
	CheckBITWISE_AND,
	CheckBITWISE_AND_NOT,
};

AstExpression CheckBinaryExpression(AstExpression expr)
{
	expr->kids[0] = CheckExpression(expr->kids[0]);
	expr->kids[1] = CheckExpression(expr->kids[1]);

	return BinaryOPCheckers[expr->op - OP_CONDITIONAL_OR](expr);
}

// 修改了expr的结构：1. 新增一层结点。2. 新结点是原来的op，旧结点的op修改成+=或-=。
AstExpression CheckIncDecExpression(AstExpression expr)
{
	int op = expr->op == OP_INC ? OP_ADD_ASSIGN : OP_MINUS_ASSIGN;	
	AstExpression root;
	CREATE_AST_NODE(root, Expression);
	root->op = expr->op;
//	root->kids[0] = CheckExpression(expr);
	expr->op = op;
	root->kids[0] = CheckExpression(expr->kids[0]);
	// TODO 这里的val能直接这样用吗？局部变量作为AST的元素，有问题吗？
	union value val;
	val.i[0] = 1;
	val.i[1] = 0;
	// expr->kids[1] = Constant(T(INT), val); 
//	root->kids[1] = Constant(T(INT), val); 
	root->kids[1] = IntConstant(1);
	return root;
}

AstExpression CheckCastExpression(AstExpression expr)
{
	AstSpecifiers spec = (AstSpecifiers)expr->kids[0];
	CheckDeclarationSpecifiers(spec);
	Type ty = spec->ty;
	expr->ty = ty;
	expr->kids[0] = CheckExpression(expr->kids[1]);
	expr->kids[1] = NULL;

	return expr;
}

// 怎么识别一个表达式是不是一元表达式啊？
// 根据表达式的op无法识别出PrimaryExpr
AstExpression CheckUnaryExpression(AstExpression expr)
{
	switch(expr->op){
		case OP_POS:		// +
			expr->kids[0] = CheckExpression(expr->kids[0]);
			break;
		case OP_NEG:		// -
			expr->kids[0] = CheckExpression(expr->kids[0]);
			break;
		case OP_NOT:			// !
			expr->kids[0] = CheckExpression(expr->kids[0]);
			break;
		case OP_BITWISE_XOR:		// ^ 
			expr->kids[0] = CheckExpression(expr->kids[0]);
			break;
		case OP_DEREF:				// *
			expr->kids[0] = CheckExpression(expr->kids[0]);
			break;
		case OP_ADDRESS:			// &
			expr->kids[0] = CheckExpression(expr->kids[0]);
			break;
		default:					// PrimaryExpr
			expr = CheckExpression(expr);
			break;
	}

	return expr;
}
