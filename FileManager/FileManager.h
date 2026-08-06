#ifndef FILEMANAGER_H
#define FILEMANAGER_H


#include <string>
#include <fstream>

#include "../SimulationEngine/SimulationEngine.h"
#include "../Entities/Patient.h"
#include "../Entities/Doctor.h"
#include "../Entities/Event.h"

using namespace std;

class FileManager
{
public:
    bool load(string fileName, SimulationEngine& engine)
    {
        ifstream file(fileName);
        if (!file)
            return false;

        int B, SU, WU, PS, PJ;
        file >> B >> SU >> WU >> PS >> PJ;
        engine.initBranches(B);

        int n;
        int total = 0;
        for (int i = 0; i < B; i++)
        {
            file >> n;
            total += n;
        }

        for (int i = 0; i < total; i++)
        {
            int BR, SH, BA, BD;
            char SP;
            file >> BR >> SP >> SH >> BA >> BD;
            Branch* b = engine.getBranchByNum(BR);
            int id = engine.allocateDoctorId();
            Doctor d(id, BR, SP, SH, BA, BD);
            b->addDoctor(d);
        }

        int AutoE;
        file >> AutoE;
        engine.setConstants(B, SU, WU, PS, PJ, AutoE);
        int M;
        file >> M;
        for (int i = 0; i < M; i++)
        {
            char t;
            file >> t;
            if (t == 'C')
            {
                char TYP;
                int TS, ID, BR, TESTS;
                file >> TYP >> TS >> ID >> BR >> TESTS;
                Patient* p = new Patient(ID, TYP, TS, BR, TESTS);
                engine.allPatients.insertEnd(p);
                Event* e = new Event('C', TS, ID, p);
                engine.events.enqueue(e);
                if (TYP == 'E')
                    engine.totalEmergency = engine.totalEmergency + 1;
                else
                    engine.totalRegular = engine.totalRegular + 1;
            }
            if (t == 'L')
            {
                int TS, ID;
                file >> TS >> ID;
                Patient* p = engine.allPatients.findById(ID);
                Event* e = new Event('L', TS, ID, p);
                engine.events.enqueue(e);
            }
            if (t == 'U')
            {
                int TS, ID;
                file >> TS >> ID;
                Patient* p = engine.allPatients.findById(ID);
                Event* e = new Event('U', TS, ID, p);
                engine.events.enqueue(e);
            }
        }

        file.close();
        return true;
    }

    void writeOutput(SimulationEngine& engine, string outName)
    {
        Patient* arr[1000];
        int n;
        engine.doneList.toArray(arr, n);

        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - 1 - i; j++) {
                bool doSwap = false;
                if (arr[j]->finishTime > arr[j + 1]->finishTime) doSwap = true;
                else if (arr[j]->finishTime == arr[j + 1]->finishTime && arr[j]->numTests > arr[j + 1]->numTests) doSwap = true;
                else if (arr[j]->finishTime == arr[j + 1]->finishTime && arr[j]->numTests == arr[j + 1]->numTests && arr[j]->id > arr[j + 1]->id) doSwap = true;

                if (doSwap) {
                    Patient* temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }

        ofstream out(outName);

        int totalWait = 0, totalVisit = 0;
        for (int i = 0; i < n; i++) {
            Patient* p = arr[i];
            out << p->finishTime << " " << p->id << " " << p->checkInTime << " " << p->waitingTime << " " << p->visitTime << endl;
            totalWait = totalWait + p->waitingTime;
            totalVisit = totalVisit + p->visitTime;
        }

        out << endl;
        out << "Patients: " << engine.allPatients.getCount()
            << " [Emergency: " << engine.totalEmergency << ", Regular: " << engine.totalRegular << "]" << endl;
        out << "Branches: " << engine.numBranches << endl;

        for (int i = 0; i < engine.numBranches; i++) {
            Branch* b = &engine.branches[i];
            int sen = 0, jun = 0;
            for (int d = 0; d < b->doctorCount; d++) {
                if (b->doctors[d].spec == 'S')
                    sen++;
                else
                    jun++;
            }
            out << "Branch " << b->branchNum << ": Doctors: " << b->doctorCount
                << " [Senior: " << sen << ", Junior: " << jun << "]" << endl;
        }

        if (n > 0)
            out << "Avg Wait = " << (double)totalWait / n << ", Avg Visit = " << (double)totalVisit / n << endl;

        if (engine.totalRegular > 0)
            out << "Auto-escalated: " << (100.0 * engine.totalAutoEscalated / engine.totalRegular) << "%" << endl;
    }
};
#endif