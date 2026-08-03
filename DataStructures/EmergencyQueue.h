#ifndef EMERGENCYQUEUE_H
#define EMERGENCYQUEUE_H

#include "LinkedQueue.h"

template <class T>
class EmergencyQueue : public LinkedQueue<T>
{
public:

    void insertByCheckInTime(T val)
    {
        typename LinkedQueue<T>::Node* n =
            new typename LinkedQueue<T>::Node(val);

        if (this->isEmpty())
        {
            this->front = this->rear = n;
            this->count++;
            return;
        }

        
        if (val->checkInTime < this->front->item->checkInTime)
        {
            n->next = this->front;
            this->front = n;
            this->count++;
            return;
        }

        typename LinkedQueue<T>::Node* prev = this->front;
        typename LinkedQueue<T>::Node* cur = this->front->next;

        while (cur != nullptr &&
               cur->item->checkInTime <= val->checkInTime)
        {
            prev = cur;
            cur = cur->next;
        }

        prev->next = n;
        n->next = cur;

        if (cur == nullptr)
            this->rear = n;

        this->count++;
    }

    bool removeById(int id, T& val)
    {
        typename LinkedQueue<T>::Node* prev = nullptr;
        typename LinkedQueue<T>::Node* cur = this->front;

        while (cur != nullptr)
        {
            if (cur->item != nullptr && cur->item->id == id)
            {
                val = cur->item;

                if (prev == nullptr)
                    this->front = cur->next;
                else
                    prev->next = cur->next;

                if (cur == this->rear)
                    this->rear = prev;

                delete cur;
                this->count--;

                return true;
            }

            prev = cur;
            cur = cur->next;
        }

        return false;
    }
};

#endif
