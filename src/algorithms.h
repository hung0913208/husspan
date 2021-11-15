#pragma once

#include <iostream>
#include <set>
#include "utilities.h"

void computeSWUs(Data inputData, float* swu_list);
std::unordered_set<int> computeICandidate(Data inputData, Pattern pattern, float threshold);
std::unordered_set<int> computeSCandidate(Data inputData, Pattern pattern, float threshold);
std::vector<std::shared_ptr<UtilityChain>> constructUCForIExtention(Data inputData, Pattern currentPattern, int extension_c);
std::vector<std::shared_ptr<UtilityChain>> constructUCForSExtention(Data inputData, Pattern currentPattern, int extension_c);
void computePatternUtilityAndPEU(Pattern& pattern);
