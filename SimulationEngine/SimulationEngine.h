#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H

// SimulationEngine layer

#include "../Entities/Branch.h"
#include "../Entities/Event.h"
#include "../Entities/Patient.h"
#include "../DataStructures/LinkedQueue.h"
#include "../DataStructures/LinkedList.h"
#include "../Scheduler/Scheduler.h"

const int MAX_BRANCHES = 20;

class SimulationEngine {
public:
    int numBranches;
    int setupDur;
    int wrapUpDur;
    int seniorPerTest;
    int juniorPerTest;
    int autoE;

    Branch branches[MAX_BRANCHES];
    LinkedQueue<Event*> events;
    LinkedList<Patient*> doneList;
    LinkedList<Patient*> allPatients;
    Scheduler scheduler;

    int currentTime;
    int nextDoctorId;
    int totalEmergency;
    int totalRegular;

    SimulationEngine();
    ~SimulationEngine();

    void setConstants(int b, int su, int wu, int ps, int pj, int ae);
    void initBranches(int b);
    Branch* getBranchByNum(int num);
    int allocateDoctorId();

    void run();   // Checkpoint 2
};

#endif
