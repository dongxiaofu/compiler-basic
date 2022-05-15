package main

import "unsafe"

type Reader interface {
  Read(p []byte) (n int)
}

type Closer interface {
  Close() int
}

type ReadWriter interface {
  Reader
  Writer
}

type Books struct {
   bookId int
}

func (v Books)Close() (int,int) {
	
	var k int = 0
	var k2 byte = 7
	
	return k, 5
}

func test(){

}

func test2(){

}
