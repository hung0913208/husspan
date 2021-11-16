#pragma once

#include <iostream>
#include <set>
#include "utilities.h"

void computeSWUs(Data inputData, float* swu_list);
std::set<int> computeICandidate(Data inputData, Pattern pattern, float threshold);
std::set<int> computeSCandidate(Data inputData, Pattern pattern, float threshold);
std::vector<std::shared_ptr<UtilityChain>> constructUCForIExtention(Data inputData, Pattern currentPattern, int extension_c);
void constructUCForSExtention(Data inputData, Pattern currentPattern,  Pattern& extendedPattern);
void computePatternUtilityAndPEU(Pattern& pattern);
