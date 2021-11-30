#pragma once
#include <string>
#include <vector>
#include <unordered_map>
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

class UtilitiyInfoBySequence {
    public:
        int sid;
        std::vector<std::vector<float>> utilitiesBySequence;
        UtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> utilitiesBySequence);
};

class RemainingUtilitiyInfoBySequence {
    public:
        int sid;
        std::vector<std::vector<float>> remUtilitiesBySequence;
        RemainingUtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> remUtilitiesBySequence);
};

class Data {
    public:
        int numSequences = 0;
        int numDistinctItems = 0;
        std::vector<int> distinctItems;
        std::vector<UtilitiyInfoBySequence*> utilitiesData;
        std::vector<RemainingUtilitiyInfoBySequence*> remUtilitiesData;

        Data(RawData rawData, const float THRESHOLD);
};

class UtilityChainNode {
    public:
        int sid;
        int tid;
        float acu;
        float ru;
        UtilityChainNode(int sid, int tid, float acu, float ru);
};

class UtilityChain {
    public:
        float seqPEU;
        std::list<UtilityChainNode*> chainNodes;
};

class Pattern {
    public:
        int extension_c;
        float utility;
        float peu;
        std::string pattern;
        std::vector<UtilityChain> utilityChains;

        Pattern(const int extension_c);
};
