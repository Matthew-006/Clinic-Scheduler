#ifndef LINKEDQUEUE_H
#define LINKEDQUEUE_H



template <class T>
class LinkedQueue {
protected:
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

   
};

#endif
