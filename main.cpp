#include <iostream>
#include <string>
#include "FileManager/FileManager.h"
#include "SimulationEngine/SimulationEngine.h"
using namespace std;

int main(int argc, char* argv[]) {
    string fileName;
    cout << "Enter input file name: ";
    cin >> fileName;

    SimulationEngine engine;
    FileManager fm;

    if (!fm.load(fileName, engine)) {
        cout << "Error: could not open \"" << fileName << "\"" << endl;
        return 1;
    }

    cout << "File loaded: " << fileName << endl;
    engine.run();

    if (!fm.writeOutput(engine, "output.txt")) {
        cout << "Error: could not write output.txt" << endl;
        return 1;
    }

    cout << "Output written to output.txt" << endl;
    return 0;
}