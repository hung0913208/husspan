#include "utilities.h"
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

UtilitiyInfoBySequence::UtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> utilitiesBySequence) {
    this->sid = sid;
    this->utilitiesBySequence = utilitiesBySequence;
}

RemainingUtilitiyInfoBySequence::RemainingUtilitiyInfoBySequence(int sid, std::vector<std::vector<float>> remainingUtilitiesBySequence) {
    this->sid = sid;
    this->remainingUtilitiesBySequence = remainingUtilitiesBySequence;
}

Data::Data(std::string dirPath) {

    std::string line;
    std::vector<std::string> tokens;

    std::fstream inDataInfoFile (dirPath + "/" + DATA_INFO_FILE);
    std::fstream inUtilityFile (dirPath + "/" + UTILITIES_FILE);
    std::fstream inRemainingUtilityFile (dirPath + "/" + REMAINING_UTILITIES_FILE);
    std::fstream inSequenceUtilityFile (dirPath + "/" + SEQUENCE_UTILITIES_FILE);

    if (inDataInfoFile.is_open()) {

        std::getline(inDataInfoFile, line);
        num_items = std::stoi(line);
        // std::cout << num_items << std::endl;

        std::getline(inDataInfoFile, line);
        boost::split(tokens, line, boost::is_any_of("\t"));
        for (int i = 0; i < num_items; i++) {
            // std::cout << " " << tokens[i] << " ";
            items.push_back(std::stoi(tokens[i]));
        }
        // std::cout << std::endl;

        std::getline(inDataInfoFile, line);
        num_sequences = std::stoi(line);
        // std::cout << num_sequences << std::endl;

    }

    if (inSequenceUtilityFile.is_open()) {
        std::getline(inSequenceUtilityFile, line);
        boost::split(tokens, line, boost::is_any_of("\t"));
        for (int i = 0; i < num_sequences; i++) {
            // std::cout << " " << tokens[i] << " ";
            sequence_utilities.push_back(std::stof(tokens[i]));
        }
        // std::cout << std::endl;
    }

    if (inUtilityFile.is_open()) {

        int sequenceID = 1;
        std::vector<std::vector<float>> utilitiesBySequence;

        while (std::getline(inUtilityFile, line)) {

            if (!std::isspace(line[0])) {

                boost::split(tokens, line, boost::is_any_of("\t"));
                std::vector<float> row;

                for (int i = 0; i < tokens.size(); i++) {
                    // std::cout << " " << tokens[i] << " ";
                    row.push_back(std::stof(tokens[i]));
                }
                // std::cout << std::endl;

                utilitiesBySequence.push_back(row);

            } else {
                UtilitiyInfoBySequence utilitiesInfoBySequence(sequenceID, utilitiesBySequence);
                utilities_info.push_back(utilitiesInfoBySequence);
                utilitiesBySequence.clear();
                sequenceID += 1;
            }

        }

        // for (auto it = begin (utilities_info); it != end (utilities_info); ++it) {
        //     std::cout << std::endl;
        //     for (int row_idx = 0; row_idx < (it->utilitiesBySequence).size(); row_idx++) {
        //         for (int col_idx = 0; col_idx < (it->utilitiesBySequence)[row_idx].size(); col_idx++) {
        //             std::cout << (it->utilitiesBySequence)[row_idx][col_idx] << " ";
        //         }
        //         std::cout << std::endl;
        //     }
        // }

    }

    if (inRemainingUtilityFile.is_open()) {

        int sequenceID = 1;
        std::vector<std::vector<float>> remainingUtilityBySequence;

        while (std::getline(inRemainingUtilityFile, line)) {

            if (!std::isspace(line[0])) {

                boost::split(tokens, line, boost::is_any_of("\t"));
                std::vector<float> row;

                for (int i = 0; i < tokens.size(); i++) {
                    // std::cout << " " << tokens[i] << " ";
                    row.push_back(std::stof(tokens[i]));
                }
                // std::cout << std::endl;

                remainingUtilityBySequence.push_back(row);

            } else {
                RemainingUtilitiyInfoBySequence remainingUtilitiyInfoBySequence(sequenceID, remainingUtilityBySequence);
                remaining_utilities_info.push_back(remainingUtilitiyInfoBySequence);
                remainingUtilityBySequence.clear();
                sequenceID += 1;
            }

        }

        // for (auto it = begin (remaining_utilities_info); it != end (remaining_utilities_info); ++it) {
        //     std::cout << std::endl;
        //     for (int row_idx = 0; row_idx < (it->remainingUtilitiesBySequence).size(); row_idx++) {
        //         for (int col_idx = 0; col_idx < (it->remainingUtilitiesBySequence)[row_idx].size(); col_idx++) {
        //             std::cout << (it->remainingUtilitiesBySequence)[row_idx][col_idx] << " ";
        //         }
        //         std::cout << std::endl;
        //     }
        // }

    }

};

UtilityChainNode::UtilityChainNode(int sid, int tid, float acu, float ru) {
    this->sid = sid;
    this->tid = tid;
    this->acu = acu;
    this->ru = ru;
}

Pattern::Pattern(int extension_c) {
    this->extension_c = extension_c;
    this->peu = 0;
    this->utility = 0;
}
