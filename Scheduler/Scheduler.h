#ifndef SCHEDULER_H
#define SCHEDULER_H

// Scheduler layer - empty in Checkpoint 1

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

    // Checkpoint 2
    Doctor* findEmergencyDoctor(Branch& b, int currentTime) {
        return 0;
    }

    // Checkpoint 2
    Doctor* findRegularDoctor(Branch& b, int currentTime) {
        return 0;
    }

    // Checkpoint 2
    void assignPatient(Patient* p, Doctor* d, int currentTime) {
    }

    // Checkpoint 2
    void serveBranch(Branch& b, int currentTime) {
    }
};

#endif
