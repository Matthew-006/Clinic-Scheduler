#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

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

    bool better(T a, T b) const {
        if (a->getPriority() != b->getPriority())
            return a->getPriority() < b->getPriority();
        return a->id < b->id;
    }

public:
    PriorityQueue() {
        head = 0;
        count = 0;
    }

    ~PriorityQueue() {
        while (!isEmpty())
            dequeue();
    }

    bool isEmpty() const {
        return head == 0;
    }

    int size() const {
        return count;
    }

    void insert(T val) {
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

    T extractBest() {
        if (head == 0)
            return T();

        Node* temp = head;
        T val = head->item;
        head = head->next;
        delete temp;
        count--;
        return val;
    }

    void enqueue(T val) {
        insert(val);
    }

    T dequeue() {
        return extractBest();
    }

    T peek() const {
        if (head == 0)
            return T();
        return head->item;
    }

    template <class Visitor>
    void forEach(Visitor visit) {
        Node* cur = head;
        while (cur != 0) {
            visit(cur->item);
            cur = cur->next;
        }
    }

    bool removeById(int id, T& val) {
        Node* prev = 0;
        Node* cur = head;
        while (cur != 0) {
            if (cur->item != 0 && cur->item->id == id) {
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
