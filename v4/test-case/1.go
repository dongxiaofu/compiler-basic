package main

import "unsafe"

func max2() int {
   var result int
	var num3 int = 4
	var num4 int = 5

   if (num4 > num3) {
      result = num3
   } else {
      result = num4
   }
   return result
}

func test44() int {
    var a int
    var b int
    var g int
    var y int
    var z int
	var ptr *int
    var k int  = 5
    var max int = 7
	max2()
    g = 13 + 28
	a = 5
	ptr = &k
	a = *ptr
	max2()

    if k > max {
        k = max
		return k
    }

    if y == 8 {
		z = a
        return y
    }else{
        y = 9
    }

	return k
    return 8
}
