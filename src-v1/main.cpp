#include <iostream>
#include <iomanip>
#include "utils.h"

int main(int argvc, char** argv) {

    const float THRESHOLD = std::stof(argv[1]);
    const std::string INPUT_DATA_PATH = argv[2];
    
    RawData rawData(INPUT_DATA_PATH);
    Data inputData(rawData, THRESHOLD);

    return 0;
}
