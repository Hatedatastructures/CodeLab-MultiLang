package main

import (
	"fmt"
	"math"
	"strconv"
	"strings"
	"time"
	"unsafe"
)

func returnValue() (string, int) {
	return "hello world", 100
}

func main() {
	// 多返回值函数演示
	var username, _ = returnValue()
	var _, age = returnValue()
	fmt.Println(username)
	fmt.Println(age)
	t := time.Date(2023, time.Month(11), 5, 10, 0, 0, 0, time.Local)
	fmt.Println(t.Month())
	fmt.Println(t)

	// iota 常量枚举
	const (
		n1 int = iota
		n2
		n3
	)
	fmt.Println(n1, n2, n3)

	// 常量声明
	const name string = "hello world"
	fmt.Println(name)

	// 变量大小
	var value int64 = 10000000000
	fmt.Printf("value = %d \n", value)
	fmt.Println(unsafe.Sizeof(value))

	// 数学运算
	a1 := int64(math.Round(float64(10000000000) / 1000000000))
	fmt.Println(a1)
	fmt.Println(unsafe.Sizeof(name))

	// 条件判断
	testString := "hello Go"
	if testString == "hello Go" {
		fmt.Println("hello world")
	} else {
		fmt.Println("hello go")
	}
	testValue := testString
	fmt.Println(testValue)

	// 字符串操作
	stringTest := "nihao"
	fmt.Println(len(stringTest))
	str1 := "你好"
	fmt.Printf("%s\n", fmt.Sprintf("%s %s", stringTest, str1))

	// 字符串切分
	str := "127157161521171251671"
	stringArr := strings.Split(str, "1")
	fmt.Print("源字符串：", str, "分割完成->")
	fmt.Print(stringArr)
	fmt.Print("\nJoin 函数操作：\n")
	valueJoin := strings.Join(stringArr, "->")
	fmt.Print("Join 函数操作结果：", valueJoin, "\n")

	// 检查字符串包含
	testStr1 := "hello world"
	testStr2 := "hello"
	if strings.Contains(testStr1, testStr2) {
		fmt.Print("找到了，里面包含", testStr2, "字符串\n")
	} else {
		fmt.Print("没有找到，里面不包含", testStr2, "字符串\n")
	}

	// 切片定义
	arr := []string{"C++", "Java", "Go", "Python", "JavaScript"}
	fmt.Println("切片打印->", arr)
	valueJoin2 := strings.Join(arr, "-")
	fmt.Println("Join 函数操作结果：", valueJoin2, "value长度大小:", len(valueJoin2))

	// 字符串转切片
	chinString := "你好"
	strRune := []rune(chinString)
	fmt.Printf("字符串转切片: %s\n", string(strRune))

	// 字符串转数字
	valueInt, _ := strconv.ParseInt("123", 10, 64)
	testValueInt := 123
	fmt.Println("数字转换为字符串：", strconv.FormatInt(int64(testValueInt), 10))
	fmt.Println("value = ", valueInt)

	// 自增运算
	number := 1234
	number++
	fmt.Println(number)

	// 循环
	var sum uint64 = 0
	j := uint64(1000000000)
	for i := uint64(0); i <= j; i++ {
		sum += i
	}
	fmt.Println(j, "连续相加结果：", sum)

	// range 遍历字符串
	nameTest := "这是个中文字符串,hello,world"
	for k, v := range nameTest {
		fmt.Printf("字符位置：%d,字符值：%c\n", k, v)
	}

	// switch
	switch stringTest {
	case "nihao":
		fmt.Println("nihao world")
		fmt.Println("nihao world")
		fallthrough
	case "hello":
		fmt.Println("hello world")
	default:
		fmt.Println("default")
	}

	// 数组
	arrTest := [...]string{"hello", "world", "nihao", "hello"}
	for k, v := range arrTest {
		fmt.Printf("数组位置：%d,数组值：%s\n", k, v)
	}

	// 多维数组
	arr2D := [...][2]string{
		{"hello", "world"},
		{"nihao", "hello"},
	}
	fmt.Println("按数组遍历")
	for k, v := range arr2D {
		fmt.Println("二维数组位置：", k, "二维数组值：", v)
	}
	fmt.Println("按值遍历")
	for k, v := range arr2D {
		for i, val := range v {
			fmt.Printf("二维数组位置：%d,%d,二维数组值：%s\n", k, i, val)
		}
	}

	// 从切片构建切片
	arrSlice := []string{"hello", "world", "nihao", "hello"}
	arrSlice2 := arrSlice[1:3]
	fmt.Println("从切片构建的切片打印->", arrSlice2)

	// 切片追加
	var arrAppend []string
	arrAppend = append(arrAppend, "nihao")
	fmt.Println("切片扩容：", arrAppend)
	arrAppend2 := []string{"hello", "world"}
	arrAppend = append(arrAppend, arrAppend2...)
	fmt.Println("切片和切片的追加：", arrAppend)

	// make 创建切片
	arrMake := make([]string, 8, 18)
	fmt.Println("切片创建和声明：", arrMake)
	arrMake[0] = "nihao"
	arrMake[7] = "world"
	fmt.Println("切片修改后：", arrMake)

	// map
	mapTest := map[string]string{"你好": "hello", "世界": "world", "nihao": "hello"}
	fmt.Println("map_test:", mapTest)
	mapTest["nihao"] = "world"
	fmt.Println("nihao:", mapTest["nihao"])
}
