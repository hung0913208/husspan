#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

const std::string SEQUENCES_FILE = "sequences.csv";
const std::string UTILITIES_FILE = "utilities.csv";

class RawData {
    public:
        std::vector<std::vector<int>> sequences;
        std::vector<std::vector<float>> utilities;
        std::vector<float> sequenceUtilities;

        RawData(std::string path);
};

class Data {
    public:
        int numSequences = 0;
        int numDistinctItems = 0;
        int* distinctItems;

        Data(RawData rawData, const float THRESHOLD);
};
