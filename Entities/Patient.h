#ifndef PATIENT_H
#define PATIENT_H

// Kerolos Sameh - Patient (Entities layer)

class Patient {
private:
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

public:
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

    int getId() { return id; }
    char getType() { return type; }
    int getCheckInTime() { return checkInTime; }
    int getBranch() { return branch; }
    int getNumTests() { return numTests; }
    int getWaitingTime() { return waitingTime; }
    int getVisitTime() { return visitTime; }
    int getFinishTime() { return finishTime; }
    int getStatus() { return status; }
    bool getAutoEscalated() { return autoEscalated; }

    void setType(char t) { type = t; }
    void setWaitingTime(int w) { waitingTime = w; }
    void setVisitTime(int v) { visitTime = v; }
    void setFinishTime(int f) { finishTime = f; }
    void setStatus(int s) { status = s; }
    void setAutoEscalated(bool a) { autoEscalated = a; }

    // priority for regular patients = check-in time + number of tests
    int getPriority() {
        return checkInTime + numTests;
    }
};

#endif
