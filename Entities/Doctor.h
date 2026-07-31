#ifndef DOCTOR_H
#define DOCTOR_H

// Kerolos Sameh - Doctor (Entities layer)

class Patient;

class Doctor {
public:
    int id;
    int branch;
    char spec;          // 'S' or 'J'
    int shiftStart;
    int breakAfter;
    int breakDuration;
    int patientsSinceBreak;
    int busyUntil;
    int breakUntil;
    int status;         // 0 not started, 1 free, 2 busy, 3 on break
    Patient* currentPatient;

    Doctor() {
        id = 0;
        branch = 0;
        spec = 'J';
        shiftStart = 0;
        breakAfter = 0;
        breakDuration = 0;
        patientsSinceBreak = 0;
        busyUntil = -1;
        breakUntil = -1;
        status = 0;
        currentPatient = 0;
    }

    Doctor(int i, int br, char s, int sh, int ba, int bd) {
        id = i;
        branch = br;
        spec = s;
        shiftStart = sh;
        breakAfter = ba;
        breakDuration = bd;
        patientsSinceBreak = 0;
        busyUntil = -1;
        breakUntil = -1;
        status = 0;
        currentPatient = 0;
    }
};

#endif
