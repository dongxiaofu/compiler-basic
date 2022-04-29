#include "ast.h"
#include "stmt.h"
#include "decl.h"
#include "expr.h"
#include "declchk.h"
#include "symbol.h"


static struct table GlobalIDs;
static struct table Constants;
static Table Identifiers;

void InitSymbolTable()
{
	GlobalIDs.buckets = NULL;
	GlobalIDs.outer = NULL;
	GlobalIDs.level = 0;

	// 常量表比较复杂，没弄懂，搁置。

	Identifiers = &GlobalIDs;
}

Symbol AddSymbol(Table tbl, Symbol sym)
{
	// int h = (unsigned long)sym->name & SYM_HASH_MASK;
	unsigned int h = (unsigned long)sym->name & SYM_HASH_MASK;
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
	//memset(linker, 0, linkerSize);
	linker->sym = sym;

	linker->link = (BucketLinker)tbl->buckets[h];
	tbl->buckets[h] = (Symbol)linker;

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

Symbol LookupID(char *name)
{
	return LookupSymbol(Identifiers, name);
}

Symbol LookupSymbol(Table tbl, char *name)
{
	return DoLookupSymbol(tbl, name, SEARCH_OUTER_TABLE); 
}

char * GetSymbolKind(int kind)
{

}

Symbol DoLookupSymbol(Table tbl, char *name, int  searchOuter)
{
	do{
		// int h = (unsigned long)name & SYM_HASH_MASK;
		unsigned int h = (unsigned long)name & SYM_HASH_MASK;
		if(tbl->buckets != NULL){
			BucketLinker linker;
			for(linker = (BucketLinker)tbl->buckets[h]; linker; linker = linker->link){
				if(name == linker->sym->name){
					return (Symbol)linker->sym;
				}
			}	
		}
	}while(((tbl = tbl->outer) != NULL) && searchOuter);

	return NULL;
}

VariableSymbol AddVariable(char *name)
{
	// int size = sizeof(VariableSymbol);
	int size = sizeof(struct variableSymbol);
	VariableSymbol p = (VariableSymbol)MALLOC(size);	
	// //memset(p, 0, sizeof(struct variableSymbol));
	//memset(p, 0, size);
	// TODO 只存储name的内存地址还是把name指向的数据复制过来？
//	p->name = (char *)MALLOC(sizeof(char) * MAX_NAME_LEN);	
//	//memset(p->name, 0, MAX_NAME_LEN);
	// strcpy(p->name, name, MAX_NAME_LEN);
//	strcpy(p->name, name);
	p->name = name;

	p = (VariableSymbol)AddSymbol(Identifiers, (Symbol)p);

	return p;
}

FunctionSymbol AddFunction(char *funcName, Signature sig)
{
	FunctionType fty = (FunctionType)MALLOC(sizeof(struct functionType));
	//memset(fty, 0, sizeof(struct functionType));
	fty->sig = sig;
	FunctionSymbol fsym = (FunctionSymbol)MALLOC(sizeof(struct functionSymbol));
	//memset(fsym, 0, sizeof(struct functionSymbol));
	fsym->name = funcName;
	fsym->ty = fty;
	fsym = (FunctionSymbol)AddSymbol(Identifiers, (Symbol)fsym);

	return fsym;
}
