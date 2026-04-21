package main

import (
	"fmt"
	"os"
)

func main() {

	fmt.Print(os.Environ(), "\n\n") // 获取所有的环境变量，并以字符串切片的形式返回

	fmt.Println("username :", os.Getenv("USERNAME")) // 获取指定环境变量的值

	os.Clearenv()                  // 清除所有环境变量
	os.Setenv("username", "admin") // 设置环境变量

	fmt.Println(os.Environ()) // 获取所有环境变量，验证是否设置成功
}
