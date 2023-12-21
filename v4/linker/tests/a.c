
int a = 5;

char *str = "hello,world";
char *str2 = "hello,Jim";


int say(char *str, int a);

int main(int argc, char *argv[])
{
	say(str, a);
	say(str2, a);

	return 0;
}
