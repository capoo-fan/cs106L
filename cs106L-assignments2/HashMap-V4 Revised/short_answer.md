Assignment 2: STL HashMap (short answer questions)
Names:

1. 在 rehash() 函数中，for-each 循环包含一个 auto& 符号。该 auto 符号的推导类型是什么？为什么需要使用 & 符号？

Answer:node* &curr 对 _buckets_array 变量进行引用，后面要进行修改 

2.STL 容器存储 value_type 类型的元素，对于你的 HashMap 来说，这个 value_type 是一个 std::pair<const K, M> 如果 HashMap 类中的 value_type 是 std::pair，会出现什么问题？<K, M> ？

Answer:如果不用const 会导致修改某一个键值对的KEY后，该键值对位于不正确的桶中，使find函数无法正确找到

3.除了 HashMap::find 成员函数之外，STL 算法库中还有一个 std::find 函数。如果您在 HashMap m 中查找键 k，那么调用 m.find(k) 还是 std::find(m.begin(), m.end(), k) 更合适？

Answer: m.find(k) ,后者的时间复杂度是 O(N),前者是 O(1) 时间复杂度，前者的工作方式是找到k所在的桶，遍历所在的链表


4.这个 HashMap 类符合 RAII 标准。请解释一下原因
不符合，例如函数中出现了 delete 关键字。在erase函数中缺少delete,没有释放内存，从而导致内存泄漏

5.为什么 HashMapIterator 的构造函数是私有的？如果构造函数是私有的，HashMapIterator 又该如何构造呢？
Answer:私有构造函数：防止外部直接创建对象
6.简要解释一下我们为您提供的 HashMapIterator 的运算符 ++ 的实现。它是如何工作的？它包含哪些检查？

iterator主要是遍历每一个桶，当++之后发现是空指针，寻找下一个桶，直到找到一个非空的桶或者遍历完所有桶。

7.

8.

9.

10.
