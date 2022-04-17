#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"

void CheckTranslationUnit(AstTranslationUnit transUnit)
{
	printf("%s\n", "Start Check");

	AstDeclaration p = transUnit->decls;

	while(p){
		if(p->kind == NK_Function){
			printf("%s\n", "Check function");
		}else{
			printf("%s\n", "Check global declaration");
			CheckGlobalDeclaration(p);
		}
		p = p->next;
	}

	printf("%s\n", "End Check");
}

// TODO 必须加static吗？
// static void CheckGlobalDeclaration(AstDeclaration decl)
void CheckGlobalDeclaration(AstDeclaration decls)
{
	if(decls->kind == NK_VarDeclaration){
		AstVarDeclaration decl = (AstVarDeclaration)decls;
		int i = 0;
		while(decl){
			AstVarDeclarator declarator = (AstVarDeclarator)decl->decs;	
			while(declarator){
				// 处理说明符
				CheckDeclarationSpecifiers(declarator->specs);
				Symbol sym;
				// TODO var a,b int = 2,4
				AstInitDeclarator initDec = (AstInitDeclarator)declarator->initDecs;
				// CG 处理a,b
				while(initDec){
					AstDeclarator dec = initDec->dec;
					if((sym = LookupID(dec->id)) == NULL){
						sym = (Symbol)AddVariable(dec->id);
					}	
					// 变量的初始值
					if(initDec->init){
						CheckInitializer((AstInitializer)initDec->init);
					}

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
	RecordType rty = (RecordType)malloc(sizeof(struct recordType));
	rty->kind = NK_RecordType;
	rty->id = NULL;
	rty->flds = NULL;
	rty->tail = &(rty->flds);

	return rty;
}

Field AddField(RecordType rty, char *id, Type ty)
{
	int fldSize = sizeof(struct field);
	Field fld = (Field)malloc(fldSize);
	memset(fld, 0, fldSize);

	int idSize = sizeof(char) * MAX_NAME_LEN;
	fld->id = (char *)malloc(idSize);
	memset(fld->id, 0, idSize);
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
//		if(fld->kind == NK_RecordType){
//			EndRecord(fld);
//		}
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

InitData CheckCompositeLitInitializer(AstCompositeLit compositeLit)
{
	AstNode literalType = compositeLit->literalType;
	AstLiteralValue literalValue = compositeLit->literalValue;
	AstKeyedElement element = literalValue->keyedElement;	

	InitData head = (InitData)malloc(sizeof(struct initData));
	memset(head, 0, sizeof(struct initData));
	InitData *init = &(head->next);
	int offset = 0;
	

//	if(literalType->kind == NK_StructDeclaration){
	if(literalType->kind == NK_StructSpecifier){
		RecordType rty = CheckStructSpecifier((AstStructSpecifier)literalType);
		Field fld = rty->flds;
		if(literalValue->hasKey){
			// {age:3,height:4}这种初始化值比较难处理。
			while(fld){
				AstKeyedElement targetElement = LookupElement(element, fld->id);
				*init = (InitData)malloc(sizeof(struct initData));
				memset(*init, 0, sizeof(struct initData));
				(*init)->expr = targetElement->element->expr;	
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
				
				*init = (InitData)malloc(sizeof(struct initData));
				memset(*init, 0, sizeof(struct initData));
				(*init)->expr = val;
				(*init)->offset = offset;
				offset += fld->ty->size;

				init = &((*init)->next);

				fld = fld->next;
			}	
		}
	}

	return head->next;
}

void CheckInitializer(AstInitializer init)
{
	InitData idata;
	
	if(init->isCompositeLit == 1){
		// 处理struct{age int;height int}{3,4}这种初始化值。
		AstCompositeLit compositeLit = init->compositeLit;
		idata = CheckCompositeLitInitializer(compositeLit);
	}else{
		idata = (InitData)malloc(sizeof(struct initData));
		memset(idata, 0, sizeof(struct initData));
		idata->offset = 0;
		idata->expr = init->expr;
	}

	init->idata = (AstNode)idata;
}

int CanAssign(Type ty, AstNode val)
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
