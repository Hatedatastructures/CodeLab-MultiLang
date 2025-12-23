

# 生成 1 ~ 20 的平方


# num_list= []

# 传统写法

# for i in range(1,21):
#     num_list.append(i**2)
#
# print(num_list)

# 列表推导式

num_list = [i**2 for i in range(1,21)]

print(num_list)


new_list = [1 ,2 , 4, 8, 16, 32, 64, 128, 256, 512, 1024]

# even_number_list = [i for i in new_list if i % 2 == 0]

print([i**2 for i in new_list if i % 2 == 0])

