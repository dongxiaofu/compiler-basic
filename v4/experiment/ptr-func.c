#include <stdio.h>
 
int max(int x, int y)
{
    return x > y ? x : y;
}
 

int main(void)
{
    /* p 是函数指针 */
    // int (* p)(int, int) = & max; // &可以省略
//    int (*p )(int, int)  = & max; // &可以省略
    typedef int (*p)(int, int);
    // typedef (int (int, int)) *p;
    p func[2];
    func[0] = max;
    int a, b, c, d;
 
    printf("请输入三个数字:");
    scanf("%d %d %d", & a, & b, & c);
 
    /* 与直接调用函数等价，d = max(max(a, b), c) */
    // d = p(p(a, b), c); 
    d = func[0](func[0](a, b), c); 
 
    printf("最大的数字是: %d\n", d);
    printf("hello,world\n\nhow");
    // C语言不支持包含换行符的字符串。
//    printf("字符串能换行吗？ %%\n", "Hello
//	World");
 
    return 0;
}
