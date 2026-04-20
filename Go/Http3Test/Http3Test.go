package main

import (
	"log"
	"net/http"
)

func main() {
	mux := http.NewServeMux()
	mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("Hello from HTTP/3!"))
	})

	// 使用标准 http.Server，Go 1.21+ 会自动处理 HTTP/3 的协商和启用
	srv := &http.Server{
		Addr:    ":443",
		Handler: mux,
	}

	log.Println("Starting HTTP/3 server on :443...")
	// 使用 ListenAndServeTLS 启动即可，前提是证书支持 ALPN h3 协议
	log.Fatal(srv.ListenAndServeTLS("cert.pem", "key.pem"))
}
