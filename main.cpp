// Main layer
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

    cout << "Load OK (Checkpoint 1)." << endl;
    engine.printSummary();

    // Checkpoint 2: engine.run(); and write output file
    return 0;
}
