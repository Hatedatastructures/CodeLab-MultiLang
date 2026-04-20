package lib

import "fmt"

// Lib 示例库函数
func Lib() {
	fmt.Println("lib.Lib() called")
}

func init() {
	fmt.Println("lib package initialized")
}