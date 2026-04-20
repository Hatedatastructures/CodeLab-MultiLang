package main

import "fmt"

// Person 基础结构体
type Person struct {
	Name string
	Age  int
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

// Man 结构体嵌入 Person
type Man struct {
	Person
	Level  int
	Gender string
}

func (thisMan *Man) Print() {
	fmt.Println("Man Print")
	fmt.Println("Name:", thisMan.Name, "Age:", thisMan.Age, "Gender:", thisMan.Gender, "Level:", thisMan.Level)
}

func (thisMan *Man) Set(name string, age int, gender string, level int) Man {
	thisMan.Name = name
	thisMan.Age = age
	thisMan.Gender = gender
	thisMan.Level = level
	return *thisMan
}

// Cat 结构体实现 Animal 接口
type Cat struct {
	NameString        string
	GetcategoryString string
	SleepString       string
}

func (c *Cat) Getcategory() string {
	return c.GetcategoryString
}

func (c *Cat) Sleep() {
	fmt.Println(c.NameString, "is sleeping")
}

func (c *Cat) Print() {
	fmt.Println("Cat Print")
	fmt.Println("Name:", c.NameString, "Getcategory:", c.GetcategoryString, "Sleep:", c.SleepString)
}

// Prints 万能类型演示
func Prints(a any) {
	value, ok := a.(string)
	if ok {
		fmt.Println(value)
	} else {
		fmt.Println("not string")
	}
}

func main() {
	// Cat 结构体测试
	var test Cat
	test.NameString = "mimi"
	test.GetcategoryString = "cat"
	test.SleepString = "sleeping"
	Print(&test)
	fmt.Println("-------------")
	fmt.Println(test.Getcategory())
	test.Sleep()

	// 接口类型可以存储实现了该接口的任何类型的变量
	var testAnimal Animal = &Cat{NameString: "mimi", GetcategoryString: "cat", SleepString: "sleeping"}
	Print(testAnimal)
	fmt.Println("-------------")
	fmt.Println(Getcategory(testAnimal))
	testAnimal.Sleep()

	// 类型断言测试
	Prints("hello")
	Prints(123)
}