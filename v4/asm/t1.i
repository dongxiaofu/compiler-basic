# 1 "t1.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "t1.c"
int num = 45;
char *string0 = "How are you?\n";
char *string1 = "Fine, thank you.\n";
char *string2 = "Fine\n";
char *string3 = "How are you?Fine, thank you.\n";

int max(int a, int b)
{
 char *str11 = "Hello,World\n";
 char *str12 = str11;
 int num1 = 100;
 int num2 = 200;
 num = num1 + num2;
 return num;
}

int main(int argc, char *argv[])
{
 int num1 = num;
 int num2 = 5;

 int num3 = max(num1, num2);
 int num4 = num;
 int num5 = num3 - num4;
 num5 = num5 + 2;

 return num5;
}
