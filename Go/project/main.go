package main

import (
	"fmt"
	"net/http"
	"os"
	"path"
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

func ProcessFunction(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Request received")

	clean_path := path.Clean(strings.TrimSpace(r.URL.Path))
	if clean_path == "" { // 空路径兜底为根路径
		clean_path = "/"
	}

	if clean_path == "/" {
		fmt.Println("Request for index.html")
		target := filepath.Join(webroot, "index.html")
		http.ServeFile(w, r, target)
		return
	}

	rel := strings.TrimPrefix(clean_path, "/") // 去掉指定字符
	target := filepath.Join(webroot, rel)      // 拼接成绝对路径

	webroot_abs, err1 := filepath.Abs(webroot)
	target_abs, err2 := filepath.Abs(target)
	if err1 == nil && err2 == nil {
		webroot_abs = filepath.Clean(webroot_abs)
		target_abs = filepath.Clean(target_abs)
		// 允许访问 webroot 本身或其子路径，其它情况返回 403
		sep := string(os.PathSeparator)
		// HasPrefix 检查 target_abs 是否以 webroot_abs 开头 加人sep检查防止请求绕过webroot目录
		if target_abs != webroot_abs && !strings.HasPrefix(target_abs, webroot_abs+sep) {
			fmt.Println("阻止路径穿越:", target_abs)
			http.Error(w, "Forbidden", http.StatusForbidden)
			return
		}
	}
	fmt.Println("Request for file", clean_path)
	http.ServeFile(w, r, target)
}

func main() {
	http.HandleFunc("/", ProcessFunction)
	fmt.Println("HTTP server listening on :8080")
	if err := http.ListenAndServe(":8080", nil); err != nil {
		fmt.Fprintf(os.Stderr, "listen and serve: %v\n", err)
		return
	}
	fmt.Println("HTTP server stopped")
}
