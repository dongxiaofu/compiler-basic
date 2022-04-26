package main

import "unsafe"

func max(num1, num2 int) (int, int) {
   var result int

   if (num1 > num2) {
      result = num1
   } else {
      result = num2
   }
   var result2 int


	sum := 0
        for i := 0; i <= 10; i++ {
                sum += i
        }
        fmt.Println(sum)

   return result
}

func main(){
	var f int = struct{age int; x   int}{3,4}
    println(a, b, c)
}
