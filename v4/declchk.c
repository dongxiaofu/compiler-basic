#include "symbol.h"
#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "stmtchk.h"
#include "declchk.h"
// #include "symbol.h"

// static AstFunction CURRENT;

void CheckTranslationUnit(AstTranslationUnit transUnit)
{
	printf("%s\n", "Start Check");

	AstDeclaration p = transUnit->decls;
	VariableSymbol sym;

	while(p){
		if(p->kind == NK_Function){
			printf("%s\n", "Check function");
			CURRENT = (AstFunction)p;

			CURRENT->loops = (StmtVector)MALLOC(sizeof(struct stmtVector));
			//memset(CURRENT->loops, 0, sizeof(struct stmtVector));
			CURRENT->loops->index = -1;

			CURRENT->breakable = (StmtVector)MALLOC(sizeof(struct stmtVector));
			//memset(CURRENT->breakable, 0, sizeof(struct stmtVector));
			CURRENT->breakable->index = -1;

			CheckFunction((AstFunction)p);
		}else{
			printf("%s\n", "Check global declaration");
			CheckGlobalDeclaration(p);
			
			// TODO 这样是查不到f对应的符号的。因为查询符号使用的是变量的内存地址而不是变量名。
			// sym = LookupID("f");
		}
		p = p->next;
	}

	printf("%s\n", "End Check");
}

void CheckLocalDeclaration(AstDeclaration decls)
{
	CheckDeclaration(decls);
}

int CheckBlock(AstBlock block)
{
	AstStatement stmt = block->stmt;
	AstStatement lastStmt = NULL;
	while(stmt != NULL){
		if(stmt->kind == NK_CompoundStatement){
			PRINTF("check local variable\n");
			AstCompoundStatement compoundStmt = (AstCompoundStatement)stmt;
			// 处理局部变量
			AstDeclaration decls = (AstDeclaration)compoundStmt->decls;
			if(decls){
				CheckLocalDeclaration(decls);
			}
			// 处理其他
			// TODO 待完善
			AstLabelStmt labeledStmt = compoundStmt->labeledStmt;
			AstStatement simpleStmt = compoundStmt->stmts;	
		}else{
			stmt = CheckStatement(stmt);
		}

		if(stmt){
			lastStmt = stmt;
			stmt = stmt->next;
		}
	}

	if(lastStmt->kind == NK_ReturnStatement){
		return 1;
	}

	return 0;
}

void CheckFunction(AstFunction p)
{
	FunctionSymbol fsym;

	AstFunctionDeclarator fdec = p->fdec;
	Signature sig = (Signature)MALLOC(sizeof(struct signature));
	//memset(sig, 0, sizeof(struct signature));
	int paramIndex = sig->paramSize - 1;
	int resultIndex = sig->resultSize - 1;

	AstParameterTypeList paramTyList = fdec->paramTyList;
	AstParameterDeclaration paramDecls = paramTyList->paramDecls;
	AstParameterDeclaration paramDecl = paramDecls;
	while(paramDecl){
		CheckDeclarationSpecifiers(paramDecl->specs);
		AstDeclarator dec = paramDecl->dec;
		SignatureElement sigElement = (SignatureElement)MALLOC(sizeof(struct signatureElement));
		//memset(sigElement, 0, sizeof(struct signatureElement));
		sigElement->id = dec->id;
		sigElement->ty = paramDecl->specs->ty;
		sig->params[++paramIndex] = sigElement;

		paramDecl = paramDecl->next;
	}

	sig->paramSize = paramIndex + 1;

	AstParameterTypeList resultList = fdec->result;
	AstParameterDeclaration resultParamDecls = resultList->paramDecls;
	AstParameterDeclaration resultParamDecl = resultParamDecls;
	while(resultParamDecl){
		CheckDeclarationSpecifiers(resultParamDecl->specs);
		AstDeclarator dec = resultParamDecl->dec;
		SignatureElement sigElement = (SignatureElement)MALLOC(sizeof(struct signatureElement));
		//memset(sigElement, 0, sizeof(struct signatureElement));
		if(dec != NULL){
			sigElement->id = dec->id;
		}
		sigElement->ty = resultParamDecl->specs->ty;
		sig->results[++resultIndex] = sigElement;

		resultParamDecl = resultParamDecl->next;
	}

	sig->resultSize = resultIndex + 1;

	AstDeclarator fname = (AstDeclarator)fdec->dec;
	if((fsym == LookupID(fname->id)) == NULL){
		fsym = AddFunction(fname->id, sig);
		FSYM = fsym;
	}	

	// 检查函数体
	AstBlock block = p->block;
	int hasReturn = CheckBlock(block);	
	CURRENT->hasReturn = hasReturn;
	if(sig->resultSize > 0 && hasReturn == 0){
		// TODO
		ERROR("函数必须有返回值\n", "");
	}
}

void CheckGlobalDeclaration(AstDeclaration decls){
	CheckDeclaration(decls);
}

// TODO 必须加static吗？
// static void CheckGlobalDeclaration(AstDeclaration decl)
// void CheckGlobalDeclaration(AstDeclaration decls)
void CheckDeclaration(AstDeclaration decls)
{
	if(decls->kind == NK_VarDeclaration){
		AstVarDeclaration decl = (AstVarDeclaration)decls;
		int i = 0;
		while(decl){
			AstVarDeclarator declarator = (AstVarDeclarator)decl->decs;	
			while(declarator){
				// 处理说明符
				CheckDeclarationSpecifiers(declarator->specs);
				VariableSymbol sym;
				VariableSymbol sym2;
				// TODO var a,b int = 2,4
				AstInitDeclarator initDec = (AstInitDeclarator)declarator->initDecs;
				// CG 处理a,b
				while(initDec){
					AstDeclarator dec = initDec->dec;
					if((sym = (VariableSymbol)LookupID(dec->id)) == NULL){
						sym = AddVariable(dec->id);
					}	
					// 变量的初始值
					if(initDec->init){
						CheckInitializer((AstInitializer)initDec->init);
						sym->initData = ((AstInitializer)initDec->init)->idata;
					}

					// TODO 测试查询功能，没有作用。
					sym2 = (VariableSymbol)LookupID(dec->id);

					initDec = (AstInitDeclarator)initDec->next;
				}
				declarator = declarator->next;
			}
			decl = decl->next;
			PRINTF("loop %d\n", i++);
		}	
	}else if(decls->kind == NK_ConstDeclaration){

	}else{
		printf("%s\n", "todo");
	}
}

void CheckDeclarationSpecifiers(AstSpecifiers specs)
{
	Type ty;

	if(specs->kind == NK_StructSpecifier){
		// ty = CheckStructSpecifier((AstStructSpecifier)specs->tySpecs);
		ty = CheckStructSpecifier((AstStructSpecifier)specs);
	}else if(specs->kind == NK_ArrayTypeSpecifier){
		ty = CheckArraySpecifier((AstArrayTypeSpecifier)specs);
	}else{
		ty = T(INT);
	}

	specs->ty = ty;
}

RecordType CheckStructSpecifier(AstStructSpecifier specs)
{
	RecordType rty = StartRecord();	
	AstStructDeclarator decl = specs->stDecls;

	while(decl){
		// CheckDeclarationSpecifiers(specs->tySpecs);
		// CheckDeclarationSpecifiers((AstSpecifiers)decl);
		CheckDeclarationSpecifiers((AstSpecifiers)decl->tySpecs);
		// AddField(rty, decl->id, decl->ty);	
		AddField(rty, decl->id, ((AstSpecifiers)(decl->tySpecs))->ty);	
		decl = (AstStructDeclarator)decl->next;
	}

	// specs->ty = rty;
	
	EndRecord(rty);

	return rty;
}

RecordType StartRecord()
{
	RecordType rty = (RecordType)MALLOC(sizeof(struct recordType));
	rty->kind = NK_RecordType;
	rty->id = NULL;
	rty->flds = NULL;
	rty->tail = &(rty->flds);

	return rty;
}

Field AddField(RecordType rty, char *id, Type ty)
{
	int fldSize = sizeof(struct field);
	Field fld = (Field)MALLOC(fldSize);
	//memset(fld, 0, fldSize);

	int idSize = sizeof(char) * MAX_NAME_LEN;
	fld->id = (char *)MALLOC(idSize);
	//memset(fld->id, 0, idSize);
	strcpy(fld->id, id);

	fld->ty = ty;
	
	*rty->tail = fld;
	rty->tail = &(fld->next);
	
	return fld;
}

void EndRecord(RecordType rty)
{
	Field fld = rty->flds;
	Field pre = NULL;
	int offset = 0;
	int size = 0;
	while(fld != NULL){
		if(pre != NULL){
			offset = pre->offset + pre->ty->size;
		}
		size += fld->ty->size;
		fld->offset = offset;
		pre = fld;
		fld = fld->next;
	}

	rty->size = size;
}

InitData CheckStructInitializer(AstCompositeLit compositeLit)
{
		AstNode literalType = compositeLit->literalType;
		AstLiteralValue literalValue = compositeLit->literalValue;
		AstKeyedElement element = literalValue->keyedElement;	
	
		InitData head = (InitData)MALLOC(sizeof(struct initData));
		//memset(head, 0, sizeof(struct initData));
		InitData *init = &(head->next);
		int offset = 0;
		RecordType rty = CheckStructSpecifier((AstStructSpecifier)literalType);
		Field fld = rty->flds;
		if(literalValue->hasKey){
			// {age:3,height:4}这种初始化值比较难处理。
			while(fld){
				AstKeyedElement targetElement = LookupElement(element, fld->id);
				*init = (InitData)MALLOC(sizeof(struct initData));
				//memset(*init, 0, sizeof(struct initData));
				if(targetElement == NULL){
					(*init)->expr = NULL;
				}else{
					(*init)->expr = targetElement->element->expr;	
				}
				(*init)->offset = offset;
				offset += fld->ty->size;

				init = &((*init)->next);

				fld = fld->next;
			}
		}else{
			while(fld){
				// 语义检查，值和数据类型是否匹配。
				// TODO 暂时不支持嵌套结构体。
				AstNode val = element->element->expr;
				if(!CanAssign(fld->ty, val)){
					// TODO 打印报错信息，搁置。
				}
				
				*init = (InitData)MALLOC(sizeof(struct initData));
				//memset(*init, 0, sizeof(struct initData));
				(*init)->expr = val;
				(*init)->offset = offset;
				offset += fld->ty->size;

				init = &((*init)->next);

				fld = fld->next;
			}	
		}

	InitData idata = head->next;

	return idata;
}

InitData CheckCompositeLitInitializer(AstCompositeLit compositeLit)
{
	InitData idata;
	AstNode literalType = compositeLit->literalType;
	if(literalType->kind == NK_StructSpecifier){
		idata = CheckStructInitializer(compositeLit);
	}else if(literalType->kind == NK_ArrayTypeSpecifier){
		idata = CheckArrayInitializer(compositeLit);
	}else if(literalType->kind == NK_MapSpecifier){
		idata = CheckMapInitializer(compositeLit);
	}

	return idata;
}

void CheckInitializer(AstInitializer init)
{
	InitData idata;
	
	if(init->isCompositeLit == 1){
		// 处理struct{age int;height int}{3,4}这种初始化值。
		AstCompositeLit compositeLit = init->compositeLit;
		idata = CheckCompositeLitInitializer(compositeLit);
	}else{
		idata = (InitData)MALLOC(sizeof(struct initData));
		//memset(idata, 0, sizeof(struct initData));
		idata->offset = 0;
		idata->expr = init->expr;
	}

	init->idata = (AstNode)idata;
}

int CanAssign(Type ty, AstExpression val)
{

	return 1;
}

AstKeyedElement LookupElement(AstKeyedElement element, char *name)
{
	while(element){
		AstKey key = element->key;
		if(key->lbrace == 0){
			char *keyName = (char *)(((AstExpression)(key->expr))->val.p);
			if(strcmp(keyName, name) == 0){
				return element;
			}
		}
		element = element->next;
	}

	return NULL;
}

ArrayType CheckArraySpecifier(AstArrayTypeSpecifier specs)
{
	ArrayType aty = (ArrayType)MALLOC(sizeof(struct arrayType));
	// TODO 数组的长度不总是可折叠的数据。这里是简化了问题。
	aty->length = specs->expr->val.i[0];
	CheckDeclarationSpecifiers((AstSpecifiers)specs->type);
	aty->bty = ((AstSpecifiers)specs->type)->ty;	

	return aty;
}

InitData CheckArrayInitializer(AstCompositeLit compositeLit)
{
	AstArrayTypeSpecifier node  = (AstArrayTypeSpecifier)compositeLit->literalType;
	ArrayType aty = CheckArraySpecifier(node);
	AstLiteralValue literalValue = compositeLit->literalValue;
	AstKeyedElement element = literalValue->keyedElement;	

	InitData head = (InitData)MALLOC(sizeof(struct initData));
	if(head == -1){
		exit(-2);
	}

	//memset(head, 0, sizeof(struct initData));
	InitData *init = &(head->next);
	int offset = 0;

	AstExpression arrayLength = node->expr;
	int length = -1;
	if(arrayLength){
		length = arrayLength->val.i[0];
	}

	int index = 0;
	// TODO RecordType转化成普通Type后，数据似乎全部丢失。此处的写法，有问题吗？
	int elementSize = aty->bty->size;
	while(element != NULL && length >= 0){
		offset = index * elementSize;
		*init = (InitData)MALLOC(sizeof(struct initData));
		//memset(*init, 0, sizeof(struct initData));
		(*init)->offset = offset;
		// TODO 这样做行吗？exlement->expr中的数据是否会被销毁？
		// TODO element->element->expr 没有考虑嵌套。
		(*init)->expr = element->element->expr;	
		init = &((*init)->next);
		// *init = &((*init)->next);
	
		element = element->next;
		length--;
		index++;
	}

	return head->next;	
}

MapType CheckMapSpecifier(AstMapSpecifier specs)
{
	// TODO 缺少一个数据类型枚举值。
	MapType mty = (MapType)MALLOC(sizeof(struct mapType));
	CheckDeclarationSpecifiers((AstSpecifiers)specs->keyType);
	CheckDeclarationSpecifiers((AstSpecifiers)specs->elementType);
	mty->key = (Type)(((AstSpecifiers)(specs->keyType))->ty);
	mty->value = (Type)(((AstSpecifiers)(specs->elementType))->ty);
	
	return mty;
}

InitData CheckMapInitializer(AstCompositeLit compositeLit) 
{
	AstMapSpecifier node  = (AstMapSpecifier)compositeLit->literalType;

	MapType mty = CheckMapSpecifier(node);
	int keySize = mty->size;
	int valSize = mty->size;
	int kvSize = keySize + valSize;
	AstLiteralValue literalValue = compositeLit->literalValue;

	AstKeyedElement element = literalValue->keyedElement;	
	InitData head = (InitData)MALLOC(sizeof(struct initData));
	if(head == -1){
		exit(-2);
	}

	//memset(head, 0, sizeof(struct initData));
	InitData *init = &(head->next);
	int offset = 0;
	int index = 0;

	// TODO 不支持嵌套
	while(element){
		int size = sizeof(struct keyValue);
		KeyValue kv = (KeyValue)MALLOC(size);
		//memset(kv, 0, size);
		kv->key = (AstExpression)(((AstKey)element->key)->expr); 
		kv->value = (AstExpression)(((AstElement)element->element)->expr);

		*init = (InitData)MALLOC(sizeof(struct initData));
		//memset(*init, 0, sizeof(struct initData));
		(*init)->kv = kv;
		offset = index * kvSize;
		(*init)->offset = offset;
		init = &((*init)->next);

		index++;
		element = element->next;
	}	
	
	return head->next;
}
