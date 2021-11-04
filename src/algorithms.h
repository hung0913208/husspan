#pragma once

#include <iostream>
#include <set>
#include "utilities.h"

void computeSWUs(Data inputData, float* swu_list);
std::set<int> computeICandidate(Data inputData, Pattern pattern, float threshold);
std::set<int> computeSCandidate(Data inputData, Pattern pattern, float threshold);
std::vector<UtilityChain> constructUCForIExtention(Data inputData, Pattern currentPattern, int extension_c);
std::vector<UtilityChain> constructUCForSExtention(Data inputData, Pattern currentPattern, int extension_c);
float computePatternUtilityAndPEU(Pattern& pattern);
