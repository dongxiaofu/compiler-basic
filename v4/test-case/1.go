package main

import "unsafe"

func test44() int {
	var a int
	var ptr *int = 9
	var k int = 5
	a = ^k
    return 8
}
