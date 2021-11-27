#include "utils.h"
#include <iostream>
#include <unordered_map>
#include <map>
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
    /*
        Now we construct utilities and remaining utilities matricies.
    */
    std::vector<std::map<int, std::vector<float>>> utilities;
    std::vector<std::map<int, std::vector<float>>> remUtilities;
    for (int seqID = 0; seqID < _sequences.size(); ++seqID) {
        std::map<int, std::vector<float>> utilitiesBySequence;
        std::map<int, std::vector<float>> remUtilitiesBySequence;
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

    std::cout << "#seqs " << numSequences << ", #items " << numDistinctItems << std::endl;

    // for (auto const& seq : remUtilities) {
    //     for (auto const& it : seq) {
    //         std::cout << it.first << " ";
    //         for (auto const& remUtil : it.second) std::cout << remUtil << " ";
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

}
