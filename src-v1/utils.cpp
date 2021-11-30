#include "utils.h"
#include <iostream>
#include <numeric>

RawData::RawData(std::string inputDataPath) {
    
    std::fstream sequencesFile (inputDataPath + "/" + SEQUENCES_FILE);
    std::fstream utilitiesFile (inputDataPath + "/" + UTILITIES_FILE);

    std::string line;
    std::vector<std::string> tokens;

    while (std::getline(sequencesFile, line)) {

        std::vector<int> sequence;
        boost::split(tokens, line, boost::is_any_of("\t"));
        for (int i = 0; i < tokens.size(); i++) {
            int item = std::stoi(tokens[i]);
            sequence.push_back(item);
        }
        sequences.push_back(sequence);

    }

    while (std::getline(utilitiesFile, line)) {
        
        float sequenceUtility = 0;
        std::vector<float> itemUtilities;
        boost::split(tokens, line, boost::is_any_of("\t"));
        for (auto const& token : tokens) {
            float itemUtility = std::stof(token);
            sequenceUtility += itemUtility;
            itemUtilities.push_back(itemUtility);
        }
        utilities.push_back(itemUtilities);
        sequenceUtilities.push_back(sequenceUtility);
    }
}

UtilitiyInfoBySequence::UtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> utilitiesBySequence) {
    this->sid = sid;
    this->utilitiesBySequence = utilitiesBySequence;
}

RemainingUtilitiyInfoBySequence::RemainingUtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> remUtilitiesBySequence) {
    this->sid = sid;
    this->remUtilitiesBySequence = remUtilitiesBySequence;
}

Data::Data(RawData rawData, const float THRESHOLD) {
    /*
        Iterate through the original database to compute SWU for all items.
    */
    std::unordered_map<int, float> itemsSWU;
    for (int seqID = 0; seqID < rawData.sequences.size(); ++seqID) {
        std::unordered_map<int, bool> isItemVisited;
        for (auto const& item : rawData.sequences[seqID]) {
            if (item != -1 && item != -2 && !isItemVisited[item]) {
                itemsSWU[item] += rawData.sequenceUtilities[seqID];
                isItemVisited[item] = true;
            }
        }
    }
    /*
        With the computed SWUs, we prune items whose SWUs are below the input THRESHOLD.
    */
    std::vector<std::vector<int>> _sequences;
    std::vector<std::vector<float>> _utilities;
    std::vector<int> _sequenceUtilities;
    std::vector<int> numItemsetsBySequence;
    std::unordered_map<int, int> mapper;
    for (int seqID = 0; seqID < rawData.sequences.size(); ++seqID) {
        int numItemsets = 0;
        int sequenceUtil = 0;
        std::vector<int> curSequence;
        std::vector<float> curUtilities;
        bool addedToItemset;
        for (int itemIdx = 0; itemIdx < rawData.sequences[seqID].size(); ++itemIdx) {
            int item = rawData.sequences[seqID][itemIdx];
            float itemUtility = rawData.utilities[seqID][itemIdx];
            if (item != -1 && item != -2) {
                if (itemsSWU[item] >= THRESHOLD) {
                    if (mapper.insert({item, numDistinctItems+1}).second) ++numDistinctItems;
                    curSequence.push_back(mapper[item]);
                    curUtilities.push_back(itemUtility);
                    sequenceUtil += itemUtility;
                    if (!addedToItemset) addedToItemset = true;
                }
            } else {
                if (addedToItemset) {
                    ++numItemsets;
                    curSequence.push_back(item);
                    curUtilities.push_back(0);
                    addedToItemset = false;
                }
            }
        }
        if (curSequence.size()) {
            _sequences.push_back(curSequence);
            _utilities.push_back(curUtilities);
            _sequenceUtilities.push_back(sequenceUtil);
            numItemsetsBySequence.push_back(numItemsets);
        }
    }
    numSequences = _sequences.size();
    distinctItems.resize(numDistinctItems);
    std::iota (distinctItems.begin(), distinctItems.end(), 1);
    /*
        Now we construct utilities and remaining utilities matricies.
    */
    std::vector<std::unordered_map<int, std::vector<float>>> utilities;
    std::vector<std::unordered_map<int, std::vector<float>>> remUtilities;
    for (int seqID = 0; seqID < _sequences.size(); ++seqID) {
        std::unordered_map<int, std::vector<float>> utilitiesBySequence;
        std::unordered_map<int, std::vector<float>> remUtilitiesBySequence;
        int itemsetCount = 0;
        float prefixUtility = 0;
        for (int itemIdx = 0; itemIdx < _sequences[seqID].size(); ++itemIdx) {
            int item = _sequences[seqID][itemIdx];
            float itemUtility = _utilities[seqID][itemIdx];
            prefixUtility += itemUtility;
            if (item != -1 && item != -2) {
                /*
                    This find() operation takes log(n) complexity.
                */
                if (utilitiesBySequence.find(item) == utilitiesBySequence.end()) {
                    std::vector<float> itemUtilities(numItemsetsBySequence[seqID]);
                    std::vector<float> itemRemUtilities(numItemsetsBySequence[seqID]);
                    utilitiesBySequence[item] = itemUtilities;
                    remUtilitiesBySequence[item] = itemRemUtilities;
                }
                utilitiesBySequence[item][itemsetCount] = itemUtility;
                remUtilitiesBySequence[item][itemsetCount] = _sequenceUtilities[seqID] - prefixUtility;
            } else ++itemsetCount;
        }
        utilities.push_back(utilitiesBySequence);
        remUtilities.push_back(remUtilitiesBySequence);
    }

    for (int seqID = 0; seqID < utilities.size(); ++seqID) {
        std::vector<std::vector<float>> utilitiesBySequence;
        std::vector<std::vector<float>> remUtilitiesBySequence;
        for (auto const& item : utilities[seqID]) {
            std::vector<float> row;
            row.push_back(item.first);
            for (int utility : item.second) row.push_back(utility);
            utilitiesBySequence.push_back(row);
        }
        for (auto const& item : remUtilities[seqID]) {
            std::vector<float> row;
            row.push_back(item.first);
            for (int remUtility : item.second) row.push_back(remUtility);
            remUtilitiesBySequence.push_back(row);
        }
        utilitiesData.push_back(new UtilitiyInfoBySequence(seqID, utilitiesBySequence));
        remUtilitiesData.push_back(new RemainingUtilitiyInfoBySequence(seqID, remUtilitiesBySequence));
    }
}

UtilityChainNode::UtilityChainNode(int sid, int tid, float acu, float ru) {
    this->sid = sid;
    this->tid = tid;
    this->acu = acu;
    this->ru = ru;
}

Pattern::Pattern(const int extension_c) {
    this->extension_c = extension_c;
    this->peu = 0;
    this->utility = 0;
}
