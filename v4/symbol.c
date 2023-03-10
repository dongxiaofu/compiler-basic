#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"


static struct table GlobalIDs;
static struct table GlobalInterfaces;
static struct table Constants;
static Table Identifiers;
static Table InterfaceIdentifiers;

void InitSymbolTable()
{
	GlobalInterfaces.buckets = GlobalIDs.buckets = NULL;
	GlobalInterfaces.outer = GlobalIDs.outer = NULL;
	GlobalInterfaces.level = GlobalIDs.level = 0;

	
	// 常量表比较复杂，没弄懂，搁置。

	Identifiers = &GlobalIDs;
	InterfaceIdentifiers = &GlobalInterfaces;

	Strings = (Symbol)MALLOC(sizeof(struct symbol));
	StringTail = Strings;

//	INTERFACE_LIST = NULL;
//	FUNCTION_LIST = NULL;
}

Symbol AddSymbol(Table tbl, Symbol sym, unsigned int hashKey)
{
	if(tbl->buckets == NULL){
		// int size = sizeof(struct symbol) * (SYM_HASH_MASK + 1);
		// int size = sizeof(struct symbol) * (SYM_HASH_MASK + 1);
		int size = sizeof(Symbol) * (SYM_HASH_MASK + 1);
	// TODO 一个十分可笑的错误。
	//	tbl->buckets = (Symbol *)MALLOC(sizeof(size));
		tbl->buckets = (Symbol *)MALLOC(size);
		//memset(tbl->buckets, 0, size);
	}
	
	int linkerSize = sizeof(BucketLinker);
	BucketLinker linker = (BucketLinker)MALLOC(linkerSize);
	linker->sym = sym;

	linker->link = (BucketLinker)tbl->buckets[hashKey];
	tbl->buckets[hashKey] = (Symbol)linker;

	sym->level = tbl->level;

	return sym;
}
// TODO AddSymbol2中包含困扰我很长时间的错误写法。AddSymbol是修复了所有所谓。
// Symbol AddSymbol2(Table tbl, Symbol sym)
// {
// 	int h = sym->name & SYM_HASH_MASK;
// 	if(tbl->buckets == NULL){
// 		// int size = SYM_HASH_MASK + 1;
// 		int size = sizeof(struct symbol) * (SYM_HASH_MASK + 1);
// 		// tbl->buckets = (Symbol *)MALLOC(sizeof(Symbol) * size);
// 		// tbl->buckets = (Symbol)MALLOC(sizeof(Symbol) * size);
// 		// tbl->buckets = (Symbol)MALLOC(sizeof(struct symbol) * size);
// 		tbl->buckets = (Symbol *)MALLOC(sizeof(struct symbol) * size);
// 		//memset(tbl->buckets, 0, size);
// 	}
// 	
// 	// int linkerSize = sizeof(BucketLinker);
// 	int linkerSize = sizeof(struct bucketLinker);
// 	// BucketLinker linker = (BucketLinker)MALLOC(sizeof(BucketLinker));
// 	// BucketLinker linker = (BucketLinker)MALLOC(sizeof(struct bucketLinker));
// 	BucketLinker linker = (BucketLinker)MALLOC(linkerSize);
// 	//memset(linker, 0, linkerSize);
// 	linker->symbol = sym;
// 	if(tbl->buckets[h] == NULL){
// 		tbl->buckets[h] = (Symbol)linker;
// 	}
// 	linker->linker = tbl->buckets[h];
// 	tbl->buckets[h] = linker;
// 
// 	sym->level = tbl->level;
// 
// 	return sym;
// }
Symbol LookupMethodID(char *name, char *receiverTypeAlias)
{
	return LookupSymbol(InterfaceIdentifiers, name, receiverTypeAlias);
}

Symbol LookupID(char *name)
{
	return LookupSymbol(Identifiers, name, NULL);
}

// TODO 这个函数，一定要修改。因为，在其他地方，必须直接调用LookupMethodID。
Symbol LookupFunction(AstFunctionDeclarator fdec)
{
	Symbol sym = NULL;
	char *id = fdec->dec->id;
	if(fdec->receiver){
		// TODO 这是从CheckFunction抄来的代码。一想到要经过这么多步骤，我就觉得烦。
		// 这是当初的设计造成的恶果。先这样吧。
		AstParameterTypeList receiverTyList = fdec->receiver;
		AstParameterDeclaration receiverDecls = receiverTyList->paramDecls;
		AstParameterDeclaration receiverDecl = receiverDecls;
		char *receiverTypeAlias = NULL;
		if(receiverDecl != NULL){
			AstDeclarator dec = receiverDecl->dec;
			Type ty = (AstSpecifiers)(receiverDecl->specs->ty);
			receiverTypeAlias = ty->alias;
		}
		sym = LookupMethodID(id, receiverTypeAlias);
	}else{
		sym = LookupID(id);
	}

	return sym;
}

Symbol LookupSymbol(Table tbl, char *name, char *receiverTypeAlias)
{
	unsigned int hashKey = 0;
	if(receiverTypeAlias){
		// TODO 找时间优化这里的代码。 	
		unsigned int h1 = SymbolHash(name);
		unsigned int h2 = SymbolHash(receiverTypeAlias);
		hashKey = (h1 + h2) & SYM_HASH_MASK;
	}else{
		hashKey = SymbolHash(name);
	}
	return DoLookupSymbol(tbl, name, hashKey, SEARCH_OUTER_TABLE); 
}


char * GetSymbolKind(int kind)
{

}

unsigned int SymbolHash(char *name)
{
	// int h = (unsigned long)name & SYM_HASH_MASK;
//	unsigned int h = (unsigned long)name & SYM_HASH_MASK;
	unsigned int h = 0;
	char *p = name;
	while(*p){
		h += *p;
		p++;
	}
	h = h  & SYM_HASH_MASK;

	return h;
}

Symbol DoLookupSymbol(Table tbl, char *name, unsigned int hashKey, int  searchOuter)
{
	do{
		if(tbl->buckets != NULL){
			BucketLinker linker;
			for(linker = (BucketLinker)tbl->buckets[hashKey]; linker; linker = linker->link){
				// if(name == linker->sym->name){
				if(strcmp(name,linker->sym->name) == 0){
					return (Symbol)linker->sym;
				}
			}	
		}
	}while(((tbl = tbl->outer) != NULL) && searchOuter);

	return NULL;
}

VariableSymbol AddVariable(char *name, Type ty)
{
	int size = sizeof(struct variableSymbol);
	VariableSymbol p = (VariableSymbol)MALLOC(size);	
	p->ty = ty;
	int sk = SK_Variable;
	if(ty->categ == STRUCT){
		sk = SK_Struct;
	}
	if(sk != -1){
		p->kind = sk;
	}
	// TODO 只存储name的内存地址还是把name指向的数据复制过来？
//	p->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);	
//	//memset(p->name, 0, MAX_NAME_LEN);
	// strcpy(p->name, name, MAX_NAME_LEN);
//	strcpy(p->name, name);
	p->name = name;

	unsigned int hashKey = SymbolHash(name);
	p = (VariableSymbol)AddSymbol(Identifiers, (Symbol)p, hashKey);

	if(FUNCTION_CURRENT != NULL){
		*(FUNCTION_CURRENT->lastv) = (Symbol)p;
		FUNCTION_CURRENT->lastv = &(p->next);
	}

	return p;
}

FunctionSymbol AddFunction(char *funcName, Signature sig)
{
	FunctionType fty = (FunctionType)MALLOC(sizeof(struct functionType));
	fty->sig = sig;

	// 处理参数
	VariableSymbol param, firstParam, preParam;
	param = firstParam = preParam = NULL;
	// SignatureElement paramPtr = sig->params;
	SignatureElement *paramPtr = sig->params;
	// int paramSize = sig->paramSize; 
	int paramSize = sig->newParamSize; 
	for(int i = 0; i < paramSize; i++){
		param = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
		param->name = (*paramPtr)->id;
		param->ty = (*paramPtr)->ty;
		if(firstParam == NULL){
			firstParam = param;
		}
	
		if(preParam != NULL){
			preParam->next = param;
		}else{
			preParam = param;
		}
		preParam = param;

		paramPtr++;
	}

	FunctionSymbol fsym = (FunctionSymbol)MALLOC(sizeof(struct functionSymbol));
	fsym->name = funcName;
	fsym->ty = fty;
	fsym->kind = SK_Function;
	fsym->params = firstParam;

	// 计算tbl和哈希键
	unsigned int h = 0;
	Table tbl = NULL;
	if(sig->receiver){
		// TODO 找时间优化这里的代码。 	
		unsigned int h1 = SymbolHash(funcName);
		char *typeAlias = sig->receiver->ty->alias;
		unsigned int h2 = SymbolHash(typeAlias);
		h = (h1 + h2) & SYM_HASH_MASK;

		tbl = InterfaceIdentifiers; 
		fty->categ = METHOD;
	}else{
		h = SymbolHash(funcName);
		tbl = Identifiers;
		fty->categ = FUNCTION;
	}
	fsym = (FunctionSymbol)AddSymbol(tbl, (Symbol)fsym, h);

	return fsym;
}

Symbol IntConstant(int v)
{
	Symbol sym = (Symbol)MALLOC(sizeof(struct symbol));
	union value val;
	val.i[0] = v;
	val.i[1] = 0;

	sym->kind = SK_CONSTANT;
	sym->ty = T(INT);
	sym->val = val;
	sym->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);
	sprintf(sym->name, "%d", v);

	return sym;
}

Symbol CreateTemp(Type ty)
{
	VariableSymbol sym = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
	sym->kind = SK_Temp;
	sym->ty = ty;
	// sym->name = sprintf("t%d", tmpNameNo++);
	sym->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);
	sprintf(sym->name, "t%d", tmpNameNo++);

	*(FUNCTION_CURRENT->lastv) = sym;
	FUNCTION_CURRENT->lastv = &(sym->next);

	return (Symbol)sym;
}

void DefineTemp(Symbol temp, Type ty, int opcode, Symbol src1, Symbol src2)
{
	ValueDef valueDef = (ValueDef)MALLOC(sizeof(struct valueDef));

	valueDef->dst = temp;
	valueDef->opcode = opcode;
	valueDef->src1 = src1;
	valueDef->src2 = src2;

	AsVar(temp)->def = valueDef;
}

Symbol CreateParam(Type ty)
{
	VariableSymbol sym = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
	sym->kind = SK_Variable;
	sym->ty = ty;
	// sym->name = sprintf("t%d", tmpNameNo++);
	sym->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);
	sprintf(sym->name, "p%d", receiverNameNo++);

	*(FUNCTION_CURRENT->lastv) = sym;
	FUNCTION_CURRENT->lastv = &(sym->next);

	return (Symbol)sym;
}

Symbol AddString(Type ty, String value)
{
	VariableSymbol sym = (VariableSymbol)MALLOC(sizeof(struct variableSymbol));
	sym->kind = SK_String;
	sym->ty = ty;
	sym->val.p = (void *)value;

	sym->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);
	sprintf(sym->name, "str%d", strNo++);

	StringTail->next = sym;	
	StringTail = sym;

	return (Symbol)sym;
}
