#ifndef FILEMANAGER_H
#define FILEMANAGER_H

// Kerolos Sameh - FileManager layer (reads input file)

#include <string>
#include "../SimulationEngine/SimulationEngine.h"

class FileManager {
public:
    bool load(std::string fileName, SimulationEngine& engine);
};

#endif
