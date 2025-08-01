/*
 * Assignment 2: HashMap template implementation (STARTER CODE)
 *      TODO: write a comment here.
 */

#include "hashmap.h"
#include <sstream>

// See milestone 2 about delegating constructors (when HashMap is called in the initalizer list below)
template <typename K, typename M, typename H> HashMap<K, M, H>::HashMap() : HashMap{kDefaultBuckets}
{
}

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H &hash)
    : _size{0}, _hash_function{hash}, _buckets_array{bucket_count, nullptr}
{
} // 初始化函数
/*
__buckets_array是一个vector，存储每个桶的链表头指针
*/

template <typename K, typename M, typename H> HashMap<K, M, H>::~HashMap()
{
    clear();
} // 清空函数

template <typename K, typename M, typename H> inline size_t HashMap<K, M, H>::size() const noexcept
{
    return _size;
} // 返回size

template <typename K, typename M, typename H> inline bool HashMap<K, M, H>::empty() const noexcept
{
    return size() == 0;
} // 判断是否为空

template <typename K, typename M, typename H> inline float HashMap<K, M, H>::load_factor() const noexcept
{
    return static_cast<float>(size()) / bucket_count();
}; // 返回负载因子,用于判断是否需要rehash

template <typename K, typename M, typename H> inline size_t HashMap<K, M, H>::bucket_count() const noexcept
{
    return _buckets_array.size();
}; // 返回桶的数量

template <typename K, typename M, typename H> M &HashMap<K, M, H>::at(const K &key)
{
    auto [prev, node_found] = find_node(key); // 查找对应key的节点
    if (node_found == nullptr)
    {
        throw std::out_of_range("HashMap<K, M, H>::at: key not found");
    }
    return node_found->value.second;
} // 返回对应key的value，可修改和读取

template <typename K, typename M, typename H> const M &HashMap<K, M, H>::at(const K &key) const
{
    // see static_cast/const_cast trick explained in find().
    return static_cast<const M &>(const_cast<HashMap<K, M, H> *>(this)->at(key));
} // 返回对应key的value, const版本，不可修改，只可读取

template <typename K, typename M, typename H> bool HashMap<K, M, H>::contains(const K &key) const noexcept
{
    return find_node(key).second != nullptr;
} // 判断是否包含key

template <typename K, typename M, typename H> void HashMap<K, M, H>::clear() noexcept
{
    for (auto &curr : _buckets_array)
    {
        while (curr != nullptr)
        {
            auto trash = curr;
            curr = curr->next;
            delete trash; // 不符合RAII,以后可用智能指针代替
        }
    }
    _size = 0;
} // 清空哈希表，删除所有节点

template <typename K, typename M, typename H> typename HashMap<K, M, H>::iterator HashMap<K, M, H>::find(const K &key)
{
    return make_iterator(find_node(key).second);
} // 查找key对应的节点，返回一个迭代器

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::find(const K &key) const
{
    // This is called the static_cast/const_cast trick, which allows us to reuse
    // the non-const version of find to implement the const version.
    // The idea is to cast this so it's pointing to a non-const HashMap, which
    // calls the overload above (and prevent infinite recursion).
    // Also note that we are calling the conversion operator in the iterator class!
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H> *>(this)->find(key));
} // 查找key对应的节点，返回一个const迭代器,只读不能修改

template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::iterator, bool> HashMap<K, M, H>::insert(const value_type &value)
{
    const auto &[key, mapped] = value;
    auto [prev, node_to_edit] = find_node(key);
    size_t index = _hash_function(key) % bucket_count();

    if (node_to_edit != nullptr)
    {
        return {make_iterator(node_to_edit), false};
    }

    auto temp = new node(value, _buckets_array[index]);
    _buckets_array[index] = temp;

    ++_size;
    return {make_iterator(temp), true};
} // 插入一个键值对，如果key已存在则返回false，否则返回true

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K &key) const
{
    size_t index = _hash_function(key) % bucket_count();
    node *curr = _buckets_array[index];
    node *prev = nullptr; // if first node is the key, return {nullptr, front}
    while (curr != nullptr)
    {
        const auto &[found_key, found_mapped] = curr->value;
        if (found_key == key)
        {
            return {prev, curr};
        }
        prev = curr;
        curr = curr->next;
    }
    return {nullptr, nullptr}; // key not found at all.
} // 查找key对应的节点，返回一个pair，包含前一个节点和当前节点

template <typename K, typename M, typename H> typename HashMap<K, M, H>::iterator HashMap<K, M, H>::begin() noexcept
{
    size_t index = first_not_empty_bucket();
    if (index == bucket_count())
    {
        return end();
    }
    return make_iterator(_buckets_array[index]);
} // 返回一个迭代器，指向第一个非空桶的第一个节点

template <typename K, typename M, typename H> typename HashMap<K, M, H>::iterator HashMap<K, M, H>::end() noexcept
{
    return make_iterator(nullptr);
} // 返回一个迭代器，指向空节点

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::begin() const noexcept
{
    // see static_cast/const_cast trick explained in find().
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H> *>(this)->begin());
} // 返回一个const迭代器，指向第一个非空桶的第一个节点

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::const_iterator HashMap<K, M, H>::end() const noexcept
{
    // see static_cast/const_cast trick explained in find().
    return static_cast<const_iterator>(const_cast<HashMap<K, M, H> *>(this)->end());
} // 返回一个const迭代器，指向空节点

template <typename K, typename M, typename H> size_t HashMap<K, M, H>::first_not_empty_bucket() const noexcept
{
    auto isNotNullptr = [](const auto &v) { return v != nullptr; };

    auto found = find_if(_buckets_array.begin(), _buckets_array.end(), isNotNullptr);
    return found - _buckets_array.begin();
} // 查找第一个非空桶的索引

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::make_iterator(node *curr)
{
    if (curr == nullptr)
    {
        return {&_buckets_array, curr, bucket_count()};
    }
    size_t index = _hash_function(curr->value.first) % bucket_count();
    return {&_buckets_array, curr, index};
}

template <typename K, typename M, typename H> bool HashMap<K, M, H>::erase(const K &key)
{
    auto [prev, node_to_erase] = find_node(key);
    if (node_to_erase == nullptr)
    {
        return false;
    }
    size_t index = _hash_function(key) % bucket_count();
    (prev ? prev->next : _buckets_array[index]) = node_to_erase->next;
    --_size;
    return true;
} // 删除key对应的节点，如果不存在则返回false，存在则返回true

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::iterator HashMap<K, M, H>::erase(typename HashMap<K, M, H>::const_iterator pos)
{
    erase(pos++->first);
    return make_iterator(pos._node); // unfortunately we need a regular iterator, not a const_iterator
}

template <typename K, typename M, typename H> void HashMap<K, M, H>::debug() const
{
    std::cout << std::setw(30) << std::setfill('-') << '\n'
              << std::setfill(' ') << "Printing debug information for your HashMap implementation\n"
              << "Size: " << size() << std::setw(15) << std::right << "Buckets: " << bucket_count() << std::setw(20)
              << std::right << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

    for (size_t i = 0; i < bucket_count(); ++i)
    {
        std::cout << "[" << std::setw(3) << i << "]:";
        node *curr = _buckets_array[i];
        while (curr != nullptr)
        {
            const auto &[key, mapped] = curr->value;
            // next line will not compile if << not supported for K or M
            std::cout << " -> " << key << ":" << mapped;
            curr = curr->next;
        }
        std::cout << " /" << '\n';
    }
    std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ');
}

template <typename K, typename M, typename H> void HashMap<K, M, H>::rehash(size_t new_bucket_count)
{
    if (new_bucket_count == 0)
    {
        throw std::out_of_range("HashMap<K, M, H>::rehash: new_bucket_count must be positive.");
    }

    std::vector<node *> new_buckets_array(new_bucket_count);
    for (auto &curr : _buckets_array)
    { // short answer question is asking about this 'curr'
        while (curr != nullptr)
        {
            const auto &[key, mapped] = curr->value;
            size_t index = _hash_function(key) % new_bucket_count;

            auto temp = curr;
            curr = temp->next;
            temp->next = new_buckets_array[index];
            new_buckets_array[index] = temp;
        }
    }
    _buckets_array = std::move(new_buckets_array);
}

/* begin student code */

// Milestone 2 (optional) - iterator-based constructors
// You will have to type in your own function headers in both the .cpp and .h files.
template <typename K, typename M, typename H>
template <typename InputIt>
HashMap<K, M, H>::HashMap(InputIt first, InputIt last, size_t bucket_count, const H &hash) : HashMap(bucket_count, hash)
{
    for (auto it = first; it != last; it++)
    {
        this->insert(*it); // insert will handle the case where the key already exists
    }
}

// initializer list
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(std::initializer_list<value_type> init, size_t bucket_count, const H &hash)
    : HashMap(bucket_count, hash)
{
    for (auto it : init)
    {
        this->insert(it); // insert will handle the case where the key already exists
    }
}
/*
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(std::initializer_list<value_type> init, size_t bucket_count, const H &hash):
HashMap(bucket_count, hash)
{
    for (auto it : init)
    {
        this->insert(it);
    }
}
*/
// Milestone 3 (required) - operator overloading
// The function headers are provided for you.
template <typename K, typename M, typename H> M &HashMap<K, M, H>::operator[](const K &key)
{
    /*
     * The following code is returning a reference to memory that is on the stack.
     * This is a very common mistake, do not make the same mistake!
     * We wanted the starter code to compile without warnings, which is why
     * you see these weird compiler directives.
     * Please remove them before you start writing code.
     */

    // BEGIN STARTER CODE (remove these lines before you begin)
    (void)key;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-stack-address"

    M trash;      // trash lives on the stack
    return trash; // return value is a reference to a variable that has been freed :(

#pragma GCC diagnostic pop

    // END STARTER CODE
    return insert({key, M{}}).first->second; // M{}是默认初始化对象

    // complete the function implementation (1 line of code)
    // isn't it funny how the bad starter code is longer than the correct answer?
}

template <typename K, typename M, typename H> bool operator==(const HashMap<K, M, H> &lhs, const HashMap<K, M, H> &rhs)
{

    // BEGIN STARTER CODE (remove these lines before you begin)
    if (lhs.size() != rhs.size())
        return false;
    for (auto it : lhs)
    {
        auto [key, value] = *it;
        if (rhs.at(key) == rhs.end() || value != rhs.at(key))
            return false;
    }
    return true;
    // END STARTER CODE

    // complete the function implementation (~4-5 lines of code)
}

template <typename K, typename M, typename H> bool operator!=(const HashMap<K, M, H> &lhs, const HashMap<K, M, H> &rhs)
{

    // BEGIN STARTER CODE (remove these lines before you begin)
    return !(lhs == rhs); // use the operator== function
    // END STARTER CODE

    // complete the function implementation (1 line of code)
}

template <typename K, typename M, typename H> std::ostream &operator<<(std::ostream &os, const HashMap<K, M, H> &rhs)
{

    // BEGIN STARTER CODE (remove these lines before you begin)
    os << "{ ";
    std::istringstream ss;
    for (auto it : rhs)
    {
        ss << it->first << ":" << it->second << " ";
    }
    os << "}";
    return os;
    // END STARTER CODE

    // complete the function implementation (~7 lines of code)
}

// Milestone 4 (required) - special member functions
// You will have to type in your own function headers in both the .cpp and .h files.

// provide the function headers and implementations (~35 lines of code)
//
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(const HashMap<K, M, H> &other) : HashMap(other.bucket_count(), other._hash_function)
{
    for (const auto &[key, value] : other)
    {
        insert({key, value});
    }
} // 拷贝构造函数

template <typename K, typename M, typename H>
HashMap<K, M, H> &HashMap<K, M, H>::operator=(const HashMap<K, M, H> &other)
{
    if (this != &other) // 防止自赋值
    {
        this->clear();                                        // 清空当前哈希表
        _size = other._size;                                  // 复制大小
        _hash_function = other._hash_function;                // 复制哈希函数
        _buckets_array.resize(other.bucket_count(), nullptr); // 调整桶的数量

        for (const auto &[key, value] : other)
        {
            insert({key, value}); // 插入每个键值对
        }
    }
    return *this; // 返回当前对象的引用
}
// 复制赋值运算符

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap<K, M, H> &&other) noexcept
    : _size(other._size),                              // 使用初始化列表
      _hash_function(std::move(other._hash_function)), // 移动初始化
      _buckets_array(std::move(other._buckets_array))  // 移动初始化
{
    // 重置 other 到有效状态
    other._size = 0;
    other._buckets_array.clear();
    other._buckets_array.resize(kDefaultBuckets, nullptr); // 确保有效状态
} // 移动构造函数

template <typename K, typename M, typename H>
HashMap<K, M, H> &HashMap<K, M, H>::operator=(HashMap<K, M, H> &&other) noexcept
{
    if (this != &other) // 防止自赋值
    {
        this->clear();                                    // 清空当前哈希表
        _size = other._size;                              // 复制大小
        _hash_function = std::move(other._hash_function); // 移动哈希函数
        _buckets_array = std::move(other._buckets_array); //
        other._size = 0;
        other._buckets_array.clear();
        other._buckets_array.resize(kDefaultBuckets, nullptr); // 确保有效状态
    }
    return *this; // 返回当前对象的引用
}// 移动赋值运算符
/* end student code */
