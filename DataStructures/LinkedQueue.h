#ifndef LINKEDQUEUE_H
#define LINKEDQUEUE_H

#include <stdexcept>

// FIFO queue used for time-ordered events and emergency waiting patients.
// The queue owns its nodes, but never the stored values (for example, Patient*).
template <class T>
class LinkedQueue {
private:
    struct Node {
        T item;
        Node* next;

        Node(const T& value) : item(value), next(0) {}
    };

    Node* front;
    Node* rear;
    int count;

public:
    LinkedQueue() : front(0), rear(0), count(0) {}

    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;

    ~LinkedQueue() {
        clear();
    }

    bool isEmpty() const {
        return front == 0;
    }

    int size() const {
        return count;
    }

    void enqueue(const T& value) {
        Node* node = new Node(value);
        if (isEmpty()) {
            front = rear = node;
        }
        else {
            rear->next = node;
            rear = node;
        }
        ++count;
    }

    T dequeue() {
        if (isEmpty())
            throw std::underflow_error("Cannot dequeue from an empty queue");

        Node* oldFront = front;
        T value = oldFront->item;
        front = front->next;
        if (front == 0)
            rear = 0;

        delete oldFront;
        --count;
        return value;
    }

    T peek() const {
        if (isEmpty())
            throw std::underflow_error("Cannot peek at an empty queue");
        return front->item;
    }

    // Inserts using a caller-provided ordering rule. Equal values keep their
    // existing FIFO order. For emergency patients, compare checkInTime.
    template <class ComesBefore>
    void insertInOrder(const T& value, ComesBefore comesBefore) {
        Node* node = new Node(value);

        if (isEmpty() || comesBefore(value, front->item)) {
            node->next = front;
            front = node;
            if (rear == 0)
                rear = node;
        }
        else {
            Node* current = front;
            while (current->next != 0 &&
                   !comesBefore(value, current->next->item)) {
                current = current->next;
            }
            node->next = current->next;
            current->next = node;
            if (node->next == 0)
                rear = node;
        }
        ++count;
    }

    // Used by Phase 2 when a regular patient is escalated to emergency.
    // Example comparator: [](Patient* a, Patient* b) {
    //     return a->checkInTime < b->checkInTime;
    // }
    template <class ComesBefore>
    void enqueueEscalated(const T& value, ComesBefore comesBefore) {
        insertInOrder(value, comesBefore);
    }

    // Supports removing waiting patients for Leave events.
    bool removeById(int id, T& value) {
        Node* previous = 0;
        Node* current = front;

        while (current != 0) {
            if (current->item != 0 && current->item->id == id) {
                value = current->item;
                if (previous == 0)
                    front = current->next;
                else
                    previous->next = current->next;
                if (current == rear)
                    rear = previous;

                delete current;
                --count;
                return true;
            }
            previous = current;
            current = current->next;
        }
        return false;
    }

    void clear() {
        while (!isEmpty())
            dequeue();
    }
};

#endif
