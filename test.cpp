#include <iostream>
#include <iomanip>
#include <set>
#include "src/algorithms.h"
#include "src/utilities.h"

void husspan(Data inputData, Pattern currentPattern, float threshold) {

    /*
        Compute PEU.
    */
    if (currentPattern.peu < threshold) {
        // std::cout << "ELIMINATED " << currentPattern.pattern << " PEU " << currentPattern.peu << std::endl;
        return;
    } 
    // else {
        // std::cout << "VALID " << currentPattern.pattern << " PEU " << currentPattern.peu << std::endl;
    // }

    /*
        Compute I-Candidates
    */
    // std::set<int> iCandidates = computeICandidate(inputData, currentPattern, threshold);
    // std::cout << "The number of I-Candidates of " << currentPattern.pattern << " is " << iCandidates.size() << std::endl;

    /*
        Compute S-Candidates
    */
    std::unordered_set<int> sCandidates = computeSCandidate(inputData, currentPattern, threshold);
    // std::cout << "The number of S-Candidates of " << currentPattern.pattern << " is " << sCandidates.size() << std::endl;

    /*
        Form new patterns by extending current patterns with I/S-Candidates.
    */
    // std::cout << ">>I-EXTENTIONS" << std::endl;
    // for (int item : iCandidates) {

        /*
            Construct the extended pattern.
        */
        // Pattern extended_pattern(item);
        // extended_pattern.pattern = currentPattern.pattern;
        // extended_pattern.pattern.append(" ").append(std::to_string(item));
        // std::cout << "Extended pattern " << extended_pattern.pattern << std::endl;

        /*
            Construct the Utility Chain for the extended pattern.
        */
        // extended_pattern.utilityChains = constructUCForIExtention(inputData, currentPattern, extended_pattern.extension_c);
        // std::cout << "Number of UCs/Sequences " << extended_pattern.utilityChains.size() << std::endl;

    //     computePatternUtilityAndPEU(extended_pattern);
    //     if (extended_pattern.utility >= threshold) {
    //         std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << extended_pattern.utility << std::endl;
    //         if (++generatedPatterns >= maxPatterns) exit(0);
    //     }

    //     husspan(inputData, extended_pattern, threshold, generatedPatterns, maxPatterns);
    // }

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
        // extended_pattern.utilityChains = constructUCForSExtention(inputData, currentPattern, extended_pattern);
        constructUCForSExtention(inputData, currentPattern, extended_pattern);
        // std::cout << "Number of UCs/Sequences " << extended_pattern.utilityChains.size() << std::endl;

        // computePatternUtilityAndPEU(extended_pattern);
        if (extended_pattern.utility >= threshold) {
            std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << extended_pattern.utility << std::endl;
        }

        husspan(inputData, extended_pattern, threshold);
    }
}

int main(int argvc, char** argv) {

    float threshold = std::stof(argv[1]);
    std::string inputDataPath = argv[2];
    // int maxPatterns = std::stoi(argv[3]);

    // int generatedPatterns = 0;
    Data inputData(inputDataPath);

    // float* swu_list = (float*) calloc(inputData.num_items, sizeof(float));

    // computeSWUs(inputData, swu_list);

    /*
        Prune items with low SWU.
    */
    // for (int idx = 0; idx < inputData.num_items; idx++) {
    //     if (swu_list[idx] < threshold) {
    //         inputData.num_items -= 1;
    //         for (int inner_idx = idx; inner_idx < inputData.num_items; inner_idx++) {
    //             inputData.items[inner_idx] = inputData.items[inner_idx+1];
    //             swu_list[inner_idx] = swu_list[inner_idx+1];
    //         }
    //         idx -= 1;
    //     }
    // }

    /*
        Initialize UC for 1-sequences.
    */
    for (int item : inputData.items) {
        Pattern pattern(item);
        pattern.pattern.append(std::to_string(item));
        /*
            Iterate over each sequence.
        */
        for (auto const& it : inputData.utilities_info) {
            /*
                Iterate over each row.
            */
            float seqUtility = 0;
            float seqPEU = 0;
            UtilityChain utilityChain;

            for (int row_idx = 0; row_idx < it.utilitiesBySequence.size(); ++row_idx) {
                /*
                    The first entry of each row represents an item that belongs to this sequence.
                */
                if (it.utilitiesBySequence[row_idx][0] == pattern.extension_c) {
                    /*
                        Now iterate over each tid to obtain the info for the utility chain.
                    */
                    for (int col_idx = 1; col_idx < it.utilitiesBySequence[row_idx].size(); ++col_idx) {
                        if (it.utilitiesBySequence[row_idx][col_idx]) {
                            float extendedACU = it.utilitiesBySequence[row_idx][col_idx];
                            float extendedREM = inputData.remaining_utilities_info[(it.sid)-1].remainingUtilitiesBySequence[row_idx][col_idx];
                            float extendedPEU = extendedACU + extendedREM;
                            // std::cout << "At TID " << col_idx << " has utility " << it.utilitiesBySequence[row_idx][col_idx] << std::endl;
                            utilityChain.chainNodes.push_back(new UtilityChainNode(it.sid, col_idx, extendedACU, extendedREM));
                            if (extendedACU > seqUtility) seqUtility = extendedACU;
                            if (extendedPEU > seqPEU) seqPEU = extendedPEU;
                        }
                    }

                    /*
                        Right after we found the extension_c, we can stop and continue with the next sequence/utility-chain.
                    */
                    break;
                }

            }

            /*
                All the instances info of this pattern are stored in the utilityChain,
            */
            if (utilityChain.chainNodes.size()) {
                pattern.utilityChains.push_back(utilityChain);
                pattern.utility += seqUtility;
                pattern.peu += seqPEU;
                pattern.utilityChains[pattern.utilityChains.size()-1].seqPEU = seqPEU;
            }
        }

        // computePatternUtilityAndPEU(pattern);

        husspan(inputData, pattern, threshold);
    }

    return 0;
}
