int test(){
  	int arr[4][2];
  	int arr2[3];
  	int *ptr;
  	int b;
  
  	// 间接使用取地址运算。
  	ptr = arr[0];
	ptr = &arr[0][1];
  	ptr = arr[1];
  	ptr = arr[2];
  	ptr = arr[3];
	ptr = &arr[3][0];
	ptr = &arr[3][1];
  	ptr = arr2;
  	b = arr2[1];
  
  	return 0;
}

int main(int argc, char **argv){
	
	test();

	return 0;
}
