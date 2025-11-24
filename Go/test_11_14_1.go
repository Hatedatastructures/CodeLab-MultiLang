package main

import (
	"context"
	"fmt"
	"net"
	"os"
	"os/signal"
	"syscall"
)

// // go 中的反射

// type Person struct {
// 	Name string `info:"name" doc:"姓名"`
// 	Sex  string `info:"sex"` // Tag 标签 方便解析
// }

// func TagFind(value any) {
// 	t := reflect.TypeOf(value)
// 	fmt.Println("当前是函数传进来的类型：", t) // 当前是函数传进来的类型： main.Person

// 	// fmt.Println("当前是函数传进来的类型的元素：", t.Elem()) // 当前是函数传进来的类型的元素： main.Person
// 	fmt.Println("当前是函数传进来的类型的元素：", t.Elem().Field(0).Tag.Get("info"))
// }

func main() {
	// var str string = "hello world"
	// var anytest any = &str
	// fmt.Println(reflect.TypeOf(str), reflect.ValueOf(str))
	// fmt.Println(reflect.TypeOf(anytest), reflect.ValueOf(anytest.(*string)))
	// fmt.Println(reflect.TypeOf(anytest), reflect.ValueOf(anytest).Elem())
	// // json.Marshal(anytest)
	// time.Date(2023, time.Month(11), 14, 10, 0, 0, 0, time.UTC)
	// var Test Person
	// TagFind(&Test)
	// {
	// 	// go 协程 创建和使用
	// 	i := 0
	// 	go func() {
	// 		for {
	// 			fmt.Println("go 协程", i)
	// 			i++
	// 			time.Sleep(time.Second * 1)
	// 			if i > 10 {
	// 				break
	// 			}
	// 			defer func() {
	// 				fmt.Println("go 协程结束", i)
	// 			}()
	// 		}
	// 	}()
	// 	j := 1
	// 	for {
	// 		j++
	// 		fmt.Println("主协程", i)
	// 		time.Sleep(time.Second * 1)
	// 		if i > 10 {
	// 			break
	// 		}
	// 	}
	// }
	// {
	// 	// go 协程的传递数据
	// 	value := make(chan int)
	// 	i := 0
	// 	go func() {
	// 		for {
	// 			i++
	// 			if i > 10 {
	// 				break
	// 			}
	// 			value <- i
	// 			fmt.Println("go 协程", "i:", i)
	// 		}
	// 	}()

	// 	for {
	// 		c := <-value
	// 		if c > 10 {
	// 			break
	// 		}
	// 		fmt.Println("主协程", "i:", c)
	// 		// 死锁问题
	// 	}
	// }
	{
		en := net.JoinHostPort("127.0.0.1", "8888")
		fmt.Println(en)
	}
	{
		ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
		defer stop()
		for {
			select {
			case <-ctx.Done():
				fmt.Println("收到信号:", ctx.Err()) // 注销信号 防止死循环
				return                          // case <-time.After(time.Second * 1):
			default:
				fmt.Println("等待信号")
			}
		}
	}
}
