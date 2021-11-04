#pragma once

#include <iostream>
#include <set>
#include "utilities.h"

void computeSWUs(Data inputData, float* swu_list);
float computePEU(Pattern& pattern);
std::set<int> computeICandidate(Data inputData, Pattern pattern, float threshold);
std::set<int> computeSCandidate(Data inputData, Pattern pattern, float threshold);
int computeRSUForICandidate(Data inputData, Pattern pattern, int iCandidate);
int computeRSUForSCandidate(Data inputData, Pattern pattern, int iCandidate);
std::vector<UtilityChain> constructUCForIExtention(Data inputData, Pattern currentPattern, int extension_c);
std::vector<UtilityChain> constructUCForSExtention(Data inputData, Pattern currentPattern, int extension_c);
float computePatternUtility(Pattern pattern);
