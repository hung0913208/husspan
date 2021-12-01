#include "algorithms.h"
#include <unordered_map>
#include <iostream>

std::unordered_set<int> computeSCandidate(Data inputData, Pattern pattern, float threshold) {
    std::unordered_set<int> sCandidates;
    std::unordered_map<int, float> candidate_rsu;
    for (auto const& chain : pattern.utilityChains) {

        /*
            S-Candidates exist right after the itemset of the first instance of a pattern in a sequence, so
            for each sequence/utility-chain we only need to use the first utility node to find out all the
            S-Candidates in that sequence/utility-chain.
        */
        UtilityChainNode *current = chain.chainNodes.front();
        std::unordered_set<int> sCandidatesInSequence;

        for (int row_idx = 0; row_idx < inputData.utilitiesData[current->sid]->utilitiesBySequence.size(); ++row_idx) {
            for (int col_idx = current->tid+1; col_idx < inputData.utilitiesData[current->sid]->utilitiesBySequence[row_idx].size(); ++col_idx) {
                if (inputData.utilitiesData[current->sid]->utilitiesBySequence[row_idx][col_idx]) {
                    int candidate = inputData.utilitiesData[current->sid]->utilitiesBySequence[row_idx][0];
                    if (sCandidatesInSequence.insert(candidate).second) {
                        candidate_rsu[candidate] += chain.seqPEU;
                        if (candidate_rsu[candidate] >= threshold) sCandidates.insert(candidate);
                    }
                    /*
                        As long as there is an instance of an item, that item is a S-Candidate.
                    */
                    break;
                }
            }
        }
    }
    return sCandidates;
}

void constructUCForSExtention(Data inputData, Pattern currentPattern, Pattern& extendedPattern) {

    for (auto const& chain : currentPattern.utilityChains) {
        float seqUtility = 0;
        float seqPEU = 0;
        
        UtilityChain extendedUtilityChain;
        for (auto const& curNode : chain.chainNodes) {
            /*
                Consider all of items.
            */
            for (int row_idx = 0; row_idx < inputData.utilitiesData[curNode->sid]->utilitiesBySequence.size(); ++row_idx) {
                /*
                    The first entry of each row represents an item that belongs to this sequence.
                */
                if (inputData.utilitiesData[curNode->sid]->utilitiesBySequence[row_idx][0] == extendedPattern.extension_c) {
                    /*
                        Only look for items with tid > the extension point's tid.
                    */
                    for (int tid_idx = curNode->tid+1 ; tid_idx < inputData.utilitiesData[curNode->sid]->utilitiesBySequence[row_idx].size(); tid_idx++) {
                        /*
                            We found one, we create a utility node for it.
                        */
                        if (inputData.utilitiesData[curNode->sid]->utilitiesBySequence[row_idx][tid_idx]) {
                            float extendedACU = curNode->acu + inputData.utilitiesData[(curNode->sid)]->utilitiesBySequence[row_idx][tid_idx];
                            float extendedREM = inputData.remUtilitiesData[(curNode->sid)]->remUtilitiesBySequence[row_idx][tid_idx];
                            if (extendedREM) {
                                float extendedPEU = extendedACU + extendedREM;
                                if (extendedPEU > seqPEU) seqPEU = extendedPEU;
                            }
                            extendedUtilityChain.chainNodes.push_back(new UtilityChainNode(curNode->sid, tid_idx, extendedACU, extendedREM));
                            if (extendedACU > seqUtility) seqUtility = extendedACU;
                        };
                    }
                    
                    /*
                        Right after we found the extension_c, we can stop and continue with the next sequence/utility-chain.
                    */
                    break;
                }
            }
        }

        if (extendedUtilityChain.chainNodes.size()) {
            extendedPattern.utilityChains.push_back(extendedUtilityChain);
            extendedPattern.utility += seqUtility;
            extendedPattern.peu += seqPEU;
            extendedPattern.utilityChains[extendedPattern.utilityChains.size()-1].seqPEU = seqPEU;
        }

    }
}
