





### 输入10个数字计算最大值最小值和平均值


num_list = []

for i in range(10):
    num_list.append(int(input("请输入一个数字：")))

num_list.sort()


print(f"平均值：{sum(num_list)/len(num_list)}, 最小值：{min(num_list)}, 最大值：{max(num_list)}")


