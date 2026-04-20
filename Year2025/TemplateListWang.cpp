#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtemplate-body"
#include <iostream>
static int a = 0;
template <class Linked_list_data>
class list_wang
{
private:
    class list_N
    {
    public:
        typedef struct list_definition
        {
            Linked_list_data *data;
            list_definition *next;
            list_definition *prev;
        } list_def;

        list_def *Deep_val;

        list_N()
        {
            a++;
            Deep_val = new list_def;
            Deep_val->next = nullptr;
            Deep_val->prev = nullptr;
        }
        list_N(const list_N &var)
        {
            Deep_val = nullptr;
            list_def *other_current = var.Deep_val;
            list_def *prev = nullptr;
            while (other_current != nullptr)
            {
                list_def *new_node = new list_def;
                new_node->data = other_current->data;
                new_node->prev = prev;
                if (prev != nullptr)
                {
                    prev->next = new_node;
                }
                else
                {
                    Deep_val = new_node;
                }
                prev = new_node;
                other_current = other_current->next;
            }
            if (prev != nullptr)
            {
                prev->next = nullptr;
            }
        }
        list_N &operator=(list_N &var)
        {
            if (this != &var)
            {
                // 先释放当前链表
                list_def *current = Deep_val;
                while (current != nullptr)
                {
                    list_def *next = current->next;
                    delete current;
                    current = next;
                }
                // 复制新链表
                list_def *other_current = var.Deep_val;
                list_def *prev = nullptr;
                while (other_current != nullptr)
                {
                    list_def *new_node = new list_def;
                    new_node->data = other_current->data;
                    new_node->prev = prev;
                    if (prev != nullptr)
                    {
                        prev->next = new_node;
                    }
                    else
                    {
                        Deep_val = new_node;
                    }
                    prev = new_node;
                    other_current = other_current->next;
                }
                if (prev != nullptr)
                {
                    prev->next = nullptr;
                }
            }
            return *this;
        }
        ~list_N()
        {
            delete Deep_val;
        }
    };
    list_N val;

public:
    list_wang()
    {
        ;
    }
    list_wang(const list_wang &var)
        : val(var.val)
    {
        ;
    }
    bool insert(Linked_list_data &Shallow_variable)
    {
        typename list_wang::list_N::list_def *new_node = new typename list_wang::list_N::list_def;
        if (new_node == nullptr || val.Deep_val == nullptr)
        {
            return false;
        }
        new_node->data = &Shallow_variable;
        new_node->next = val.Deep_val;
        if (val.Deep_val != nullptr)
        {
            val.Deep_val->prev = new_node;
        }
        val.Deep_val = new_node;
        return true;
        // bug : 改变指针
    }
    void list_print()
    {
        typename list_wang::list_N::list_def *current = val.Deep_val;
        while (current != nullptr)
        {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

int main()
{
    list_wang<int> t1;
    for (int i = 0; i < 10; i++)
    {
        t1.insert(i);
    }
    std::cout << a << std::endl;
    t1.list_print();
    return 0;
}

#pragma GCC diagnostic pop
