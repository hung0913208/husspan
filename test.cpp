#include <iostream>
#include <iomanip>
#include <set>
#include "src/algorithms.h"
#include "src/utilities.h"

void husspan(Data inputData, Pattern currentPattern, float threshold) {
    /*
        Compute PEU.
    */
    float peu = computePEU(currentPattern);
    if (peu < threshold) {
        // std::cout << "RETURN: PEU " << peu << " is less than the threshold" << std::endl;
        return;
    }
    // std::cout << currentPattern.pattern << " has PEU of first sequence of " << currentPattern.utilityChains[0].seqPEU;

    /*
        Compute I-Candidates
    */
    std::set<int> iCandidates = computeICandidate(inputData, currentPattern, threshold);
    // std::cout << "The number of candidates of " << currentPattern.pattern << " is " << iCandidates.size() << std::endl;

    // exit(1);
    // std::cout << "I candidates of " << currentPattern.pattern << ": ";
    // for (auto it = iCandidates.begin(); it != iCandidates.end(); ++it) {
    //     std::cout << *it << " ";
    // }
    // std::cout << std::endl;

    /*
        Compute S-Candidates
    */
    std::set<int> sCandidates = computeSCandidate(inputData, currentPattern, threshold);
    // std::cout << "S-Extention candidates of " << currentPattern.extension_c << ": ";
    // for (auto it = sCandidates.begin(); it != sCandidates.end(); ++it) {
    //     std::cout << *it << " ";
    // }
    // std::cout << std::endl;
    /*
        Compute RSU then remove candidates with low RSU.
    */
    // std::vector<int> itemsToDelete;
    // for (int item : iCandidates) {
    //     int itemRSU = computeRSUForICandidate(inputData, currentPattern, item);
    //     std::cout << "I-Candidate " << item << " with RSU " << itemRSU << " " << std::endl;
    //     if (itemRSU < threshold) {
    //         std::cout << "REMOVE: I-Candidate RSU " << itemRSU << std::endl;
    //         itemsToDelete.push_back(item);
    //     }
    // }
    // for (int item : itemsToDelete) {
    //     iCandidates.erase(item);
    // }
    // for (int item : sCandidates) {
    //     int itemRSU = computeRSUForSCandidate(inputData, currentPattern, item);
    //     std::cout << "S-Candidate " << item << " with RSU " << itemRSU << " " << std::endl;
    //     if (itemRSU < threshold) {
    //         std::cout << "REMOVE: S-Candidate RSU " << itemRSU << std::endl;
    //         itemsToDelete.push_back(item);
    //     }
    // }
    // for (int item : itemsToDelete) {
    //     sCandidates.erase(item);
    // }
    /*
        Display I/S-Candidates.
    */
    // std::cout << "I-Candidates ";
    // for (int item : iCandidates) {
    //     std::cout << item << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "S-Candidates ";
    // for (int item : sCandidates) {
    //     std::cout << item << " ";
    // }
    // std::cout << std::endl;
    /*
        Form new patterns by extending current patterns with I/S-Candidates.
    */
    // std::cout << ">>I-EXTENTIONS" << std::endl;
    for (int item : iCandidates) {
        /*
            Construct the extended pattern.
        */
        Pattern extended_pattern(item);
        extended_pattern.pattern = currentPattern.pattern;
        extended_pattern.pattern.append(" ").append(std::to_string(item));
        // std::cout << "Extended pattern " << extended_pattern.pattern << std::endl;
        /*
            Construct the Utility Chain for the extended pattern.
        */
        extended_pattern.utilityChains = constructUCForIExtention(inputData, currentPattern, extended_pattern.extension_c);
        // std::cout << "Number of UCs/Sequences " << extended_pattern.utilityChains.size() << std::endl;

        // std::cout << "PEU of " << extended_pattern.pattern << " is " << computePEU(extended_pattern) << std::endl;
        // std::cout << "Utitliy of " << extended_pattern.pattern << " is " << computePatternUtility(extended_pattern) << std::endl;

        float patternUtility = computePatternUtility(extended_pattern);
        if (patternUtility >= threshold) {
            std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << patternUtility << std::endl;
        } else {
            // std::cout << "-MISSED\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << patternUtility << std::endl;
        }

        husspan(inputData, extended_pattern, threshold);
    }

    // std::cout << ">>S-EXTENTIONS" << std::endl;
    for (int item : sCandidates) {
        /*
            Construct the extended pattern.
        */
        Pattern extended_pattern(item);
        extended_pattern.pattern = currentPattern.pattern;
        extended_pattern.pattern.append(" -1 ").append(std::to_string(item));
        // std::cout << "Extended pattern " << extended_pattern.pattern << std::endl;
        /*
            Construct the Utility Chain for the extended pattern.
        */
        extended_pattern.utilityChains = constructUCForSExtention(inputData, currentPattern, extended_pattern.extension_c);
        // std::cout << "Number of UCs/Sequences " << extended_pattern.utilityChains.size() << std::endl;

        // std::cout << "PEU of " << extended_pattern.pattern << " is " << computePEU(extended_pattern) << std::endl;
        // std::cout << "Utitliy of " << extended_pattern.pattern << " is " << computePatternUtility(extended_pattern) << std::endl;

        int patternUtility = computePatternUtility(extended_pattern);
        if (patternUtility >= threshold) {
            std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << patternUtility << std::endl;
        } else {
            // std::cout << "-MISSED\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << patternUtility << std::endl;
        }

        husspan(inputData, extended_pattern, threshold);
    }
}

int main(int argvc, char** argv) {

    float threshold = std::stof(argv[1]);
    std::string inputDataPath = argv[2];

    Data inputData(inputDataPath);

    float* swu_list = (float*) calloc(inputData.num_items, sizeof(float));

    computeSWUs(inputData, swu_list);

    // for (int itemIdx = 0; itemIdx < 10; itemIdx++) {
    //     std::cout << inputData.items[itemIdx] << " has swu " << swu_list[itemIdx] << std::endl;
    // }
    // std::cout << std::endl;

    /*
        Prune items with low SWU.
    */
    for (int idx = 0; idx < inputData.num_items; idx++) {
        if (swu_list[idx] < threshold) {
            inputData.num_items -= 1;
            for (int inner_idx = idx; inner_idx < inputData.num_items; inner_idx++) {
                inputData.items[inner_idx] = inputData.items[inner_idx+1];
                swu_list[inner_idx] = swu_list[inner_idx+1];
            }
            idx -= 1;
        }
    }

    /*
        Initialize UC for 1-sequences.
    */
    // std::cout << "Considering item " << inputData.items[0] << std::endl;
    for (int item : inputData.items) {
        Pattern pattern(item);
        pattern.pattern.append(std::to_string(item));
        /*
            Iterate over each sequence.
        */
        for (auto it = begin (inputData.utilities_info); it != end (inputData.utilities_info); it++) {
            /*
                Iterate over each row.
            */
            for (int row_idx = 0; row_idx < it->utilitiesBySequence.size(); row_idx++) {
                /*
                    The first entry of each row represents an item that belongs to this sequence.
                */
                if (it->utilitiesBySequence[row_idx][0] == pattern.extension_c) {
                    // std::cout << "Extension item " << pattern.extension_c << " is found!" << std::endl;
                    UtilityChain utilityChain;
                    /*
                        Now iterate over each tid to obtain the info for the utility chain.
                    */
                    for (int col_idx = 1; col_idx < it->utilitiesBySequence[row_idx].size(); col_idx++) {
                        if (it->utilitiesBySequence[row_idx][col_idx]) {
                            // std::cout << "At TID " << col_idx << " has utility " << it->utilitiesBySequence[row_idx][col_idx] << std::endl;
                            utilityChain.append(
                                new UtilityChainNode(
                                    it->sid,
                                    col_idx,
                                    it->utilitiesBySequence[row_idx][col_idx], 
                                    inputData.remaining_utilities_info[(it->sid)-1].remainingUtilitiesBySequence[row_idx][col_idx]
                                )
                            );
                        }
                    }
                    pattern.utilityChains.push_back(utilityChain);
                }
            }
            // std::cout << std::endl;
        }

        husspan(inputData, pattern, threshold);
    }

    return 0;
}
