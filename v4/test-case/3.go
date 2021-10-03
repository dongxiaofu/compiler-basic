package main

import "unsafe"
const (
    a = "abc"
    b = len(a)
    c = unsafe.Sizeof(a)
)

const (
	Sunday = iota
	Monday
	Tuesday
	Wednesday
	Thursday
	Friday
	Partyday;
	numberOfDays;  // this constant is not exported
)


func main(){
    println(a, b,c)
    println(Sunday, Monday,Tuesday)
}
