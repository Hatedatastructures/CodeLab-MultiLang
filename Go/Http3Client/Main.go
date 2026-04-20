package main

import (
	"crypto/tls"
	"fmt"
	"io"
	"net/http"

	"github.com/quic-go/quic-go/http3"
)

func main() {
	// 创建 HTTP/3 RoundTripper
	roundTripper := &http3.Transport{
		TLSClientConfig: &tls.Config{
			InsecureSkipVerify: true,
		},
	}
	defer roundTripper.Close()

	// 使用 RoundTripper 创建标准 http.Client
	client := &http.Client{
		Transport: roundTripper,
	}

	resp, err := client.Get("https://localhost:443/")
	if err != nil {
		fmt.Printf("请求失败: %v\n", err)
		return
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Printf("读取响应失败: %v\n", err)
		return
	}

	fmt.Printf("状态码: %d\n", resp.StatusCode)
	fmt.Printf("协议: %s\n", resp.Proto)
	fmt.Printf("响应内容: %s\n", string(body))
}
