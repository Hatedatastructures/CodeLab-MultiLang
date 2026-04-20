package main

import (
	"fmt"
	"strconv"
)

// 标准的函数，可以返回两个值
func add(x int, y int) (int, int) {
	fmt.Println("X:", x, "Y:", y)
	return x, y
}

// 可变参数接受相同类型的数组传入，参数本质是一个切片
func addTest(x ...int) int {
	fmt.Printf("可变参数列表传入的参数数据：%v,及其函数参数包类型: %T\n", x, x)
	sub := 0
	for _, value := range x {
		sub += value
	}
	return sub
}

// 返回string类型的函数
func stringFunc() string {
	return "hello,world"
}

// 基于基本数据类型定义自定义类型
type myInt int

// 基于函数来定义类型
type functions func(int, int) string

func testFunctionsAdd(first int, second int) string {
	var number = first + second
	return strconv.FormatInt(int64(number), 10)
}

// 定义一个函数来返回函数类型，或者接受函数类型
func functionsTest(str string) functions {
	fmt.Println("调用functions_test函数,输入的参数数据是：", str)
	return testFunctionsAdd
}

func testAdd() int {
	defer func() {
		fmt.Println("调用defer函数")
	}()
	return 0
}

// 函数演示
func main() {
	a, b := add(10, 88)
	fmt.Println("输入值分别是", a, b)
	c := addTest(12, 23, 4424, 454353, 6465, 5646, 24, 24, 244, 45, 56, 54)
	fmt.Println("在调用可变参数列表的函数得到的累加结果：", c)
	fmt.Println("调用string返回值的函数结果:", stringFunc())
	var myIntVar myInt = 10
	fmt.Printf("自定义数据类型：%v类型为%T\n", myIntVar, myIntVar)
	funcValue := functionsTest("泥嚎啊")
	fmt.Println("返回函数的函数执行的结果：", funcValue(123, 234))
	fmt.Println("调用test_add函数的结果:", testAdd())
}
