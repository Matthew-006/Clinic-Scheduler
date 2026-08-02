// Main -> FileManager -> SimulationEngine -> Scheduler -> Data Structures -> Entities

#include <iostream>
#include <string>
#include "FileManager/FileManager.h"
#include "SimulationEngine/SimulationEngine.h"
using namespace std;

int main() {
    string fileName;
    cout << "Enter input file name: ";
    cin >> fileName;

    SimulationEngine engine;
    FileManager fm;

    if (!fm.load(fileName, engine)) {
        cout << "Load failed." << endl;
        return 1;
    }

    cout << "File loaded." << endl;
    return 0;
}
