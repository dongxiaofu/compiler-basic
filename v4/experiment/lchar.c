#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
void main()
{
	char *str = "abcd";
	//wchar_t *wstr = "abcd";
	wchar_t *wstrs = L"fsdfs";
	//strlen(wstrs);
	wprintf(L"strs's length is:%d\n",wcslen(wstrs));
	// wprintf("%ls\n", wstrs);
	wprintf(L"%s\n", wstrs);
	wprintf(L"%ls\n", wstrs);
}
