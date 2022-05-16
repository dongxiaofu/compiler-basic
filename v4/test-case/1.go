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

type Person struct{
	age int
}

func (v Books)Close() (int,int) {
	
	var k int = 0
	var k2 byte = 7
	
	return k, 5
}

func (v Person)Close() (int,int) {
	
	var j int  = 8
	
	return j, 5
}

func test(){
	var i Closer = Books{4}
	i.Close()
}

func test2(){
}
