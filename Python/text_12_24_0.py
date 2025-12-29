
# 计算每个学生的平均成绩总分，并输出平均成绩最高的学生信息
students = (
    ("S0001","John", 89,67,56),
    ("S0002","Mike", 78,56,67),
    ("S0003","Mary", 56,78,89),
    ("S0004","Tom", 67,89,56),
    ("S0005","Sam", 89,56,78),
    ("S0006","Lily", 78,91,99),
    ("S0007","Lucy", 56,67,89),
    ("S0008","Jack", 89,56,78),
    ("S0009","Jenny", 78,89,56),
    ("S0010","Tommy", 56,67,89),
)

excellent_students = []

for s in students:
    total_score = sum(s[2:])
    print(f"学号：{s[0]}\t姓名：{s[1]}\t总分：{total_score}\t平均分：{total_score/3:.1f}")
    if total_score // 3  >= 70 :
        excellent_students.append(s)

print()

chinese_scores = []
mathematics_scores = []
english_scores = []

for student in students:
    chinese_scores.append(student[2])
    mathematics_scores.append(student[3])
    english_scores.append(student[4])

print(f"语文最高分：{max(chinese_scores)} \t 平均分：{sum(chinese_scores)/len(chinese_scores):.1f}")
print(f"数学最高分：{max(mathematics_scores)} \t 平均分：{sum(mathematics_scores)/len(mathematics_scores):.1f}")
print(f"英语最高分：{max(english_scores)} \t 平均分：{sum(english_scores)/len(english_scores):.1f}")

print()

for s in excellent_students:
    print(f"优秀学生学号：{s[0]}\t姓名：{s[1]}\t总分：{sum(s[2:])}\t平均分：{sum(s[2:])/3:.1f}")
