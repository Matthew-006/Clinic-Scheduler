#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H

// SimulationEngine layer
// uses Scheduler, Data Structures, Entities

#include "../Entities/Branch.h"
#include "../Entities/Event.h"
#include "../Entities/Patient.h"
#include "../DataStructures/LinkedQueue.h"
#include "../DataStructures/LinkedList.h"
#include "../Scheduler/Scheduler.h"

const int MAX_BRANCHES = 20;

class SimulationEngine {
private:
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

public:
    SimulationEngine();
    ~SimulationEngine();

    void setConstants(int b, int su, int wu, int ps, int pj, int ae);
    void initBranches(int b);

    Branch& getBranch(int index);
    Branch* getBranchByNum(int num);

    LinkedQueue<Event*>& getEvents();
    LinkedList<Patient*>& getDoneList();
    LinkedList<Patient*>& getAllPatients();
    Scheduler& getScheduler();

    int getNumBranches();
    int getSetupDur();
    int getWrapUpDur();
    int getSeniorPerTest();
    int getJuniorPerTest();
    int getAutoE();
    int getTotalEmergency();
    int getTotalRegular();
    int allocateDoctorId();

    void addEmergency();
    void addRegular();

    void printSummary();
    void run();   // Checkpoint 2
};

#endif
