package main

import (
	"fmt"
	"mime"
	"net/http"
	"os"
	"path/filepath"
	"runtime"
	"strings"
)

/**
 * @brief `derive_webroot` 计算静态目录 `webroot` 的绝对路径
 *
 * 通过 `runtime.Caller` 获取当前源码所在目录，再拼接 `webroot`，
 * 保证无论从哪一个工作目录启动（例如在仓库根目录或 `Go/project` 目录）都能正确定位静态文件。
 *
 * @return `string` 返回静态目录的绝对路径
 */
func derive_webroot() string {
	if _, current_file, _, ok := runtime.Caller(0); ok {
		return filepath.Join(filepath.Dir(current_file), "webroot")
	}
	// 退化到工作目录
	if cwd, err := os.Getwd(); err == nil {
		return filepath.Join(cwd, "webroot")
	}
	return "./webroot"
}

// `webroot` 静态目录绝对路径
var webroot = derive_webroot()

// CachedFile 缓存文件结构体
type CachedFile struct {
	Content     []byte
	ContentType string
}

// 全局文件缓存：路径 -> 文件内容
var fileCache = make(map[string]*CachedFile)

/**
 * @brief 加载静态资源到内存
 */
func loadStaticFiles() {
	fmt.Println("正在加载静态资源到内存...", webroot)
	err := filepath.Walk(webroot, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() {
			return nil
		}

		// 读取文件内容
		content, err := os.ReadFile(path)
		if err != nil {
			fmt.Printf("无法读取文件: %s, error: %v\n", path, err)
			return nil
		}

		// 计算相对路径作为 key，统一使用 "/" 分隔符
		relPath, err := filepath.Rel(webroot, path)
		if err != nil {
			return nil
		}
		// Windows 下 filepath.Rel 可能返回反斜杠，统一转换为正斜杠以匹配 URL
		relPath = filepath.ToSlash(relPath)
		// 确保以 "/" 开头，与 URL path 匹配
		if !strings.HasPrefix(relPath, "/") {
			relPath = "/" + relPath
		}

		// 获取 MIME 类型
		ext := filepath.Ext(path)
		contentType := mime.TypeByExtension(ext)
		if contentType == "" {
			contentType = "application/octet-stream"
		}

		fileCache[relPath] = &CachedFile{
			Content:     content,
			ContentType: contentType,
		}
		// fmt.Printf("已缓存: %s (%d bytes, %s)\n", relPath, len(content), contentType)
		return nil
	})

	if err != nil {
		fmt.Printf("加载静态资源失败: %v\n", err)
	} else {
		fmt.Printf("静态资源加载完成，共缓存 %d 个文件\n", len(fileCache))
	}
}

func ProcessFunction(w http.ResponseWriter, r *http.Request) {
	// 性能优化：高并发下严禁在热点路径打印日志
	// fmt.Println("Request received")

	reqPath := r.URL.Path
	if reqPath == "/" {
		reqPath = "/index.html"
	}

	// 直接查表，O(1) 复杂度
	if file, ok := fileCache[reqPath]; ok {
		w.Header().Set("Content-Type", file.ContentType)
		w.Header().Set("Content-Length", fmt.Sprint(len(file.Content)))
		w.Write(file.Content)
	} else {
		http.NotFound(w, r)
	}
}

func main() {
	// 初始化缓存
	loadStaticFiles()

	// 自定义 ServeMux
	mux := http.NewServeMux()
	mux.HandleFunc("/", ProcessFunction)

	// 自定义 Server，优化超时设置
	server := &http.Server{
		Addr:    ":8080",
		Handler: mux,
		// ReadTimeout:  5 * time.Second, // 读超时
		// WriteTimeout: 10 * time.Second, // 写超时
		// IdleTimeout:  120 * time.Second, // Keep-Alive 空闲超时
		MaxHeaderBytes: 1 << 20,
	}

	fmt.Println("HTTP server listening on :8080 (Optimized)")
	// ListenAndServe 默认开启 Keep-Alive
	if err := server.ListenAndServe(); err != nil {
		fmt.Fprintf(os.Stderr, "listen and serve: %v\n", err)
		return
	}
	fmt.Println("HTTP server stopped")
}
