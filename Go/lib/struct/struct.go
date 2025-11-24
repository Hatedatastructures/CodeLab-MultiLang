package structls

import "fmt"

type Person struct {
	Name string
	Age  int
}
type Address struct {
	City    string
	Country string
}

// Animal 定义一个动物接口，包含Print、GetCategory、Sleep方法
type Animal interface {
	Print()
	Getcategory() string
	Sleep()
}

func Getcategory(animal Animal) string {
	return animal.Getcategory()
}
func Print(animal Animal) {
	animal.Print()
}

func (p Person) Print() {
	fmt.Println("Person Print")
	fmt.Println("Name:", p.Name, "Age:", p.Age)
}
