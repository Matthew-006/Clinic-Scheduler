#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Mohamed Ayman - Linked List, used for done patients and all patients

template <class T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
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
    }

    bool isEmpty() {
        return head == 0;
    }

    int getCount() {
        return count;
    }

    void insertEnd(T value) {
        Node* temp = new Node();
        temp->data = value;
        temp->next = 0;

        if (head == 0) {
            head = temp;
            tail = temp;
        }
        else {
            tail->next = temp;
            tail = temp;
        }
        count++;
    }

    // used for Leave and Urgent events
    T findById(int id) {
        Node* cur = head;
        while (cur != 0) {
            if (cur->data != 0 && cur->data->id == id)
                return cur->data;
            cur = cur->next;
        }
        return 0;
    }
};

#endif