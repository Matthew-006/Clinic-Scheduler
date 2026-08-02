#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Mohamed Ayman joined - Linked List (Data Structures layer)
// used for done patients and all patients

template <class T>
class LinkedList {
private:
    struct Node {
        T item;
        Node* next;
        Node(T val) {
            item = val;
            next = 0;
        }
    };
    Node* head;
    Node* tail;
    int count;

public:
    LinkedList() {
        head = 0;
        tail = 0;
        count = 0;
    }

    ~LinkedList() {
        Node* cur = head;
        while (cur != 0) {
            Node* temp = cur;
            cur = cur->next;
            delete temp;
        }
        head = 0;
        tail = 0;
        count = 0;
    }

    bool isEmpty() {
        return head == 0;
    }

    int size() {
        return count;
    }

    void insertEnd(T val) {
        Node* n = new Node(val);
        if (head == 0) {
            head = n;
            tail = n;
        }
        else {
            tail->next = n;
            tail = n;
        }
        count++;
    }

    T findById(int id) {
        Node* cur = head;
        while (cur != 0) {
            if (cur->item != 0 && cur->item->id == id)
                return cur->item;
            cur = cur->next;
        }
        return 0;
    }
};

#endif
