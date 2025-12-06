package main

// #cgo CXXFLAGS: -std=c++11 -I.
// #include "main.h"
import "C"

func main() {
	C.cfunc(C.CString("127.0.0.1"), 8080)
}
