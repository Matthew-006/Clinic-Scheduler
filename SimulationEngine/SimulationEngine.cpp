#include "SimulationEngine.h"

SimulationEngine::SimulationEngine() {
    numBranches = 0;
    setupDur = 0;
    wrapUpDur = 0;
    seniorPerTest = 0;
    juniorPerTest = 0;
    autoE = 0;
    currentTime = 0;
    nextDoctorId = 1;
    totalEmergency = 0;
    totalRegular = 0;
    totalAutoEscalated = 0;
    transferMode = false;
}

SimulationEngine::~SimulationEngine() {
    while (!events.isEmpty()) {
        Event* e = events.dequeue();
        delete e;
    }
}

void SimulationEngine::setConstants(int b, int su, int wu, int ps, int pj, int ae) {
    numBranches = b;
    setupDur = su;
    wrapUpDur = wu;
    seniorPerTest = ps;
    juniorPerTest = pj;
    autoE = ae;
}

void SimulationEngine::initBranches(int b) {
    numBranches = b;
    for (int i = 0; i < b; i++) {
        branches[i].branchNum = i + 1;
        branches[i].doctorCount = 0;
    }
}

Branch* SimulationEngine::getBranchByNum(int num) {
    if (num < 1 || num > numBranches)
        return 0;
    return &branches[num - 1];
}

int SimulationEngine::allocateDoctorId() {
    return nextDoctorId++;
}

void SimulationEngine::run() {
    while (runOneTimeStep()) {}
}

bool SimulationEngine::runOneTimeStep() {
    if (isComplete())
        return false;

    processEventsAtCurrentTime();
    finishVisits();
    autoEscalateWaitingPatients();
    serveBranches();
    transferPatients();
    currentTime++;
    return true;
}

bool SimulationEngine::isComplete() {
    return events.isEmpty() && !hasActivePatients();
}

void SimulationEngine::processEventsAtCurrentTime() {
    while (!events.isEmpty() && events.peek()->time == currentTime) {
        Event* e = events.dequeue();
        processEvent(e);
        delete e;
    }
}

void SimulationEngine::processEvent(Event* e) {
    if (e == 0)
        return;

    if (e->type == 'C')
        processCheckIn(e);
    else if (e->type == 'U')
        processUrgent(e);
    else if (e->type == 'L')
        processLeave(e);
}

void SimulationEngine::processCheckIn(Event* e) {
    if (e == 0 || e->patient == 0)
        return;

    Branch* b = getBranchByNum(e->patient->branch);
    if (b == 0)
        return;

    if (e->patient->type == 'E')
        b->emergencyWaiting.enqueue(e->patient);
    else if (e->patient->type == 'R')
        b->regularWaiting.insert(e->patient);
    e->patient->status = 1;
}

void SimulationEngine::processUrgent(Event* e) {
    if (e == 0 || e->patient == 0)
        return;

    Branch* b = getBranchByNum(e->patient->branch);
    if (b == 0)
        return;

    Patient* patient = 0;
    if (b->regularWaiting.removeById(e->patientId, patient)) {
        patient->type = 'E';
        b->emergencyWaiting.insertByCheckInTime(patient);
    }
}

void SimulationEngine::processLeave(Event* e) {
    if (e == 0 || e->patient == 0)
        return;

    Branch* b = getBranchByNum(e->patient->branch);
    if (b == 0)
        return;

    Patient* patient = 0;
    if (b->emergencyWaiting.removeById(e->patientId, patient) ||
        b->regularWaiting.removeById(e->patientId, patient))
        patient->status = 3;
}

void SimulationEngine::autoEscalateWaitingPatients() {
    if (autoE < 0)
        return;

    for (int i = 0; i < numBranches; i++) {
        Branch& b = branches[i];
        int waitingCount = b.regularWaiting.size();
        for (int checked = 0; checked < waitingCount; checked++) {
            Patient* p = b.regularWaiting.extractBest();
            if (p == 0)
                break;
            if (currentTime - p->checkInTime >= autoE) {
                p->autoEscalated = true;
                p->type = 'E';
                b.emergencyWaiting.insertByCheckInTime(p);
                totalAutoEscalated++;
            }
            else {
                b.regularWaiting.insert(p);
            }
        }
    }
}

void SimulationEngine::transferPatients() {
    if (!transferMode)
        return;

    const int TRANSFER_LIMIT = 5;

    for (int i = 0; i < numBranches; i++) {
        Branch& b = branches[i];

        int regularCount = b.regularWaiting.size();
        for (int checked = 0; checked < regularCount; checked++) {
            Patient* p = b.regularWaiting.extractBest();
            if (p == 0)
                break;

            if (currentTime - p->checkInTime > TRANSFER_LIMIT) {
                int bestBranch = -1;
                int bestDistance = 999999;
                for (int j = 0; j < numBranches; j++) {
                    if (j == i)
                        continue;

                    Doctor* d = scheduler.findRegularDoctor(branches[j], currentTime);
                    int distance = distTable.getDistance(i, j);
                    if (d != 0 && distance < bestDistance) {
                        bestDistance = distance;
                        bestBranch = j;
                    }
                }

                if (bestBranch != -1) {
                    p->transferDelay += bestDistance;
                    p->branch = branches[bestBranch].branchNum;
                    branches[bestBranch].regularWaiting.insert(p);
                    continue;
                }
            }

            b.regularWaiting.insert(p);
        }

        int emergencyCount = b.emergencyWaiting.size();
        for (int checked = 0; checked < emergencyCount; checked++) {
            Patient* p = b.emergencyWaiting.dequeue();

            if (currentTime - p->checkInTime > TRANSFER_LIMIT) {
                int bestBranch = -1;
                int bestDistance = 999999;
                for (int j = 0; j < numBranches; j++) {
                    if (j == i)
                        continue;

                    Doctor* d = scheduler.findEmergencyDoctor(branches[j], currentTime);
                    int distance = distTable.getDistance(i, j);
                    if (d != 0 && distance < bestDistance) {
                        bestDistance = distance;
                        bestBranch = j;
                    }
                }

                if (bestBranch != -1) {
                    p->transferDelay += bestDistance;
                    p->branch = branches[bestBranch].branchNum;
                    branches[bestBranch].emergencyWaiting.insertByCheckInTime(p);
                    continue;
                }
            }

            b.emergencyWaiting.insertByCheckInTime(p);
        }
    }
}

void SimulationEngine::finishVisits() {
    for (int i = 0; i < numBranches; i++)
        scheduler.finishVisits(branches[i], currentTime, doneList);
}

void SimulationEngine::serveBranches() {
    for (int i = 0; i < numBranches; i++) {
        scheduler.serveBranch(branches[i], currentTime, setupDur, wrapUpDur,
                              seniorPerTest, juniorPerTest);
    }
}

bool SimulationEngine::hasActivePatients() {
    for (int i = 0; i < numBranches; i++) {
        Branch& b = branches[i];
        if (!b.emergencyWaiting.isEmpty() || !b.regularWaiting.isEmpty())
            return true;
        for (int j = 0; j < b.doctorCount; j++) {
            if (b.doctors[j].currentPatient != 0)
                return true;
        }
    }
    return false;
}
