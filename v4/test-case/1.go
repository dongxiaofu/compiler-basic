package main

import "unsafe"

type Reader interface {
  Read(p []byte) (n int)
}

type Closer interface {
  Close() int
  Say() int
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

func (v Books)Close(a int, b int, c int) (int,int) {
	
	var k int = 0
	var k2 byte = 7
	
	return k, 5
}

func (v Books)Say() int {
	var k4  int  = 7
	return k4
}

func (v Person)Close() (int,int) {
	
	var j int  = 8
	
	return j, 5
}

func test(){
	var x interface{} = 7
	var i Closer = Books{4}
	d := i.(Books)
	d := i.(Closer)
	d := i.Say()
}

func test2(){
	var y int = 4
}
