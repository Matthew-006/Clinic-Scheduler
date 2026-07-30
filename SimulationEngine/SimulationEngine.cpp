#include "SimulationEngine.h"
#include <iostream>
using namespace std;

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
    Event* e;
    while (events.dequeue(e)) {
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
        branches[i].setBranchNum(i + 1);
    }
}

Branch& SimulationEngine::getBranch(int index) {
    return branches[index];
}

Branch* SimulationEngine::getBranchByNum(int num) {
    if (num < 1 || num > numBranches)
        return 0;
    return &branches[num - 1];
}

LinkedQueue<Event*>& SimulationEngine::getEvents() {
    return events;
}

LinkedList<Patient*>& SimulationEngine::getDoneList() {
    return doneList;
}

LinkedList<Patient*>& SimulationEngine::getAllPatients() {
    return allPatients;
}

Scheduler& SimulationEngine::getScheduler() {
    return scheduler;
}

int SimulationEngine::getNumBranches() { return numBranches; }
int SimulationEngine::getSetupDur() { return setupDur; }
int SimulationEngine::getWrapUpDur() { return wrapUpDur; }
int SimulationEngine::getSeniorPerTest() { return seniorPerTest; }
int SimulationEngine::getJuniorPerTest() { return juniorPerTest; }
int SimulationEngine::getAutoE() { return autoE; }
int SimulationEngine::getTotalEmergency() { return totalEmergency; }
int SimulationEngine::getTotalRegular() { return totalRegular; }

int SimulationEngine::allocateDoctorId() {
    return nextDoctorId++;
}

void SimulationEngine::addEmergency() { totalEmergency++; }
void SimulationEngine::addRegular() { totalRegular++; }

void SimulationEngine::printSummary() {
    cout << "===== Checkpoint 1 Load Summary =====" << endl;
    cout << "Branches: " << numBranches << endl;
    cout << "Setup=" << setupDur << " WrapUp=" << wrapUpDur
         << " SeniorPerTest=" << seniorPerTest
         << " JuniorPerTest=" << juniorPerTest << endl;
    cout << "AutoE: " << autoE << endl;

    for (int i = 0; i < numBranches; i++) {
        cout << "Branch " << branches[i].getBranchNum()
             << " doctors=" << branches[i].getDoctorCount()
             << " (S=" << branches[i].countSenior()
             << " J=" << branches[i].countJunior() << ")" << endl;

        for (int j = 0; j < branches[i].getDoctorCount(); j++) {
            Doctor& d = branches[i].getDoctor(j);
            cout << "   Doctor " << d.getId() << " " << d.getSpec()
                 << " shift=" << d.getShiftStart()
                 << " breakAfter=" << d.getBreakAfter()
                 << " breakDur=" << d.getBreakDuration() << endl;
        }
    }

    cout << "Events: " << events.size() << endl;
    cout << "Patients: " << allPatients.size() << endl;
    cout << "Done list: " << doneList.size() << " (empty until CP2)" << endl;
    cout << "=====================================" << endl;
}

void SimulationEngine::run() {
    // Checkpoint 2 - simulation loop will go here
}
