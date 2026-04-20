# 九九
for i in range(1, 10):
    for j in range(1, i+1):
        print(f"{j}*{i}={j*i}", end=" ")
    print()

# 水仙花
# for i in range(100, 1000):
#     hundreds = i // 100
#     tens = (i // 10) % 10
#     units = i % 10
#     if i == hundreds ** 3 + tens ** 3 + units ** 3:
#         print(i)

# 姓名函数
# def stu(mingzi, xingbie, age, aihao):
#     print(f"姓名：{mingzi}")
#     print(f"性别：{xingbie}")
#     print(f"年龄：{age}")
#     print(f"爱好：{aihao}")

# stu("张三", "男", 19, "Python程序设计")