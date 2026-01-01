import requests

url = "https://api.deepseek.com/v1/chat/completions"
# params 会自动拼接到 URL 后：?key1=value1&key2=value2
payload = {'key1': 'value1', 'key2': 'value2'}

# 发起请求
r = requests.post(url, params=payload)
print(r.status_code) # 200
print(r.text)        # 自动解码后的字符串 (Unicode)
print(r.content)     # 原始二进制字节 (bytes)，类似 C++ char* buffer