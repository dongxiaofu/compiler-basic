package main

import "fmt"

func main() {
   var n [10]int /* n 是一个长度为 10 的数组 */
   var i,j int

//	n[2] = 78

   /* 为数组 n 初始化元素 */        
   for i = 3; i < 10; i++ {
		j = 5
      n[i] = i + 100 /* 设置元素为 i + 100 */
   }

   /* 输出每个数组元素的值 */
}
