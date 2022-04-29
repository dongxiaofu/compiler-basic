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

	return result, abc, def;

}
