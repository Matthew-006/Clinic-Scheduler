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
        cout << "Not found" << endl;
    }
    else {
        cout << "File loaded" << endl;
    }

    return 0;
}