#include <iostream>
//模拟实现list
namespace wa
{ 
    template<typename list_type_function_node>
    struct listNode
    {
        //节点类
        listNode<list_type_function_node>* _prev;
        listNode<list_type_function_node>* _next;
        list_type_function_node _data;

        listNode(const list_type_function_node& data = list_type_function_node())
        :_prev(nullptr), _next(nullptr), _data(data)
        {
            //列表初始化
        }
        
    };
    template <typename list_type ,typename Ref ,typename Ptr >
    class _list_iterator_
    {
    public:
        //迭代器类
        typedef listNode<list_type> Node;
        typedef _list_iterator_<list_type ,list_type& ,list_type*> iterator;
        typedef Ref reference;
        typedef Ptr pointer;
        Node* _node;
        _list_iterator_(Node* node)
        :_node(node)
        {
            //拿一个指针来构造迭代器
        }
        Ref operator*()
        {
            //返回该节点的自定义类型的数据
            return _node->_data;
        }
        _list_iterator_& operator++()
        {
            //先加在用
            _node = _node -> _next;
            return *this;
            //返回类型名，如果为迭代器就会因为const 报错
        }
        _list_iterator_ operator++(int)
        {
            //先用在加
            _list_iterator_ temp(_node);
            _node = _node->_next;
            //把本体指向下一个位置
            return temp;
        }
        _list_iterator_& operator--()
        {
            _node = _node->_prev;
            return *this;
        }
        _list_iterator_ operator--(int)
        {
            _list_iterator_ temp (_node);
            _node = _node->_prev;
            return temp;
        }
        bool operator!= (const _list_iterator_& _iterator_temp_)
        {
            //比较两个指针及其上一个和下一个指针地址
            return _node != _iterator_temp_._node;
        }
        Ptr operator->()
        {
            return &(_node->_data);
        }
    };
    template <typename iterator>
    class _Reverse_list_iterator_
    {
        //创建反向迭代器
        typedef typename iterator::reference Ref;
        typedef typename iterator::pointer Ptr;
        typedef _Reverse_list_iterator_<iterator> ConstReverseListIterator;
    public:
        iterator _it;
        _Reverse_list_iterator_(iterator it)
        :_it(it)
        {
            ;
        } 
        Ref& operator*()
        {
            //因为反向迭代器起始位置在哨兵节点所以通过指向上一个来找到准确位置
            //正好到rend位置停下来的时候已经遍历到rend位置
            iterator temp(_it);
            --(temp);
            return *temp;
        }
        Ptr operator->()
        {
            //两者函数差不多可直接调用
            return &(operator*());
        }
        _Reverse_list_iterator_& operator++()
        {
            --_it;
            return *this;
        }
        _Reverse_list_iterator_ operator++(int)
        {
            _Reverse_list_iterator_ _temp (_it);
            --_it;
            return _temp;
        }
        _Reverse_list_iterator_& operator--()
        {
            ++_it;
            return *this;
        }
        _Reverse_list_iterator_ operator--(int)
        {
            _Reverse_list_iterator_ _temp (_it);
            ++_it;
            return _temp;
        }
        bool operator!=(const ConstReverseListIterator& _temp_)
        {
            return _it != _temp_._it;
        }
    };
    template <typename list_type>
    class list
    {
        typedef listNode<list_type> Node;

        Node* _head;
        //_head为哨兵位
        void CreateHead()
        {
            _head = new Node;
            _head -> _prev = _head;
            _head -> _next = _head;
        }
    public:
        typedef _list_iterator_<list_type,list_type& ,list_type*> iterator;
        typedef _list_iterator_<list_type,const list_type&,const list_type*> const_iterator;

        //拿正向迭代器构造反向迭代器，可以直接调用 iterator 已经重载的运算符和函数，相当于在封装一层类
        typedef _Reverse_list_iterator_<iterator> reverse_iterator;
        typedef _Reverse_list_iterator_<const_iterator> reverse_const_iterator;
        list()
        {
            CreateHead();
        }
        ~list()
        {
            clear();
            delete _head;
            _head = nullptr;
        }
        list(iterator first , iterator last)
        {
            //通过另一个list对象构建一个list
            CreateHead();
            //已经创建一个哨兵节点
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }
        list(const_iterator first , const_iterator last)
        {
            //通过另一个list对象构建一个list
            CreateHead();
            //已经创建一个哨兵节点
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }
        list(const list<list_type>& _list_data)
        {
            //拷贝构造
            CreateHead();
            list<list_type> _temp_ (_list_data.cbegin(),_list_data.cend());
            swap(_temp_);
        }
        void swap(wa::list<list_type>& _swap_temp)
        {
            std::swap(_head,_swap_temp._head);
        }
        iterator begin()
        {
            //因为_head为哨兵位，所以哨兵下一个结点为有效数据
            return iterator(_head ->_next);
        }
        iterator end()
        {
            return iterator(_head);
        }
        const_iterator cbegin()const
        {
            //因为_head为哨兵位，所以哨兵下一个结点为有效数据
            return const_iterator(_head ->_next);
        }
        const_iterator cend()const
        {
            return const_iterator(_head);
        }
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
        reverse_iterator rbegin()
        {
            return reverse_iterator(end());
        }
        reverse_iterator rend()
        {
            return reverse_iterator(begin());
        }
        reverse_const_iterator rcbegin()const
        {
            return reverse_const_iterator(cend());
        }
        reverse_const_iterator rcend()const
        {
            return reverse_const_iterator(cbegin());
        }
        /*
        元素访问操作
        */
        list_type& front()
        {
            return _head->_next->_data;
        }

        const list_type& front()const
        {
            return _head->_next->_data;
        }

        list_type& back()
        {
            return _head->_prev->_data;
        }

        const list_type& back()const
        {
            return _head->_prev->_data;
        }
        /*
        插入删除操作
        */
        void push_back(const list_type& push_back_data)
        {
            insert(end(),push_back_data);
        }
        void push_front(const list_type& push_front_data)
        {
            //插入到头
            insert(begin(),push_front_data);
        }
        void pop_back() 
        { 
            //删除尾
            erase(--end()); 
        }
        void pop_front() 
        { 
            //删除头
            erase(begin()); 
        }
        iterator insert(iterator pos ,const list_type& val)
        {
            Node* Pnew_node = new Node(val);
            //开辟新节点
            Node* Pcur = pos._node;
            //保存pos位置的值
            Pnew_node->_prev = Pcur->_prev;
            Pnew_node->_next = Pcur;
            Pnew_node->_prev->_next = Pnew_node;
            Pcur->_prev = Pnew_node;
            return iterator(Pnew_node);
        }
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
        void resize(size_t newsize, const list_type& data = list_type())
        {
            //将data插入到链表中
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
        void clear()
        {
            //循环释放资源
            Node* cur = _head->_next;
            // 采用头删除
            while (cur != _head)
            {
                _head->_next = cur->_next;
                delete cur;
                cur = _head->_next;
            }

            _head->_next = _head->_prev = _head;
        }
        list& operator=(list<list_type> _lsit_temp)
        {
            //运算符重载
            if( this != &_lsit_temp)
            {
                swap(_lsit_temp);
            }
            return *this;
        }
        list operator+(const list<list_type>& _list_temp_)
        {
            list<list_type> _return_temp_ (cbegin(),cend());
            const_iterator _begin = _list_temp_.cbegin();
            const_iterator _end  = _list_temp_.cend();
            while(_begin != _end)
            {
                _return_temp_.push_back(*_begin);
                ++_begin;
            }
            return _return_temp_;
        }
        list& operator+=(const list<list_type>& _lsit_temp_)
        {
            const_iterator _begin = _lsit_temp_.cbegin();
            const_iterator _end  = _lsit_temp_.cend();
            while(_begin != _end)
            {
                push_back(*_begin);
                ++_begin;
            }
            return *this;
        }
        template <typename const_list_output_templates>
        friend std::ostream& operator<< (std::ostream& list_ostream, const list<const_list_output_templates>& Dynamic_arrays_data);
        
        template <typename ListOutputTemplates>
        friend std::ostream& operator<< (std::ostream& list_ostream, list<ListOutputTemplates>& Dynamic_arrays_data);
    };
    template <typename const_list_output_templates>
    std::ostream& operator<< (std::ostream& list_ostream, const list<const_list_output_templates>& Dynamic_arrays_data)
    {
        //typename声明这是一个类型而不是表达式
        typename list<const_list_output_templates>::const_iterator it = Dynamic_arrays_data.cbegin();
        while (it != Dynamic_arrays_data.cend()) 
        {
            list_ostream << *it << " ";
            ++it;
        }
        return list_ostream;
    }

    template <typename ListOutputTemplates>
    std::ostream& operator<< (std::ostream& list_ostream, list<ListOutputTemplates>& Dynamic_arrays_data)
    {
        //typename声明这是一个类型而不是表达式
        typename list<ListOutputTemplates>::iterator it = Dynamic_arrays_data.begin();
        while (it != Dynamic_arrays_data.end()) 
        {
            list_ostream << *it << " ";
            ++it;
        }
        return list_ostream;
    }
}
int main()
{
    wa::list<int> test1;
    for(size_t i = 1; i < 10; i++)
    {
        test1.push_back(i);
    }
    wa::list<int>::const_iterator it =test1.cbegin();
    while(it != test1.cend())
    {
        std::cout << *it  << " ";
        it++;
    }
    std::cout << std::endl;
    wa::list<int>::reverse_const_iterator i = test1.rcbegin();
    while(i != test1.rcend())
    {
        std::cout << *i << " ";
        i++;
    }
    std::cout <<std::endl;

    test1.pop_back(); 
    wa::list<int>::const_iterator j =test1.cbegin();
    while(j != test1.cend())
    {
        std::cout << *j  << " ";
        j++;
    }
    std::cout << std::endl;
    std::cout << test1.size() << std::endl;

    wa::list<int> test2 = test1;
    wa::list<int>::const_iterator p =test2.cbegin();
    while(p != test2.cend())
    {
        std::cout << *p  << " ";
        p++;
    }
    std::cout << std::endl;
    std::cout << test2.size() << std::endl;

    wa::list<int> test3 = test2 + test1;
    wa::list<int>::const_iterator k =test3.cbegin();
    while(k != test3.cend())
    {
        std::cout << *k  << " ";
        k++;
    }
    std::cout << std::endl;
    std::cout << test3.size() << std::endl;

    wa::list<int> test4 = test3 + test1;
    wa::list<int>::const_iterator kp =test4.cbegin();
    while(kp != test4.cend())
    {
        std::cout << *kp  << " ";
        kp++;
    }
    std::cout << std::endl;
    std::cout << test4.size() << std::endl;
    std::cout << test4 << std::endl;
    return 0;
}