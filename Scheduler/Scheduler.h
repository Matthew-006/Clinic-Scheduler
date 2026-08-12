#ifndef SCHEDULER_H
#define SCHEDULER_H


#include "../Entities/Branch.h"
#include "../Entities/Patient.h"
#include "../Entities/Doctor.h"
#include "../DataStructures/LinkedList.h"

class Scheduler {
public:
    Scheduler() {}

    int calcPriority(Patient* p) {
        if (p == 0)
            return 0;
        return p->getPriority();
    }

    bool isAvailable(Doctor& d, int currentTime) {
        if (currentTime < d.shiftStart || currentTime < d.breakUntil ||
            currentTime < d.busyUntil)
            return false;

        return d.currentPatient == 0;
    }

    Doctor* findEmergencyDoctor(Branch& b, int currentTime) {
        Doctor* fallback = 0;
        for (int i = 0; i < b.doctorCount; i++) {
            Doctor& d = b.doctors[i];
            if (!isAvailable(d, currentTime))
                continue;
            if (d.spec == 'S')
                return &d;
            if (fallback == 0)
                fallback = &d;
        }
        return fallback;
    }

    Doctor* findRegularDoctor(Branch& b, int currentTime) {
        Doctor* fallback = 0;
        for (int i = 0; i < b.doctorCount; i++) {
            Doctor& d = b.doctors[i];
            if (!isAvailable(d, currentTime))
                continue;
            if (d.spec == 'J')
                return &d;
            if (fallback == 0)
                fallback = &d;
        }
        return fallback;
    }

    void finishVisits(Branch& b, int currentTime, LinkedList<Patient*>& doneList) {
        for (int i = 0; i < b.doctorCount; i++) {
            Doctor& d = b.doctors[i];

            if (d.currentPatient != 0 && currentTime >= d.busyUntil) {
                Patient* p = d.currentPatient;
                p->finishTime = p->checkInTime + p->waitingTime + p->visitTime;
                p->status = 4;
                doneList.insertEnd(p);
                d.currentPatient = 0;
                d.patientsSinceBreak++;

                if (d.breakAfter > 0 && d.patientsSinceBreak >= d.breakAfter) {
                    d.patientsSinceBreak = 0;
                    d.breakUntil = currentTime + d.breakDuration;
                    d.status = 3;
                }
                else {
                    d.status = 1;
                }
            }

            if (d.currentPatient == 0 && currentTime >= d.shiftStart &&
                currentTime >= d.breakUntil && currentTime >= d.busyUntil) {
                d.status = 1;
            }
        }
    }

    void assignPatient(Patient* p, Doctor* d, int currentTime,
                       int setupDuration, int wrapUpDuration,
                       int seniorPerTest, int juniorPerTest) {
        if (p == 0 || d == 0)
            return;

        const int perTest = (d->spec == 'S') ? seniorPerTest : juniorPerTest;
        p->waitingTime = currentTime - p->checkInTime + p->transferDelay;
        p->visitTime = setupDuration + (p->numTests * perTest) + wrapUpDuration;
        if (p->visitTime < 1)
            p->visitTime = 1;
        p->status = 2;
        d->currentPatient = p;
        d->busyUntil = currentTime + p->visitTime;
        d->status = 2;
    }

    void serveBranch(Branch& b, int currentTime, int setupDuration,
                     int wrapUpDuration, int seniorPerTest, int juniorPerTest) {
        while (!b.emergencyWaiting.isEmpty()) {
            Doctor* d = findEmergencyDoctor(b, currentTime);
            if (d == 0)
                break;
            Patient* p = b.emergencyWaiting.dequeue();
            assignPatient(p, d, currentTime, setupDuration, wrapUpDuration,
                          seniorPerTest, juniorPerTest);
        }

        while (!b.regularWaiting.isEmpty()) {
            Doctor* d = findRegularDoctor(b, currentTime);
            if (d == 0)
                break;
            Patient* p = b.regularWaiting.dequeue();
            assignPatient(p, d, currentTime, setupDuration, wrapUpDuration,
                          seniorPerTest, juniorPerTest);
        }
    }

    void serveBranch(Branch& b, int currentTime) {
        serveBranch(b, currentTime, 0, 0, 0, 0);
    }

    void assignPatient(Patient* p, Doctor* d, int currentTime) {
        assignPatient(p, d, currentTime, 0, 0, 0, 0);
    }
};

#endif
