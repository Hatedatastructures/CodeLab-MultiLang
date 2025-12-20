class user_name:
    def __init__(self, name: str):
        self.name = name
        self.__age = 0

    # @property
    # def print(self) :
    #     print("用户姓名 " + self.name)

    @property
    def age(self):
        return self.__age

    @age.setter
    def age(self, value: int):
        self.__age = value
    @age.deleter
    def age(self):
        del self.__age

people = user_name("张三")
print(people.name)
print(people.age)
people.age = 20
print(people.age)

string_value = input("请输入一个字符串：")
if string_value in ["a", "b", "c"]:
    print("您输入的字符串是：" + string_value)
else:
    print("您没有输入任何字符串")

print(f"按照字符截取 = {string_value[2:4]}") # 类似Go语言字符串截取