#ifndef EVENT_H
#define EVENT_H

// Kerolos Sameh - Event (Entities layer)

class Patient;

class Event {
private:
    char type;      // 'C' check-in, 'L' leave, 'U' urgent
    int time;
    int patientId;
    Patient* patient;

public:
    Event() {
        type = 'C';
        time = 0;
        patientId = 0;
        patient = 0;
    }

    Event(char ty, int t, int id, Patient* p) {
        type = ty;
        time = t;
        patientId = id;
        patient = p;
    }

    char getType() { return type; }
    int getTime() { return time; }
    int getPatientId() { return patientId; }
    Patient* getPatient() { return patient; }
    void setPatient(Patient* p) { patient = p; }
};

#endif
