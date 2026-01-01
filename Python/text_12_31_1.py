import ollama
# 调用llava模型分析图像（替换为你的本地图像路径）
response = ollama.chat(
    model='llava',
    messages=[
        {
            'role': 'user',
            'content': '分析这张图片的内容',
            'images': ['/path/to/your/image.jpg']  # 本地CV图像路径
        }
    ]
)
print("图像分析结果：", response['message']['content'])