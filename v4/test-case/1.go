package main

import "unsafe"

func test44() int {
	var a int
	var ptr *int = 9
	var k int = 5
	var c int = 3
	a = k % c
	a = k * c
	a = k / c
    return 8
}
