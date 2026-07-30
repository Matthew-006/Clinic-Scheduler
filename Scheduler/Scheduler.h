#ifndef SCHEDULER_H
#define SCHEDULER_H

// Scheduler layer
// Checkpoint 1: class ready, serving logic empty
// Checkpoint 2: fill serving logic here

#include "../Entities/Branch.h"
#include "../Entities/Patient.h"
#include "../Entities/Doctor.h"

class Scheduler {
public:
    Scheduler() {}

    // Matthew: priority formula
    int calcPriority(Patient* p) {
        if (p == 0)
            return 0;
        return p->getPriority();   // CT + tests
    }

    // Checkpoint 2: Senior first, then Junior
    Doctor* findEmergencyDoctor(Branch& b, int currentTime) {
        return 0;
    }

    // Checkpoint 2: Junior first, then Senior
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
