package main

import "unsafe"

func test(){
	var  arr[5][7]int
	arr[3][4] = 4
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
