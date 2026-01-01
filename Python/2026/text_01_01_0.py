import os
from openai import OpenAI

file_path = "Python\\2026\\key.log"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
        deepseek_key = content.strip()
        # with 关键字会自动关闭文件，无需手动调用 f.close()
except FileNotFoundError:
    print("文件未找到，请检查路径。")
    exit(1)
except UnicodeDecodeError:
    print("编码错误，文件可能不是 UTF-8 格式。")
    exit(1)


client = OpenAI(
    api_key=deepseek_key,
    base_url="https://api.deepseek.com")

response = client.chat.completions.create(
    model="deepseek-chat",
    messages=[
        {"role": "system", "content": "你是一个专业的c++开发者，需要帮助用户解决实际开发问题"},
        {"role": "user", "content": "你好啊，你是谁？"},
    ],
    stream=False
)

print(response.choices[0].message.content)
