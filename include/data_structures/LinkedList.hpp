#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <iostream>
#include <stdexcept>

template <typename T>
class LinkedList {
private:
    
    struct Node {
        T data;
        Node* next;
        Node* prev;

        Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    size_t length;

public:


    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}

        T& operator*() { return current->data; }
        
        Iterator& operator++() { // ++it
            if (current) current = current->next;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };


    class ConstIterator {
    private:
        Node* current;
    public:
        ConstIterator(Node* node) : current(node) {}

        const T& operator*() const { return current->data; }
        
        ConstIterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        bool operator!=(const ConstIterator& other) const {
            return current != other.current;
        }
    };

    
    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    
    ~LinkedList() {
        clear();
    }

    
    void clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
        tail = nullptr;
        length = 0;
    }


    void push_back(const T& value) {
        Node* newNode = new Node(value);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        length++;
    }

    
    size_t size() const { return length; }
    bool empty() const { return length == 0; }

   
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }
};

#endif