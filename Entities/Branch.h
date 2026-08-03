#ifndef BRANCH_H
#define BRANCH_H


#include "Doctor.h"
#include "Patient.h"
#include "../DataStructures/LinkedQueue.h"
#include "../DataStructures/PriorityQueue.h"

const int MAX_DOCTORS = 50;

class Branch {
public:
    int branchNum;
    Doctor doctors[MAX_DOCTORS];
    int doctorCount;
    LinkedQueue<Patient*> emergencyWaiting;
    PriorityQueue<Patient*> regularWaiting;

    Branch() {
        branchNum = 0;
        doctorCount = 0;
    }

    Branch(int num) {
        branchNum = num;
        doctorCount = 0;
    }

    void addDoctor(Doctor d) {
        if (doctorCount < MAX_DOCTORS) {
            doctors[doctorCount] = d;
            doctorCount++;
        }
    }
};

#endif
