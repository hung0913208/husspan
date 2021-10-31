#pragma once

#include <iostream>
#include <map>
#include <vector>

const std::string DATA_INFO_FILE = "info.csv";
const std::string UTILITIES_FILE = "utilities.csv";
const std::string REMAINING_UTILITIES_FILE = "remaining.csv";
const std::string SEQUENCE_UTILITIES_FILE = "sequence_utilities.csv";

class UtilitiyInfoBySequence {
    public:
        int sid;
        std::vector<std::vector<float>> utilitiesBySequence;

        UtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> utilitiesBySequence);
};

class RemainingUtilitiyInfoBySequence {
    public:
        int sid;
        std::vector<std::vector<float>> remainingUtilitiesBySequence;

        RemainingUtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> remainingUtilitiesBySequence);
};

class Data {
    public:
        
        /*
            These variables hold the dataset itself.
        */
        int num_items;
        int num_sequences;
        std::vector<int> items;
        std::vector<float> sequence_utilities;
        std::vector<UtilitiyInfoBySequence> utilities_info;
        std::vector<RemainingUtilitiyInfoBySequence> remaining_utilities_info;

        Data(std::string path);
};

class UtilityChainNode {
    public:
        int sid;
        int tid;
        float acu;
        float ru;
        UtilityChainNode* next;

        UtilityChainNode(int sid, int tid, float acu, float ru);
};

class UtilityChain {
    public:
        UtilityChainNode* head;

        UtilityChain();
        void append(UtilityChainNode* node);
};

class Pattern {
    public:
        int extension_c;
        std::string pattern;
        std::vector<UtilityChain> utilityChains;

        Pattern(int extension_c);
};
