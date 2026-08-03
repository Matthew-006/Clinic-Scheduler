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

void SimulationEngine::run() {}
