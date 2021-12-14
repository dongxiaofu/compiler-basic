package main

import "fmt"

func main() {
   /* 定义局部变量 */
   var a int = 100
   var b int = 200
   var ret *int
   var e int = test(88, )
   // var d,f,k int;
   // d = f = k = 20;
   var d int;
   d  = 20;
   test(88, )

   //var e float64
   // golang不支持这种变量声明方式。
   // var m int, n float64

   /* 调用函数并返回最大值 */
   ret = max(a, b)

   fmt.Printf( "最大值是 : %d\n", *ret )
   fmt.Printf( "e是 : %d\n", e )
   // fmt.Printf( "d是 : %d, f = %d, k = %d\n", d, f, k )
   fmt.Printf( "d是 : %d, f = %d, k = %d\n", d )
}

/* 函数返回两个数的最大值 */
//func max(num1 int, num2 int) (ptr *int,  ) {
// func max(num1 , num2 int) (ptr *int,  ) {
// func max(num1 , num2 int) ( *int,  ) {
func max(num1 , num2 int) ( *int ) {
   /* 声明局部变量 */
   var result *int
   //var t float64 = 7

   if (num1 > num2) {
      result = &num1
   } else {
      result = &num2
   }
   return result
}

// func test(num int,) (int ){		// ok
// func test(num int,) (res2 int ){	// ok
// func test(num int,) int{		// ok
// func test(num int,) int,{		// error
func test(num int,) int{
	var res int = num
   	fmt.Printf( "res是 : %d\n", res )

	return res
}
