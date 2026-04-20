package main

import (
	"fmt"
	"net"
	"reflect"
)

// Person 带标签的结构体（反射演示）
type Person struct {
	Name string `info:"name" doc:"姓名"`
	Sex  string `info:"sex"`
}

// TagFind 通过反射获取结构体标签
func TagFind(value any) {
	t := reflect.TypeOf(value)
	fmt.Println("当前是函数传进来的类型：", t)
	fmt.Println("当前是函数传进来的类型的元素：", t.Elem().Field(0).Tag.Get("info"))
}

// testReflect 反射测试
func testReflect() {
	var str string = "hello world"
	var anyTest any = &str
	fmt.Println(reflect.TypeOf(str), reflect.ValueOf(str))
	fmt.Println(reflect.TypeOf(anyTest), reflect.ValueOf(anyTest.(*string)))
	fmt.Println(reflect.TypeOf(anyTest), reflect.ValueOf(anyTest).Elem())

	var test Person
	TagFind(&test)
}

// MakePrintln 处理 TCP 连接
func MakePrintln(socket net.Conn) {
	var request = make([]byte, 4096)
	stringLen, err := socket.Read(request)
	if err != nil {
		fmt.Println("读取失败", err)
		return
	}
	fmt.Println("连接成功,来自：", socket.RemoteAddr().String(), "长度：", stringLen, "消息：", string(request[:stringLen]))
}

// testTcpServer TCP 服务器测试
func testTcpServer() {
	en := net.JoinHostPort("127.0.0.1", "8888")
	fmt.Println("JoinHostPort 结果：", en)

	listeningPort, errorValue := net.Listen("tcp", "127.0.0.1:6779")
	if errorValue != nil {
		fmt.Println("监听失败", errorValue)
		return
	}
	defer listeningPort.Close()

	fmt.Println("TCP 服务器启动，监听 127.0.0.1:6779")
	for {
		socketValue, errorValue := listeningPort.Accept()
		if errorValue != nil {
			fmt.Println("接受连接失败", errorValue)
			continue
		}
		if socketValue != nil {
			MakePrintln(socketValue)
			socketValue.Close()
		} else {
			fmt.Println("监听错误")
		}
	}
}

func main() {
	// 反射测试
	fmt.Println("=== 反射测试 ===")
	testReflect()

	// TCP 服务器（会阻塞运行，注释掉反射测试下面的代码可以只运行反射）
	// 取消下面的注释可以启动 TCP 服务器
	// fmt.Println("\n=== TCP 服务器 ===")
	// testTcpServer()
}