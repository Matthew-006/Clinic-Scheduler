#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

// Matthew Ayman - Priority Queue (Data Structures layer)
// sorted linked list for waiting regular patients
// priority = checkInTime + numTests  (smaller = served first)

template <class T>
class PriorityQueue {
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
    int count;

    bool better(T a, T b) {
        if (a->getPriority() != b->getPriority())
            return a->getPriority() < b->getPriority();
        return a->getId() < b->getId();
    }

public:
    PriorityQueue() {
        head = 0;
        count = 0;
    }

    ~PriorityQueue() {
        T temp;
        while (!isEmpty())
            dequeue(temp);
    }

    bool isEmpty() {
        return head == 0;
    }

    int size() {
        return count;
    }

    // insert in sorted order - O(n)
    void enqueue(T val) {
        Node* n = new Node(val);
        if (head == 0 || better(val, head->item)) {
            n->next = head;
            head = n;
        }
        else {
            Node* cur = head;
            while (cur->next != 0 && !better(val, cur->next->item))
                cur = cur->next;
            n->next = cur->next;
            cur->next = n;
        }
        count++;
    }

    bool dequeue(T& val) {
        if (isEmpty())
            return false;
        Node* temp = head;
        val = head->item;
        head = head->next;
        delete temp;
        count--;
        return true;
    }

    bool peek(T& val) {
        if (isEmpty())
            return false;
        val = head->item;
        return true;
    }

    bool removeById(int id, T& val) {
        Node* prev = 0;
        Node* cur = head;
        while (cur != 0) {
            if (cur->item != 0 && cur->item->getId() == id) {
                val = cur->item;
                if (prev == 0)
                    head = cur->next;
                else
                    prev->next = cur->next;
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
