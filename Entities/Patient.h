#ifndef PATIENT_H
#define PATIENT_H


class Patient {
public:
    int id;
    char type;
    int checkInTime;
    int branch;
    int numTests;
    int waitingTime;
    int visitTime;
    int finishTime;
    int status;
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

    int getPriority() {
        return (3 * checkInTime) + numTests;
    }
};

#endif
