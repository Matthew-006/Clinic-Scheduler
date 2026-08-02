#ifndef PATIENT_H
#define PATIENT_H

// Kerolos Sameh - Patient (Entities layer)

class Patient {
public:
    int id;
    char type;          // 'E' or 'R'
    int checkInTime;
    int branch;
    int numTests;
    int waitingTime;
    int visitTime;
    int finishTime;
    int status;         // 0 waiting, 1 in visit, 2 done, 3 left
    bool autoEscalated;

    Patient() {
        id = 0;
        type = 'R';
        checkInTime = 0;
        branch = 0;
        numTests = 0;
        waitingTime = 0;
        visitTime = 0;
        finishTime = 0;
        status = 0;
        autoEscalated = false;
    }

    Patient(int i, char t, int ct, int br, int tests) {
        id = i;
        type = t;
        checkInTime = ct;
        branch = br;
        numTests = tests;
        waitingTime = 0;
        visitTime = 0;
        finishTime = 0;
        status = 0;
        autoEscalated = false;
    }

    // smaller priority = served sooner
    int getPriority() {
        return checkInTime + numTests;
    }
};

#endif
