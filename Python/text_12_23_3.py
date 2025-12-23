



# 合并两个列表并去重

list1 = [23,121,342343,535,535,45,3435,3,345,34]
list2 = [23,11,343,535,35,45,345,3,35,34,34]

# for i in list1:
#     list2.append(i)
# 手动添加


# 解包
# num_list = [*list1 ,*list2]

# 组包

num_list = list1 + list2


print(f"合并成一个链表：{num_list}")

new_list = []

for j in num_list:
    if j not in new_list:
        new_list.append(j)

print(f"去重完成：{new_list}")