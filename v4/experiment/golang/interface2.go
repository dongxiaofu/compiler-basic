package main

import (
    "fmt"
)

type People interface {
    ReturnName() string
}

type Role interface {
    ReturnRole() string
}

type Student struct {
    Name string
}

func (s Student) ReturnName() string {
    return s.Name
}

func (s Student) ReturnRole() string {
    return "学生"
}

func main() {
    cbs := Student{Name: "咖啡色的羊驼"}

    var a People  // 定义a为People接口类型
    var b Role    // 定义b为Role接口类型
    var c Role    // 定义b为Role接口类型
	c = new(Student)

    a = cbs // 由于Student实现了People所有方法，所以接口实现成功，可直接赋值
    b = cbs // 由于Student实现了Role所有方法，所以接口实现成功，可直接赋值

    name := a.ReturnName()
    fmt.Println(name) // 输出"咖啡色的羊驼"

    role := b.ReturnRole()
    fmt.Println(role) // 输出"学生"

    role2 := c.ReturnRole()
    fmt.Println(role2) // 输出"学生"
}
