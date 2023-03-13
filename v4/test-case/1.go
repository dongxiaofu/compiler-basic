package main

import "fmt"

/* 函数返回两个数的最大值 */
func max(num1, num2 int) int {
   /* 定义局部变量 */
   var result int

   if (num1 > num2) {
      result = num1
   } else {
      result = num2
   }
   return result
}

func main() {
   /* 定义局部变量 */
   var a int = 100
   var b int = 200
   var ret int

   /* 调用函数并返回最大值 */
//   ret = max(3, b)
   ret = a + max(3, b)
   ret = a + max(5, b) + b + (a+b)
  // ret = a + b +  a

  // fmt.Printf( "最大值是 : %d\n", ret )
}

// /* 函数返回两个数的最大值 */
// func max(num1, num2 int) int {
//    /* 定义局部变量 */
//    var result int
// 
//    if (num1 > num2) {
//       result = num1
//    } else {
//       result = num2
//    }
//    return result
// }
