
class Print:
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return f"{self.value}"

    def print(self):
        '''
        打印类的value属性
        '''
        print(self.value)