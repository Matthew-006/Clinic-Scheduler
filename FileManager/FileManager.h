#ifndef FILEMANAGER_H
#define FILEMANAGER_H

// Mohamed Ayman - reads the input file

#include <string>
#include <fstream>
#include "../SimulationEngine/SimulationEngine.h"
#include "../Entities/Patient.h"
#include "../Entities/Doctor.h"
#include "../Entities/Event.h"
using namespace std;

class FileManager {
public:

    bool load(string fileName, SimulationEngine& engine) {
        ifstream file(fileName.c_str());

        int B, SU, WU, PS, PJ;
        file >> B >> SU >> WU >> PS >> PJ;
        engine.initBranches(B);

        // doctors in each branch
        int n;
        int total = 0;
        for (int i = 0; i < B; i++) {
            file >> n;
            total = total + n;
        }

        // read doctors
        for (int i = 0; i < total; i++) {
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

        // read events
        for (int i = 0; i < M; i++) {
            char t;
            file >> t;

            if (t == 'C') {
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

            if (t == 'L') {
                int TS, ID;
                file >> TS >> ID;

                Patient* p = engine.allPatients.findById(ID);
                Event* e = new Event('L', TS, ID, p);
                engine.events.enqueue(e);
            }

            if (t == 'U') {
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
};

#endif
