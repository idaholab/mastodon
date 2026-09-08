#ifndef _CommonCauseFailures_H
#define _CommonCauseFailures_H

#include <EventTreeUtils.h>
#include "MastodonUtils.h"

namespace FTAUtils
{

void CCF(std::string ccfList,
         std::string ccfParameters,
         std::string ccfFtDetails,
         std::string ftBasicProb,
         MapVecString & ccfFtDetailsDict,
         std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe,
         std::vector<VecVecString> & ftBasicProbVector);
void combinations(
    VecString & finalVector,
    VecString & arr,
    std::string curString,
    int index,
    int arrSize,
    int curStringSize,
    std::string key,
    MapVecString & ccfEventsDict,
    std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe);
void createNewBasicEvents(
    std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe,
    MapVecString ccfEventsDict,
    MapVecVecString & newBeProbDict,
    MapVecString ccfParametersDict);
void updateCCFProb(FTAUtils::MapVecVecString & newBeProbDict,
                   std::vector<VecVecString> & ftBasicProbVector);
void CCF2(FTAUtils::VecVecString ftText,
          std::map<std::string, std::map<std::string, std::set<std::string>>> newCorrespondingBe,
          FTAUtils::MapVecString ccfFtDetailsDict,
          FTAUtils::VecVecString & ftMocusVector);
void
addBeCCF(FTAUtils::VecVecString & ftMocusVector,
         std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe,
         MapVecString & ccfFtDetailsDict);
void addBeCCFSupport(VecVecString & ftMocusVector,
                     std::map<std::string, std::set<std::string>> newCorrespondingBeForTheKey,
                     std::string key);

std::string str2Upper(const std::string & str_in, bool trim_input);
int factorial(int num);
double nCr(int n, int r);

} // namespace FTAUtils

#endif