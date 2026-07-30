#include "FileManager.h"
#include "../Entities/Patient.h"
#include "../Entities/Doctor.h"
#include "../Entities/Event.h"
#include <fstream>
#include <iostream>
using namespace std;

bool FileManager::load(string fileName, SimulationEngine& engine) {
    ifstream in(fileName.c_str());
    if (!in.is_open()) {
        cout << "Cannot open file: " << fileName << endl;
        return false;
    }

    int B, SU, WU, PS, PJ;
    in >> B >> SU >> WU >> PS >> PJ;

    engine.initBranches(B);

    int docCount[20];
    int totalDocs = 0;
    for (int i = 0; i < B; i++) {
        in >> docCount[i];
        totalDocs += docCount[i];
    }

    for (int i = 0; i < totalDocs; i++) {
        int BR, SH, BA, BD;
        char SP;
        in >> BR >> SP >> SH >> BA >> BD;

        Branch* br = engine.getBranchByNum(BR);
        if (br == 0) {
            cout << "Bad branch number for doctor" << endl;
            return false;
        }
        Doctor d(engine.allocateDoctorId(), BR, SP, SH, BA, BD);
        br->addDoctor(d);
    }

    int AutoE;
    in >> AutoE;
    engine.setConstants(B, SU, WU, PS, PJ, AutoE);

    int M;
    in >> M;

    for (int i = 0; i < M; i++) {
        char code;
        in >> code;

        if (code == 'C') {
            char TYP;
            int TS, ID, BR, TESTS;
            in >> TYP >> TS >> ID >> BR >> TESTS;

            Patient* p = new Patient(ID, TYP, TS, BR, TESTS);
            engine.getAllPatients().insertEnd(p);

            Event* e = new Event('C', TS, ID, p);
            engine.getEvents().enqueue(e);

            if (TYP == 'E')
                engine.addEmergency();
            else
                engine.addRegular();
        }
        else if (code == 'L') {
            int TS, ID;
            in >> TS >> ID;

            Patient* p = 0;
            engine.getAllPatients().findById(ID, p);

            Event* e = new Event('L', TS, ID, p);
            engine.getEvents().enqueue(e);
        }
        else if (code == 'U') {
            int TS, ID;
            in >> TS >> ID;

            Patient* p = 0;
            engine.getAllPatients().findById(ID, p);

            Event* e = new Event('U', TS, ID, p);
            engine.getEvents().enqueue(e);
        }
        else {
            cout << "Unknown event: " << code << endl;
            return false;
        }
    }

    in.close();
    return true;
}
