package main

import "unsafe"



func test44(x1 int, x2 int) (y int, b int) {
    return 8,12
}

func max() {
	var k1 int
	var a1,a2 int
	a1 = 14
	a2 = 15
	g1, g2 := test44(4,9)
	k1 = g1 + g2
	k1 = k1 + a1
}
