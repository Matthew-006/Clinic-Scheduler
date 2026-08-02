#ifndef LINKEDQUEUE_H
#define LINKEDQUEUE_H

// Hassan Yehia - Linked Queue (Data Structures layer)
// used for events and emergency waiting patients

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
        while (!isEmpty())
            dequeue();
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

    T dequeue() {
        Node* temp = front;
        T val = front->item;
        front = front->next;
        if (front == 0)
            rear = 0;
        delete temp;
        count--;
        return val;
    }

    T peek() {
        return front->item;
    }

    // needed for Leave events
    bool removeById(int id, T& val) {
        Node* prev = 0;
        Node* cur = front;
        while (cur != 0) {
            if (cur->item != 0 && cur->item->id == id) {
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
