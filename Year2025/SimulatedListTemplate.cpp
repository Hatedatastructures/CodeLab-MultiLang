#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <iostream>
using namespace std;
#include <assert.h>

namespace test_list
{
    // List的节点类
    template<class T>
    struct ListNode
    {
        ListNode(const T& val = T())
            : _prev(nullptr)
            , _next(nullptr)
            , _val(val)
        {}

        ListNode<T>* _prev;
        ListNode<T>* _next;
        T _val;
    };

    /*
    list 的迭代器
    迭代器有两种实现方式，具体应根据容器底层数据结构实现：
      1. 原生态指针，比如：vector
      2. 将原生态指针进行封装，因迭代器使用形式与指针完全相同，因此在自定义的类中必须实现以下方法：
         1. 指针可以解引用，迭代器的类中必须重载operator*()
         2. 指针可以通过->访问其所指空间成员，迭代器类中必须重载oprator->()
         3. 指针可以++向后移动，迭代器类中必须重载operator++()与operator++(int)
            至于operator--()/operator--(int)释放需要重载，根据具体的结构来抉择，双向链表可以向前             移动，所以需要重载，如果是forward_list就不需要重载--
         4. 迭代器需要进行是否相等的比较，因此还需要重载operator==()与operator!=()
    */
    template<class T, class Ref, class Ptr>
    class ListIterator
    {
        typedef ListNode<T> Node;
        typedef ListIterator<T, Ref, Ptr> self;

        // Ref 和 Ptr 类型需要重定义下，实现反向迭代器时需要用到
    public:
        using IteratorRef = Ref;
        using IteratorPtr = Ptr;
    public:
        //////////////////////////////////////////////
        // 构造
        ListIterator(Node* node = nullptr)
            : _node(node)
        {}

        //////////////////////////////////////////////
        // 具有指针类似行为
        Ref operator*() 
        { 
            return _node->_val;
        }

        Ptr operator->() 
        { 
            return &(operator*()); 
        }

        //////////////////////////////////////////////
        // 迭代器支持移动
        self& operator++()
        {
            _node = _node->_next;
            return *this;
        }

        self operator++(int)
        {
            self temp(*this);
            _node = _node->_next;
            return temp;
        }

        self& operator--()
        {
            _node = _node->_prev;
            return *this;
        }

        self operator--(int)
        {
            self temp(*this);
            _node = _node->_prev;
            return temp;
        }

        //////////////////////////////////////////////
        // 迭代器支持比较
        bool operator!=(const self& l)const
        { 
            return _node != l._node;
        }

        bool operator==(const self& l)const
        { 
            return _node != l._node;
        }

        Node* _node;
    };

    template<class Iterator>
    class ReverseListIterator
    {
        // 注意：此处typename的作用是明确告诉编译器，Ref是Iterator类中的一个类型，而不是静态成员变量
        // 否则编译器编译时就不知道Ref是Iterator中的类型还是静态成员变量
        // 因为静态成员变量也是按照 类名::静态成员变量名 的方式访问的
    public:
        using IteratorRef = typename Iterator::IteratorRef;
        using IteratorPtr = typename Iterator::IteratorPtr;
        typedef ReverseListIterator<Iterator> self;
    public:
        //////////////////////////////////////////////
        // 构造
        ReverseListIterator(Iterator it)
            : _it(it)
        {}

        //////////////////////////////////////////////
        // 具有指针类似行为
        Ref operator*()
        {
            Iterator temp(_it);
            --temp;
            return *temp;
        }

        Ptr operator->()
        {
            return &(operator*());
        }

        //////////////////////////////////////////////
        // 迭代器支持移动
        self& operator++()
        {
            --_it;
            return *this;
        }

        self operator++(int)
        {
            self temp(*this);
            --_it;
            return temp;
        }

        self& operator--()
        {
            ++_it;
            return *this;
        }

        self operator--(int)
        {
            self temp(*this);
            ++_it;
            return temp;
        }

        //////////////////////////////////////////////
        // 迭代器支持比较
        bool operator!=(const self& l)const
        {
            return _it != l._it;
        }

        bool operator==(const self& l)const
        {
            return _it != l._it;
        }

        Iterator _it;
    };

    template<class T>
    class list
    {
        typedef ListNode<T> Node;

    public:
        // 正向迭代器
        typedef ListIterator<T, T&, T*> iterator;
        typedef ListIterator<T, const T&, const T&> const_iterator;

        // 反向迭代器
        typedef ReverseListIterator<iterator> reverse_iterator;
        typedef ReverseListIterator<const_iterator> const_reverse_iterator;
    public:
        ///////////////////////////////////////////////////////////////
        // List的构造
        list()
        {
            CreateHead();
        }

        list(int n, const T& value = T())
        {
            CreateHead();
            for (int i = 0; i < n; ++i)
                push_back(value);
        }

        template <class Iterator>
        list(Iterator first, Iterator last)
        {
            CreateHead();
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }

        list(const list<T>& l)
        {
            CreateHead();

            // 用l中的元素构造临时的temp,然后与当前对象交换
            list<T> temp(l.begin(), l.end());
            this->swap(temp);
        }

        list<T>& operator=(list<T> l)
        {
            this->swap(l);
            return *this;
        }

        ~list()
        {
            clear();
            delete _head;
            _head = nullptr;
        }

        ///////////////////////////////////////////////////////////////
        // List的迭代器
        iterator begin() 
        { 
            return iterator(_head->_next); 
        }

        iterator end() 
        { 
            return iterator(_head); 
        }

        const_iterator begin()const 
        { 
            return const_iterator(_head->_next); 
        }

        const_iterator end()const
        { 
            return const_iterator(_head); 
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rbegin()const
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator rend()const
        {
            return const_reverse_iterator(begin());
        }

        ///////////////////////////////////////////////////////////////
        // List的容量相关
        size_t size()const
        {
            Node* cur = _head->_next;
            size_t count = 0;
            while (cur != _head)
            {
                count++;
                cur = cur->_next;
            }

            return count;
        }

        bool empty()const
        {
            return _head->_next == _head;
        }

        void resize(size_t newsize, const T& data = T())
        {
            size_t oldsize = size();
            if (newsize <= oldsize)
            {
                // 有效元素个数减少到newsize
                while (newsize < oldsize)
                {
                    pop_back();
                    oldsize--;
                }
            }
            else
            {
                while (oldsize < newsize)
                {
                    push_back(data);
                    oldsize++;
                }
            }
        }
        ////////////////////////////////////////////////////////////
        // List的元素访问操作
        // 注意：List不支持operator[]
        T& front()
        {
            return _head->_next->_val;
        }

        const T& front()const
        {
            return _head->_next->_val;
        }

        T& back()
        {
            return _head->_prev->_val;
        }

        const T& back()const
        {
            return _head->_prev->_val;
        }

        ////////////////////////////////////////////////////////////
        // List的插入和删除
        void push_back(const T& val) 
        { 
            insert(end(), val); 
        }

        void pop_back() 
        { 
            erase(--end()); 
        }

        void push_front(const T& val) 
        { 
            insert(begin(), val); 
        }

        void pop_front() 
        { 
            erase(begin()); 
        }

        // 在pos位置前插入值为val的节点
        iterator insert(iterator pos, const T& val)
        {
            Node* pNewNode = new Node(val);
            Node* pCur = pos._node;
            // 先将新节点插入
            pNewNode->_prev = pCur->_prev;
            pNewNode->_next = pCur;
            pNewNode->_prev->_next = pNewNode;
            pCur->_prev = pNewNode;
            return iterator(pNewNode);
        }

        // 删除pos位置的节点，返回该节点的下一个位置
        iterator erase(iterator pos)
        {
            // 找到待删除的节点
            Node* pDel = pos._node;
            Node* pRet = pDel->_next;

            // 将该节点从链表中拆下来并删除
            pDel->_prev->_next = pDel->_next;
            pDel->_next->_prev = pDel->_prev;
            delete pDel;

            return iterator(pRet);
        }

        void clear()
        {
            Node* cur = _head->_next;
            
            // 采用头删除删除
            while (cur != _head)
            {
                _head->_next = cur->_next;
                delete cur;
                cur = _head->_next;
            }

            _head->_next = _head->_prev = _head;
        }

        void swap(test_list::list<T>& l)
        {
            std::swap(_head, l._head);
        }

    private:
        void CreateHead()
        {
            _head = new Node;
            _head->_prev = _head;
            _head->_next = _head;
        }
    private:
        Node* _head;
    };
}


///////////////////////////////////////////////////////
// 对模拟实现的list进行测试
// 正向打印链表
template<class T>
void PrintList(const test_list::list<T>& l)
{
    auto it = l.begin();
    while (it != l.end())
    {
        cout << *it << " ";
        ++it;
    }

    cout << endl;
}

// 测试List的构造
void TestBiteList1()
{
    test_list::list<int> l1;
    test_list::list<int> l2(10, 5);
    PrintList(l2);

    int array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    test_list::list<int> l3(array, array + sizeof(array) / sizeof(array[0]));
    PrintList(l3);

    test_list::list<int> l4(l3);
    PrintList(l4);

    l1 = l4;
    PrintList(l1);
}

// PushBack()/PopBack()/PushFront()/PopFront()
void TestBiteList2()
{
    // 测试PushBack与PopBack
    test_list::list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    PrintList(l);

    l.pop_back();
    l.pop_back();
    PrintList(l);

    l.pop_back();
    cout << l.size() << endl;

    // 测试PushFront与PopFront
    l.push_front(1);
    l.push_front(2);
    l.push_front(3);
    PrintList(l);

    l.pop_front();
    l.pop_front();
    PrintList(l);

    l.pop_front();
    cout << l.size() << endl;
}

// 测试insert和erase
void TestBiteList3()
{
    int array[] = { 1, 2, 3, 4, 5 };
    test_list::list<int> l(array, array + sizeof(array) / sizeof(array[0]));

    auto pos = l.begin();
    l.insert(l.begin(), 0);
    PrintList(l);

    ++pos;
    l.insert(pos, 2);
    PrintList(l);

    l.erase(l.begin());
    l.erase(pos);
    PrintList(l);

    // pos指向的节点已经被删除，pos迭代器失效
    cout << *pos << endl;

    auto it = l.begin();
    while (it != l.end())
    {
        it = l.erase(it);
    }
    cout << l.size() << endl;
}

// 测试反向迭代器
void TestBiteList4()
{
    int array[] = { 1, 2, 3, 4, 5 };
    test_list::list<int> l(array, array + sizeof(array) / sizeof(array[0]));

    auto rit = l.rbegin();
    while (rit != l.rend())
    {
        cout << *rit << " ";
        ++rit;
    }
    cout << endl;

    const test_list::list<int> cl(l);
    auto crit = l.rbegin();
    while (crit != l.rend())
    {
        cout << *crit << " ";
        ++crit;
    }
    cout << endl;

#pragma GCC diagnostic pop
