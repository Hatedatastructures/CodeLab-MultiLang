import test.print

class_value = test.print.Print("hello python")

print(class_value)

try :
    raise Exception("exception")
except Exception as e :
    print(f"捕获到异常 :{e}")
finally :
    print("finally")