#ifndef LINKEDLIST_H
#define LINKEDLIST_H


template <class T>

class LinkedList 
{
private:
    struct Node 
    {
        T data;
        Node* next;
    };
    Node* head;
    Node* tail;
    int count;

public:
    LinkedList() 
    {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    ~LinkedList() 
    {
        Node*cur = head;
        while (cur!= nullptr)
        {
            Node*temp = cur;
            cur=cur->next;
            delete temp;
        }
    }

    bool isEmpty() 
    {
        return head == nullptr;
    }

    int getCount() 
    {
        return count;
    }

    void insertEnd(T value) 
    {
        Node*temp = new Node();
        temp->data = value;
        temp->next = nullptr;

        if (head == nullptr) {
            head = temp;
            tail = temp;
        }
        else {
            tail->next = temp;
            tail = temp;
        }
        count++;
    }

    T findById(int id) {
        Node*cur = head;
        while (cur!= nullptr) {
            if (cur->data!= nullptr && cur->data->id ==id)
                return cur->data;
            cur=cur->next;
        }
        return nullptr;
    }
};

#endif