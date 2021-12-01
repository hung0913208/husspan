#pragma once

#include <unordered_set>
#include "utils.h"

std::unordered_set<int> computeSCandidate(Data inputData, Pattern pattern, float THRESHOLD);
void constructUCForSExtention(Data inputData, Pattern currentPattern,  Pattern& extendedPattern);
