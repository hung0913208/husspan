#include <iostream>
#include <iomanip>
#include <set>
#include "src/algorithms.h"
#include "src/utilities.h"

void husspan(Data inputData, Pattern currentPattern, float threshold, int& generatedPatterns, int maxPatterns) {

    /*
        Compute PEU.
    */
    if (currentPattern.peu < threshold) return;

    /*
        Compute I-Candidates
    */
    std::set<int> iCandidates = computeICandidate(inputData, currentPattern, threshold);
    // std::cout << "The number of I-Candidates of " << currentPattern.pattern << " is " << iCandidates.size() << std::endl;

    /*
        Compute S-Candidates
    */
    std::set<int> sCandidates = computeSCandidate(inputData, currentPattern, threshold);
    // std::cout << "The number of S-Candidates of " << currentPattern.pattern << " is " << sCandidates.size() << std::endl;

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

        computePatternUtilityAndPEU(extended_pattern);
        if (extended_pattern.utility >= threshold) {
            std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << extended_pattern.utility << std::endl;
            if (++generatedPatterns >= maxPatterns) exit(0);
        }

        husspan(inputData, extended_pattern, threshold, generatedPatterns, maxPatterns);
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

        computePatternUtilityAndPEU(extended_pattern);
        if (extended_pattern.utility >= threshold) {
            std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << extended_pattern.utility << std::endl;
            if (++generatedPatterns >= maxPatterns) exit(0);
        }

        husspan(inputData, extended_pattern, threshold, generatedPatterns, maxPatterns);
    }
}

int main(int argvc, char** argv) {

    float threshold = std::stof(argv[1]);
    std::string inputDataPath = argv[2];
    int maxPatterns = std::stoi(argv[3]);

    int generatedPatterns = 0;
    Data inputData(inputDataPath);

    float* swu_list = (float*) calloc(inputData.num_items, sizeof(float));

    computeSWUs(inputData, swu_list);

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
                    std::shared_ptr<UtilityChain> utilityChain = std::make_shared<UtilityChain>();
                    /*
                        Now iterate over each tid to obtain the info for the utility chain.
                    */
                    for (int col_idx = 1; col_idx < it->utilitiesBySequence[row_idx].size(); col_idx++) {
                        if (it->utilitiesBySequence[row_idx][col_idx]) {
                            // std::cout << "At TID " << col_idx << " has utility " << it->utilitiesBySequence[row_idx][col_idx] << std::endl;
                            utilityChain->append(
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

        computePatternUtilityAndPEU(pattern);

        husspan(inputData, pattern, threshold, generatedPatterns, maxPatterns);
    }

    return 0;
}
