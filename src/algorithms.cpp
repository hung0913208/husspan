#include "algorithms.h"

void computeSWUs(Data inputData, float* swu_list) {
    for (auto it = begin (inputData.utilities_info); it != end (inputData.utilities_info); ++it) {
        for (int row_idx = 0; row_idx < (it->utilitiesBySequence).size(); row_idx++) {
            swu_list[int(it->utilitiesBySequence[row_idx][0])-1] += inputData.sequence_utilities[(it->sid)-1];
        }
    }
}

std::set<int> computeICandidatesInItemset(Data inputData, int seq_id, int item, int tid) {
    std::set<int> iCandidateInSequence;
    int row_idx = 0;
    while (inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][0] != item) ++row_idx;
    row_idx++;
    while (row_idx < inputData.utilities_info[seq_id].utilitiesBySequence.size()) {
        if (inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][tid]) {
            iCandidateInSequence.insert(inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][0]);
        }
        row_idx++;
    }
    return iCandidateInSequence;
}

std::set<int> computeICandidate(Data inputData, Pattern pattern, float threshold) {
    // std::cout << pattern.pattern << std::endl;
    std::set<int> iCandidates;
    std::map<int, float> candidate_rsu;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {

        /*
            Iterate through the sequence/utility-chain of the current pattern.
        */
        UtilityChainNode *current = pattern.utilityChains[uc_idx]->head;
        std::set<int> iCandidatesInSequence;
        do {
            std::set<int> iCandidatesInItemset = computeICandidatesInItemset(inputData, current->sid-1, pattern.extension_c, current->tid);
            iCandidatesInSequence.insert(iCandidatesInItemset.begin(), iCandidatesInItemset.end());
            current = current->next;
        } while (current != NULL);

        /*
            As long as a candidate within this sequence/utility-chain, its RSU should be increased by the PEU of
            the current pattern.
        */
        for (int candidate : iCandidatesInSequence) candidate_rsu[candidate] += pattern.utilityChains[uc_idx]->seqPEU;

        // iCandidates.insert(iCandidatesInSequence.begin(), iCandidatesInSequence.end());
    }

    /*
        Only candidates whose RSUs are bigger than the threshold are accepted.
    */
    for (auto candidate : candidate_rsu) {
        if (candidate.second >= threshold) {
            iCandidates.insert(candidate.first);
            // std::cout << candidate.first << "-" << candidate.second << std::endl;
        }
    }

    return iCandidates;
}

std::set<int> computeSCandidatesInSequence(Data inputData, int seq_id, int item, int tid) {
    std::set<int> sCandidateInSequence;
    int row_idx = 0;
    while (row_idx < inputData.utilities_info[seq_id].utilitiesBySequence.size()) {
        for (int idx = tid+1; idx < inputData.utilities_info[seq_id].utilitiesBySequence[row_idx].size(); idx++) {
            if (inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][idx]) {
                sCandidateInSequence.insert(inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][0]);
                break;
            }
        }
        row_idx++;
    }
    return sCandidateInSequence;
}

std::set<int> computeSCandidate(Data inputData, Pattern pattern, float threshold) {
    std::set<int> sCandidates;
    std::map<int, float> candidate_rsu;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {

        /*
            S-Candidates exist right after the itemset of the first instance of a pattern in a sequence, so
            for each sequence/utility-chain we only need to use the first utility node to find out all the
            S-Candidates in that sequence/utility-chain.
        */
        UtilityChainNode *current = pattern.utilityChains[uc_idx]->head;
        // std::set<int> sCandidatesInSequence = computeSCandidatesInSequence(inputData, current->sid-1, pattern.extension_c, current->tid);

        std::set<int> sCandidatesInSequence;
        int row_idx = 0;
        while (row_idx < inputData.utilities_info[current->sid-1].utilitiesBySequence.size()) {
            for (int idx = current->tid+1; idx < inputData.utilities_info[current->sid-1].utilitiesBySequence[row_idx].size(); idx++) {
                if (inputData.utilities_info[current->sid-1].utilitiesBySequence[row_idx][idx]) {
                    int candidate = inputData.utilities_info[current->sid-1].utilitiesBySequence[row_idx][0];
                    if (sCandidatesInSequence.insert(candidate).second) {
                        candidate_rsu[candidate] += pattern.utilityChains[uc_idx]->seqPEU;
                        // if (candidate_rsu[candidate] >= threshold) sCandidates.insert(candidate);
                    }
                    break;
                }
            }
            row_idx++;
        }

        // for (int candidate : sCandidatesInSequence) candidate_rsu[candidate] += pattern.utilityChains[uc_idx]->seqPEU;
    }

    for (auto candidate : candidate_rsu) {
        if (candidate.second >= threshold) {
            sCandidates.insert(candidate.first);
            // std::cout << candidate.first << "-" << candidate.second << std::endl;
        }
    }

    return sCandidates;
}

std::vector<std::shared_ptr<UtilityChain>> constructUCForIExtention(Data inputData, Pattern currentPattern, int extension_c) {
    std::vector<std::shared_ptr<UtilityChain>> utilityChains;

    for (int uc_idx = 0; uc_idx < currentPattern.utilityChains.size(); uc_idx++) {
        std::shared_ptr<UtilityChain> extendedUtilityChain = std::make_shared<UtilityChain>();
        UtilityChainNode *curNode = currentPattern.utilityChains[uc_idx]->head;
        do {
            int row_idx = 0;
            /*
                Go to the extension position/item of the current pattern. 
            */
            while (inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][0] != currentPattern.extension_c) ++row_idx;
            row_idx++;
            /*
                Look for the candidate item.
            */
            while (row_idx < inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence.size()) {
                if (
                    inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][0] == extension_c &&
                    inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][curNode->tid]
                ) {
                    /*
                        We found one, we create a utility node for it.
                    */
                    // UtilityChainNode * utilityChainNode = new UtilityChainNode(
                    //     curNode->sid, 
                    //     curNode->tid, 
                    //     curNode->acu + inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][curNode->tid],
                    //     inputData.remaining_utilities_info[(curNode->sid)-1].remainingUtilitiesBySequence[row_idx][curNode->tid]
                    // );
                    // std::cout << "Created node " << utilityChainNode->sid << " " << utilityChainNode->tid << " " << utilityChainNode->acu << " " << utilityChainNode->ru << std::endl;
                    extendedUtilityChain->append(new UtilityChainNode(
                        curNode->sid, 
                        curNode->tid, 
                        curNode->acu + inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][curNode->tid],
                        inputData.remaining_utilities_info[(curNode->sid)-1].remainingUtilitiesBySequence[row_idx][curNode->tid]
                    ));
                };
                row_idx++;
            }
            curNode = curNode->next;
        } while (curNode != NULL);
        if (extendedUtilityChain->head) utilityChains.push_back(extendedUtilityChain);
    }

    return utilityChains;
}

void constructUCForSExtention(Data inputData, Pattern currentPattern, Pattern& extendedPattern) {

    for (int uc_idx = 0; uc_idx < currentPattern.utilityChains.size(); uc_idx++) {
        float seqUtility = 0;
        float seqPEU = 0;
        
        std::shared_ptr<UtilityChain> extendedUtilityChain = std::make_shared<UtilityChain>();
        UtilityChainNode *curNode = currentPattern.utilityChains[uc_idx]->head;
        do {
            /*
                Consider all of items.
            */
            int row_idx = 0;
            while (row_idx < inputData.utilities_info[curNode->sid-1].utilitiesBySequence.size()) {
                /*
                    Only look for items with tid > the extension point's tid.
                */
                for (int tid_idx = curNode->tid+1 ; tid_idx < inputData.utilities_info[curNode->sid-1].utilitiesBySequence[row_idx].size(); tid_idx++) {
                    /*
                        We found one, we create a utility node for it.
                    */
                    if (
                        inputData.utilities_info[curNode->sid-1].utilitiesBySequence[row_idx][0] == extendedPattern.extension_c &&
                        inputData.utilities_info[curNode->sid-1].utilitiesBySequence[row_idx][tid_idx]
                    ) {
                        float extendedACU = curNode->acu + inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][tid_idx];
                        float extendedREM = inputData.remaining_utilities_info[(curNode->sid)-1].remainingUtilitiesBySequence[row_idx][tid_idx];
                        float extendedPEU = extendedACU + extendedREM;
                        extendedUtilityChain->append(new UtilityChainNode(
                            curNode->sid,
                            tid_idx,
                            extendedACU,
                            extendedREM
                        ));
                        if (extendedACU > seqUtility) seqUtility = extendedACU;
                        if (extendedPEU > seqPEU) seqPEU = extendedPEU;
                    };
                }
                row_idx++;
            }
            curNode = curNode->next;
        } while (curNode != NULL);

        if (extendedUtilityChain->head) {
            extendedPattern.utilityChains.push_back(extendedUtilityChain);
            extendedPattern.utility += seqUtility;
            extendedPattern.peu += seqPEU;
            extendedPattern.utilityChains[extendedPattern.utilityChains.size()-1]->seqPEU = seqPEU;
        }

    }
}

void computePatternUtilityAndPEU(Pattern& pattern) {
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        float seqUtility = 0;
        float seqPEU = 0;
        UtilityChainNode *current = pattern.utilityChains[uc_idx]->head;
        do {
            if (current->acu > seqUtility) {
                seqUtility = current->acu;
            }
            if (current->ru > 0) {
                float current_peu = current->acu + current->ru;
                if (current_peu > seqPEU) {
                    seqPEU = current_peu;
                }
            }
            current = current->next;
        } while (current != NULL);
        pattern.utility += seqUtility;
        pattern.peu += seqPEU;
        pattern.utilityChains[uc_idx]->seqPEU = seqPEU;
    }
}
