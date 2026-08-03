#ifndef SCHEDULER_H
#define SCHEDULER_H


#include "../Entities/Branch.h"
#include "../Entities/Patient.h"
#include "../Entities/Doctor.h"

class Scheduler {
public:
    Scheduler() {}

    int calcPriority(Patient* p) {
        if (p == 0)
            return 0;
        return p->getPriority();
    }

    Doctor* findEmergencyDoctor(Branch& b, int currentTime) {
        return 0;
    }

    Doctor* findRegularDoctor(Branch& b, int currentTime) {
        return 0;
    }

    void assignPatient(Patient* p, Doctor* d, int currentTime) {
    }

    void serveBranch(Branch& b, int currentTime) {
    }
};

#endif
