#ifndef SYMBOL_H
#define SYMBOL_H

// #include "grammer.h"
#include "ast.h"
#include "lex.h"
// #include "grammer.h"
#include "stmt.h"
#include "type2.h"
#include "expr.h"
// #include "grammer.h"

#define SYM_HASH_MASK	127
#define SEARCH_OUTER_TABLE 1

#define SYMBOL_COMMON	\
	int kind;	\
	char *name;	\
	int level;	\
	union value val;	\
	struct symbol *link;	\
	struct symbol *next;

typedef struct symbol{
	SYMBOL_COMMON
} *Symbol;

typedef struct variableSymbol{
	SYMBOL_COMMON
	InitData initData;	
} *VariableSymbol;

typedef struct bucketLinker{
	Symbol sym;
	struct bucketLinker *link;
} *BucketLinker;

typedef struct table{
	Symbol *buckets;
	int level;
	struct table *outer;
} *Table;

void InitSymbolTable();
Symbol AddSymbol(Table tbl, Symbol sym);
Symbol LookupID(char *name);
Symbol LookupSymbol(Table tbl, char *name);
char * GetSymbolKind(int kind);
Symbol DoLookupSymbol(Table tbl, char *name, int  searchOuter);
VariableSymbol AddVariable(char *name);

#endif
