package main;

import "fmt";

func main() {
   /* 定义局部变量 */
   var a int = 100
   var b int = 200
  a++;
  b--;
   //++b
  // ++a
  // --b
   var ret int = a
   var c int = b

   fmt.Printf( "b值是 : %d\n", ret )
   fmt.Printf( "c值是 : %d\n", c );
}

