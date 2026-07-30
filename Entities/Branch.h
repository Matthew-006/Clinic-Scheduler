#ifndef BRANCH_H
#define BRANCH_H

// Kerolos Sameh - Branch (Entities layer)
// doctors in array; waiting lists use Data Structures layer

#include "Doctor.h"
#include "Patient.h"
#include "../DataStructures/LinkedQueue.h"
#include "../DataStructures/PriorityQueue.h"

const int MAX_DOCTORS = 50;

class Branch {
private:
    int branchNum;
    Doctor doctors[MAX_DOCTORS];
    int doctorCount;
    LinkedQueue<Patient*> emergencyWaiting;
    PriorityQueue<Patient*> regularWaiting;

public:
    Branch() {
        branchNum = 0;
        doctorCount = 0;
    }

    Branch(int num) {
        branchNum = num;
        doctorCount = 0;
    }

    int getBranchNum() { return branchNum; }
    void setBranchNum(int n) { branchNum = n; }

    int getDoctorCount() { return doctorCount; }

    Doctor& getDoctor(int i) {
        return doctors[i];
    }

    void addDoctor(Doctor d) {
        if (doctorCount < MAX_DOCTORS) {
            doctors[doctorCount] = d;
            doctorCount++;
        }
    }

    LinkedQueue<Patient*>& getEmergencyWaiting() {
        return emergencyWaiting;
    }

    PriorityQueue<Patient*>& getRegularWaiting() {
        return regularWaiting;
    }

    int countSenior() {
        int c = 0;
        for (int i = 0; i < doctorCount; i++) {
            if (doctors[i].getSpec() == 'S')
                c++;
        }
        return c;
    }

    int countJunior() {
        int c = 0;
        for (int i = 0; i < doctorCount; i++) {
            if (doctors[i].getSpec() == 'J')
                c++;
        }
        return c;
    }
};

#endif
