package main

import (
	"fmt"
	"os"
)

func main() {
	fileptr, errorcode := os.Create("tempfile")
	if errorcode != nil {
		fmt.Println("创建文件失败")
		panic(errorcode)
	}

	defer fileptr.Close()

	writebyte, errorcode := fileptr.Write([]byte("你好"))
	if errorcode != nil {
		fmt.Println("写入文件失败")
		panic(errorcode)
	}
	fmt.Println("写入字节数:", writebyte)

	// 先关闭文件，再删除
	fileptr.Close()

	errorcode = os.Remove("tempfile")
	if errorcode != nil {
		fmt.Println("删除文件失败")
		panic(errorcode)
	}
}
