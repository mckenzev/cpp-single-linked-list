#pragma once

#include <compare>
#include <cstddef>
#include <exception>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value = {};
        Node* next_node = nullptr;
    };

    // Итератор
    template <typename ValueType>
    class BasicIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        [[nodiscard]] constexpr bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] constexpr bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] constexpr bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] constexpr bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }
        
        BasicIterator operator++(int) noexcept {
            BasicIterator old = *this;
            ++(*this);
            return old;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &(node_->value);
        }

        BasicIterator& operator = (const BasicIterator<std::remove_const_t<ValueType>>& other) {
            node_ = other.node_;
            return *this;
        }

        BasicIterator() = default;
        BasicIterator(const BasicIterator<std::remove_const_t<ValueType>>& other) : node_(other.node_) {}
        
    private:
        explicit BasicIterator(Node* node) : node_(node) {}
        friend class SingleLinkedList;
        Node* node_ = nullptr;
    };

public:
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> items) {
        SingleLinkedList tmp;
        Node** next_node = &tmp.head_.next_node;
        for (auto& item : items) {
            *next_node = new Node(item, nullptr);
            next_node = &((*next_node)->next_node);
            ++tmp.size_;
        }

        swap(tmp);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        Node** next_node = &tmp.head_.next_node;
        for (auto& item : other) {
            *next_node = new Node(item, nullptr);
            next_node = &((*next_node)->next_node);
            ++tmp.size_;
        }

        swap(tmp);
    }

    ~SingleLinkedList() {
        Clear();
    }
    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void PopFront() noexcept {
        EraseAfter(before_begin());
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator(new_node);
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* to_del = pos.node_->next_node;
        pos.node_->next_node = pos.node_->next_node->next_node;
        delete to_del;
        --size_;
        return Iterator(pos.node_->next_node);
    }

    // Очищает список за время O(N)
    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* to_del = head_.next_node;
            head_.next_node = to_del->next_node;
            delete to_del;
        }
        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    bool constexpr operator==(const SingleLinkedList& other) const noexcept {
        return size_ == other.size_ && std::equal(cbegin(), cend(), other.cbegin());
    }

    auto constexpr operator<=>(const SingleLinkedList& other) const noexcept {
        return std::lexicographical_compare_three_way(begin(), end(), other.begin(), other.end());
    }

    SingleLinkedList& operator=(const SingleLinkedList& other) {
        if (&other != this) {
            SingleLinkedList tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        
        return Iterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}