#ifndef EVENT_H
#define EVENT_H


class Patient;

class Event {
public:
    char type;
    int time;
    int patientId;
    Patient* patient;

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
};

#endif
