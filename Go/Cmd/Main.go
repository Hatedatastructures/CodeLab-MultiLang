package main

import (
	"flag"
	"fmt"
	"os" // 系统相关的包，用于访问命令行参数等功能
)

type VersionInfo struct {
	Version   string
	BuildTime string
	GitCommit string
}

// 修改需要指针接收者，因为我们要修改 VersionInfo 的字段值。读取不需要，值就可以
func (cfg *VersionInfo) InIt() {
	fmt.Println("InIt")
	cfg.Version = "0.1.0-dev"
	cfg.BuildTime = "2024-06-01T12:00:00Z"
	cfg.GitCommit = "abcdef1234567890"
}

func (v VersionInfo) String() string {
	return fmt.Sprintf("Version: %s\nBuild Time: %s\nGit Commit: %s", v.Version, v.BuildTime, v.GitCommit)
}

var version = "当前版本为 0.1.0-dev"

func main() {
	fmt.Println("Cmd data", os.Args) // os.Args 包含了命令行参数，第一个元素是程序的名称
	// 命令行注册和解析
	fmt.Println("----start----")
	Path := flag.String("c", "configs/relay.yaml", "path to config file")
	VersionShow := flag.Bool("version", false, "show version info")
	flag.Parse()
	VerInfo := VersionInfo{}
	VerInfo.InIt()
	if *VersionShow {
		fmt.Println("Version info :\n" + VerInfo.String())
	}
	fmt.Println("Config file path:", *Path)
	fmt.Println("Number of flags:", flag.NFlag()) // 返回命令行参数的数量
}
