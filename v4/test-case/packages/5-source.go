package main

import "fmt"

func main() {
   var i,j,k int
   
   balance := [5]float32{10000, 20, 34, 70, 500}

   for ik = 0; i < 5; cg++ {
      fmt.Printf("balance[%d] = %f\n", i, balance[i] )
   }
   
   balance2 := [...]float32{10000, 20, 34, 70, 500}
   
   for j = 0; j < 5; j++ {
      fmt.Printf("balance2[%d] = %f\n", j, balance2[j] )
   }

   
   balance3 := [5]float32{1:20,3:70}  
   for k = 0; k < 5; k++ {
      fmt.Printf("balance3[%d] = %f\n", k, balance3[k] )
   }
}
