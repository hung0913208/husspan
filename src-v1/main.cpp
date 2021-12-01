#include <iostream>
#include <iomanip>
#include "utils.h"
#include "algorithms.h"
#include <functional>
#include <taskflow/taskflow.hpp>

void husspan(Data inputData, Pattern currentPattern, float threshold) {
    // std::cout << "Pattern " << currentPattern.pattern << "thread " << std::this_thread::get_id() << std::endl;
    /*
        Compute PEU.
    */
    if (currentPattern.peu < threshold) return;

    std::unordered_set<int> sCandidates = computeSCandidate(inputData, currentPattern, threshold);
    // std::cout << "The number of S-Candidates of " << currentPattern.pattern << " is " << sCandidates.size() << std::endl;

    /*
        Form new patterns by extending current patterns with I/S-Candidates.
    */
    // std::cout << ">>S-EXTENTIONS" << std::endl;
    for (int item : sCandidates) {
        /*
            Construct the extended pattern.
        */
        Pattern extended_pattern(item);
        extended_pattern.pattern = currentPattern.pattern;
        extended_pattern.pattern.append(" -1 ").append(std::to_string(item));

        constructUCForSExtention(inputData, currentPattern, extended_pattern);

        if (extended_pattern.utility >= threshold) {
            std::cout << "FOUND\t" << std::left << std::setw(120) << extended_pattern.pattern << " with utility\t" << extended_pattern.utility << std::endl;
        }

        husspan(inputData, extended_pattern, threshold);
    }
}

int main(int argvc, char** argv) {

    const float THRESHOLD = std::stof(argv[1]);
    const std::string INPUT_DATA_PATH = argv[2];
    
    RawData rawData(INPUT_DATA_PATH);
    Data inputData(rawData, THRESHOLD);

    tf::Executor executor;
    tf::Taskflow taskflow;
    /*
        Initialize UC for 1-sequences.
    */
    for (int item : inputData.distinctItems) {
        Pattern pattern(item);
        pattern.pattern.append(std::to_string(item));
        /*
            Iterate over each sequence.
        */
        for (auto const& seq : inputData.utilitiesData) {
            /*
                Iterate over each row.
            */
            float seqUtility = 0;
            float seqPEU = 0;
            UtilityChain utilityChain;
            for (int row_idx = 0; row_idx < seq->utilitiesBySequence.size(); ++row_idx) {
                /*
                    The first entry of each row represents an item that belongs to this sequence.
                */
                if (seq->utilitiesBySequence[row_idx][0] == pattern.extension_c) {
                    /*
                        Now iterate over each tid to obtain the info for the utility chain.
                    */
                    for (int col_idx = 1; col_idx < seq->utilitiesBySequence[row_idx].size(); ++col_idx) {
                        if (seq->utilitiesBySequence[row_idx][col_idx]) {
                            float extendedACU = seq->utilitiesBySequence[row_idx][col_idx];
                            float extendedREM = inputData.remUtilitiesData[seq->sid]->remUtilitiesBySequence[row_idx][col_idx];
                            float extendedPEU = extendedACU + extendedREM;
                            // std::cout << "At TID " << col_idx << " has utility " << seq->utilitiesBySequence[row_idx][col_idx] << std::endl;
                            utilityChain.chainNodes.push_back(new UtilityChainNode(seq->sid, col_idx, extendedACU, extendedREM));
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
        
        taskflow.emplace([=](){ husspan(inputData, pattern, THRESHOLD); });
    }

    executor.run(taskflow).wait();

    return 0;
}
