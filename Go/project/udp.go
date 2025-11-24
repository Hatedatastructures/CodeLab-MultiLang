package main

import (
	"context"
	"fmt"
	"net"
	"os"
	"os/signal"
	"sync"
	"syscall"
)

const listenAddr = "0.0.0.0:8888"

func main() {
	// 1. 监听 UDP 地址
	pc, err := net.ListenPacket("udp", listenAddr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "listen udp %s: %v\n", listenAddr, err)
		return
	}
	defer pc.Close()
	fmt.Println("UDP server listening on", listenAddr)

	// 2. 信号捕获，用于优雅退出
	ctx, stop := signal.NotifyContext(context.Background(), os.Interrupt, syscall.SIGTERM)
	defer stop()

	var wg sync.WaitGroup

	// 3. 主循环读取包
	buf := make([]byte, 2048)
	for {
		select {
		case <-ctx.Done():
			fmt.Println("\nshutting down...")
			wg.Wait()
			return
		default:
		}

		n, addr, err := pc.ReadFrom(buf)
		if err != nil {
			if netErr, ok := err.(net.Error); ok && netErr.Temporary() {
				continue // 临时错误，继续
			}
			fmt.Fprintf(os.Stderr, "read udp: %v\n", err)
			continue
		}

		// 4. 每个包一个 goroutine 处理
		wg.Add(1)
		go func(data []byte, from net.Addr) {
			defer wg.Done()
			handle(pc, data, from)
		}(buf[:n], addr)
	}
}

// handle 负责业务逻辑：这里简单回显
func handle(pc net.PacketConn, data []byte, from net.Addr) {
	// 你可以在这里替换成任意业务
	if _, err := pc.WriteTo(data, from); err != nil {
		fmt.Fprintf(os.Stderr, "write to %s: %v\n", from, err)
	}
}
