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
				break;
				if(i == 4){
					continue;
				}
        }
        fmt.Println(sum)

   return result
}

func main2() {
   var c1, c2, c3 chan int
   var i1, i2 int
   select {
      case i1 = <-c1:
         fmt.Printf("received ", i1, " from c1\n")
      case c2 <- i2:
         fmt.Printf("sent ", i2, " to c2\n")
	case a[f()] = <-c4:
      case i3, ok := (<-c3): 
         if ok {
            fmt.Printf("received ", i3, " from c3\n")
         } else {
            fmt.Printf("c3 is closed\n")
			i1++
			c2--
         }
      default:
         fmt.Printf("no communication\n")
   }    

	defer  max(3,4);
	ch <- 3 

	a[i] <<= 2
i &^= 1<<n

	x = 1
*p = f()
a[i] = 23
}

func main(){
	var f int = struct{age int; x   int}{3,4}
    println(a, b, c)
}
