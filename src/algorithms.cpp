#include "algorithms.h"

void computeSWUs(Data inputData, float* swu_list) {
    for (auto it = begin (inputData.utilities_info); it != end (inputData.utilities_info); ++it) {
        for (int row_idx = 0; row_idx < (it->utilitiesBySequence).size(); row_idx++) {
            swu_list[int(it->utilitiesBySequence[row_idx][0])-1] += inputData.sequence_utilities[(it->sid)-1];
        }
    }
}

float computePEU(Pattern pattern) {
    float peu = 0;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        // std::cout << "Chain " << uc_idx << std::endl;
        int sequence_peu = 0;
        UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
        do {
            if (current->ru > 0) {
                float current_peu = current->acu + current->ru;
                if (current_peu > sequence_peu) sequence_peu = current_peu;
            }
            current = current->next;
        } while (current != NULL);
        if (sequence_peu != 0) peu += sequence_peu;
        // std::cout << sequence_peu << std::endl;
    }
    // std::cout << peu << std::endl;
    return peu;
}

std::set<int> computeICandidatesInSequence(Data inputData, int seq_id, int item, int tid) {
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

std::set<int> computeICandidate(Data inputData, Pattern pattern) {
    std::set<int> iCandidates;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
        do {
            std::set<int> iCandidatesInSequence = computeICandidatesInSequence(inputData, current->sid-1, pattern.extension_c, current->tid);
            iCandidates.insert(iCandidatesInSequence.begin(), iCandidatesInSequence.end());
            current = current->next;
        } while (current != NULL);
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

std::set<int> computeSCandidate(Data inputData, Pattern pattern) {
    std::set<int> sCandidates;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
        do {
            std::set<int> sCandidatesInSequence = computeSCandidatesInSequence(inputData, current->sid-1, pattern.extension_c, current->tid);
            sCandidates.insert(sCandidatesInSequence.begin(), sCandidatesInSequence.end());
            current = current->next;
        } while (current != NULL);
    }
    return sCandidates;
}

bool isICandidateValidForRSU(Data inputData, int seq_id, int item, int tid, int iCandidate) {
    bool isValid = false;
    int row_idx = 0;
    while (inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][0] != item) ++row_idx;
    row_idx++;
    while (row_idx < inputData.utilities_info[seq_id].utilitiesBySequence.size()) {
        if (
            inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][0] == iCandidate &&
            inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][tid]
        ) isValid = true;
        row_idx++;
    }
    return isValid;
}

int computeRSUForICandidate(Data inputData, Pattern pattern, int iCandidate) {
    float rsu = 0;
    // std::cout << "Utility chain of " << pattern.pattern << std::endl;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        // std::cout << "Chain " << uc_idx << std::endl;
        UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
        bool isValidForRSU = false;
        do {
            // std::cout << "SID " << current->sid << " " << current->tid << std::endl;
            if (isICandidateValidForRSU(inputData, current->sid-1, pattern.extension_c, current->tid, iCandidate)) {
                isValidForRSU = true;
                break;
            }
            current = current->next;
        } while (current != NULL);
        /*
            When the extension item is valid for computing RSU.
        */
        if (isValidForRSU) {
            float sequence_peu = 0;
            UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
            do {
                if (current->ru > 0) {
                    float current_peu = current->acu + current->ru;
                    if (current_peu > sequence_peu) sequence_peu = current_peu;
                }
                current = current->next;
            } while (current != NULL);
            rsu += sequence_peu;
        }
    }
    return rsu;
}

bool isSCandidateValidForRSU(Data inputData, int seq_id, int item, int tid, int sCandidate) {
    bool isValid = false;
    int row_idx = 0;
    while (row_idx < inputData.utilities_info[seq_id].utilitiesBySequence.size()) {
        for (int idx = tid+1; idx < inputData.utilities_info[seq_id].utilitiesBySequence[row_idx].size(); idx++) {
            if (
                inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][0] == sCandidate &&
                inputData.utilities_info[seq_id].utilitiesBySequence[row_idx][idx]
            ) isValid = true;
        }
        row_idx++;
    }
    return isValid;
}

int computeRSUForSCandidate(Data inputData, Pattern pattern, int iCandidate) {
    float rsu = 0;
    // std::cout << "Utility chain of " << pattern.pattern << std::endl;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        // std::cout << "Chain " << uc_idx << std::endl;
        UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
        bool isValidForRSU = false;
        do {
            // std::cout << "SID " << current->sid << " " << current->tid << std::endl;
            if (isSCandidateValidForRSU(inputData, current->sid-1, pattern.extension_c, current->tid, iCandidate)) {
                isValidForRSU = true;
                break;
            }
            current = current->next;
        } while (current != NULL);
        /*
            When the extension item is valid for computing RSU.
        */
        if (isValidForRSU) {
            float sequence_peu = 0;
            UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
            do {
                if (current->ru > 0) {
                    float current_peu = current->acu + current->ru;
                    if (current_peu > sequence_peu) sequence_peu = current_peu;
                }
                current = current->next;
            } while (current != NULL);
            rsu += sequence_peu;
        }
    }
    return rsu;
}

std::vector<UtilityChain> constructUCForIExtention(Data inputData, Pattern currentPattern, int extension_c) {
    std::vector<UtilityChain> utilityChains;

    for (int uc_idx = 0; uc_idx < currentPattern.utilityChains.size(); uc_idx++) {
        UtilityChain extendedUtilityChain;
        UtilityChainNode *curNode = currentPattern.utilityChains[uc_idx].head;
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
                    UtilityChainNode * utilityChainNode = new UtilityChainNode(
                        curNode->sid, 
                        curNode->tid, 
                        curNode->acu + inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][curNode->tid],
                        inputData.remaining_utilities_info[(curNode->sid)-1].remainingUtilitiesBySequence[row_idx][curNode->tid]
                    );
                    // std::cout << "Created node " << utilityChainNode->sid << " " << utilityChainNode->tid << " " << utilityChainNode->acu << " " << utilityChainNode->ru << std::endl;
                    extendedUtilityChain.append(utilityChainNode);
                };
                row_idx++;
            }
            curNode = curNode->next;
        } while (curNode != NULL);
        if (extendedUtilityChain.head) utilityChains.push_back(extendedUtilityChain);
    }

    // for (int uc_idx = 0; uc_idx < utilityChains.size(); uc_idx++) {
    //     std::cout << "UC " << uc_idx << std::endl;
    //     UtilityChainNode *node = utilityChains[uc_idx].head;
    //     do {
    //         std::cout << "Node " << node->sid << " " << node->tid << " " << node->acu << " " << node->ru << std::endl;
    //         node = node->next;
    //     } while (node != NULL);
    // }

    return utilityChains;
}

std::vector<UtilityChain> constructUCForSExtention(Data inputData, Pattern currentPattern, int extension_c) {
    std::vector<UtilityChain> utilityChains;

    for (int uc_idx = 0; uc_idx < currentPattern.utilityChains.size(); uc_idx++) {
        UtilityChain extendedUtilityChain;
        UtilityChainNode *curNode = currentPattern.utilityChains[uc_idx].head;
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
                        inputData.utilities_info[curNode->sid-1].utilitiesBySequence[row_idx][0] == extension_c &&
                        inputData.utilities_info[curNode->sid-1].utilitiesBySequence[row_idx][tid_idx]
                    ) {
                        UtilityChainNode * utilityChainNode = new UtilityChainNode(
                            curNode->sid,
                            tid_idx,
                            curNode->acu + inputData.utilities_info[(curNode->sid)-1].utilitiesBySequence[row_idx][tid_idx],
                            inputData.remaining_utilities_info[(curNode->sid)-1].remainingUtilitiesBySequence[row_idx][tid_idx]
                        );
                        // std::cout << "Created node " << utilityChainNode->sid << " " << utilityChainNode->tid << " " << utilityChainNode->acu << " " << utilityChainNode->ru << std::endl;
                        extendedUtilityChain.append(utilityChainNode);
                    };
                }
                row_idx++;
            }
            curNode = curNode->next;
        } while (curNode != NULL);
        if (extendedUtilityChain.head) utilityChains.push_back(extendedUtilityChain);
    }

    return utilityChains;
}

float computePatternUtility(Pattern pattern) {
    float u = 0;
    for (int uc_idx = 0; uc_idx < pattern.utilityChains.size(); uc_idx++) {
        float sequence_u = 0;
        UtilityChainNode *current = pattern.utilityChains[uc_idx].head;
        do {
            if (current->acu > sequence_u) sequence_u = current->acu;
            current = current->next;
        } while (current != NULL);
        u += sequence_u;
        // std::cout << "sequence utility of chain " << uc_idx << " is " << sequence_u << " and u is " << u << std::endl;
    }
    return u;
}

// int pruneLowSWUItems(Data inputData, int* swu_list, int threshold) {
//     int num_items = inputData.num_items;
//     for (int idx = 0; idx < num_items; idx++) {
//         if (swu_list[idx] < threshold) {
//             num_items -= 1;
//             for (int inner_idx = idx; inner_idx < num_items; inner_idx++) {
//                 inputData.items[inner_idx] = inputData.items[inner_idx+1];
//                 swu_list[inner_idx] = swu_list[inner_idx+1];
//             }
//             idx -= 1;
//         }
//     }
//     return num_items;
// }
