package main

import (
	"fmt"
	"time"
)

func testTime() {
	timeObject := time.Now()
	timeString := timeObject.Format("2006-01-02 15:04:05")
	fmt.Println(timeObject)
	fmt.Println(timeString)
}

// 创建一个定时器任务
func testTime2() {
	start := time.Now()
	time.Sleep(time.Second * 2)
	end := time.Now()
	fmt.Println("2秒后时间:", end.Sub(start).Nanoseconds())
}

func testFunc(val int) {
	fmt.Println("test func", val)
}

func main() {
	// 时间测试
	testTime()
	testTime2()

	// Goroutine 测试
	fmt.Println("\n启动 Goroutine 测试...")
	for i := range 10 {
		go testFunc(i)
	}
	time.Sleep(time.Second * 2)
	fmt.Println("Goroutine 测试完成")
}