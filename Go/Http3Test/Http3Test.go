package main

import (
	"log"
	"net/http"

	"github.com/quic-go/quic-go/http3"
)

func main() {
	mux := http.NewServeMux()
	mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "text/plain")
		w.Write([]byte("Hello from HTTP/3!"))
		log.Printf("请求: %s %s (协议: %s)", r.Method, r.URL.Path, r.Proto)
	})

	// 使用 http3.Server 启用 HTTP/3
	server := &http3.Server{
		Addr:    ":443",
		Handler: mux,
	}

	log.Println("Starting HTTP/3 server on :443...")
	log.Fatal(server.ListenAndServeTLS("cert.pem", "key.pem"))
}
