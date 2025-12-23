import random

# 创建一个随机数
random_number = random.randint(1, 100)


# 开始猜数字

while True:
    guess = int(input("请输入一个数字："))
    if guess == random_number:
        print("恭喜你猜对了！")
        break
    elif guess < random_number:
        print("猜小了！")
    else:
        print("猜大了！")
print(f"数字是：{random_number}")