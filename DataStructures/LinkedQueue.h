#ifndef LINKEDQUEUE_H
#define LINKEDQUEUE_H

// Hassan Yehia - Linked Queue (Data Structures layer)
// used for: events list, waiting emergency patients

template <class T>
class LinkedQueue {
private:
    struct Node {
        T item;
        Node* next;
        Node(T val) {
            item = val;
            next = 0;
        }
    };
    Node* front;
    Node* rear;
    int count;

public:
    LinkedQueue() {
        front = 0;
        rear = 0;
        count = 0;
    }

    ~LinkedQueue() {
        T temp;
        while (!isEmpty())
            dequeue(temp);
    }

    bool isEmpty() {
        return front == 0;
    }

    int size() {
        return count;
    }

    void enqueue(T val) {
        Node* n = new Node(val);
        if (isEmpty()) {
            front = n;
            rear = n;
        }
        else {
            rear->next = n;
            rear = n;
        }
        count++;
    }

    bool dequeue(T& val) {
        if (isEmpty())
            return false;
        Node* temp = front;
        val = front->item;
        front = front->next;
        if (front == 0)
            rear = 0;
        delete temp;
        count--;
        return true;
    }

    bool peek(T& val) {
        if (isEmpty())
            return false;
        val = front->item;
        return true;
    }

    // remove a patient by id (for Leave event) - O(n)
    bool removeById(int id, T& val) {
        Node* prev = 0;
        Node* cur = front;
        while (cur != 0) {
            if (cur->item != 0 && cur->item->getId() == id) {
                val = cur->item;
                if (prev == 0)
                    front = cur->next;
                else
                    prev->next = cur->next;
                if (cur == rear)
                    rear = prev;
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }
};

#endif
