package main

import "unsafe"



func test44(x1 int, x2 int) (y int, b int, c int) {
	var a int
	var ptr *int = 9
	var k int = 5
	var c int = 3
	a = k % c
	a = k * c
	a = k / c
    return 8,a,12
}

func max() {
	var k1 int
	g1,g2,g3 := test44(4,5)
	k1 = 4 + 5
}
