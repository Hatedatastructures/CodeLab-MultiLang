package structtype

import "fmt"

// Person 基础结构体
type Person struct {
	Name string
	Age  int
}

// Address 地址结构体
type Address struct {
	City    string
	Country string
}

// Animal 定义一个动物接口
type Animal interface {
	Print()
	Getcategory() string
	Sleep()
}

// Getcategory 获取动物类别
func Getcategory(animal Animal) string {
	return animal.Getcategory()
}

// Print 打印动物信息
func Print(animal Animal) {
	animal.Print()
}

// Person 的 Print 方法
func (p Person) Print() {
	fmt.Println("Person Print")
	fmt.Println("Name:", p.Name, "Age:", p.Age)
}