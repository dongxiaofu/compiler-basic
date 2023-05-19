int num = 45;

int max(int a, int b)
{
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
