package main

import "unsafe"

type Books struct {
   bookId int
}

func test(){
	var book Books
	book.bookId = 5

	
}

type Point3D struct{
	y int
	z int
}

type Line struct{
	x int
	pos Point3D
}

line := Line{45, Point3D{y: 4, z: 12}}

type TreeNode struct {
	left, right *TreeNode
	value any
}

type TreeNode2 struct {
	left, right *TreeNode
	value any
}
