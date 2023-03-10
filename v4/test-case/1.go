package main

import "fmt"

func main() {
   // var n [8][10]int /* n 是一个长度为 10 的数组 */
   var n [10]int /* n 是一个长度为 10 的数组 */
//   var i,j,k int

	n[2] = 78
	n[1] = 45
	n[0] = 45

	n[0] = n[2] + n[1];

//   /* 为数组 n 初始化元素 */        
//   for i = 3; i < 10; i++ {
//		j = 5
//		k = j + i
//		j = k - 8
////      n[i] = i + 100 /* 设置元素为 i + 100 */
//   }

   /* 输出每个数组元素的值 */
}
