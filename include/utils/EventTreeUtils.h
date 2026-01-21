#ifndef _EVENTTREE_H
#define _EVENTTREE_H

#include <FaultTreeUtils.h>
#include <MastodonUtils.h>
namespace FTAUtils
{

void ETA(std::string teFtFile,
         std::string teSeqs,
         std::map<std::string, FTAUtils::MapVecVecDouble> & dictQuantificationParamsDouble,
         std::map<std::string, FTAUtils::MapAnalysis> & dictQuantificationParamsAnalysis,
         std::map<std::string, FTAUtils::MapInt> & dictQuantificationParamsInt,
         std::map<std::string, FTAUtils::MapVecVecString> & dictQuantificationParamsString,
         std::map<std::string, FTAUtils::MapBool> & dictQuantificationParamsBool,
         std::string probText = "",
         std::string ccfList = "",
         std::string ccfParameters = "",
         std::string ccfftDetail = "",
         std::string hazard = "",
         std::string analysis = "RISK",
         double im_lower = 0.1,
         double im_upper = 4,
         int nbins = 15,
         bool uncertainity = false,
         int nsamp = 1,
         int seed = 0);
VecVecString mocusRemoveSubsets(SetSetString failedCutSets, SetSetString successCutSets);
VecVecString ftMocusList(std::string key, SetSetString cutSets);
VecVecString mocusSuccessFailed(std::vector<std::string> seq,
                                std::string flag,
                                FTAUtils::MapVecVecString dictMList);
MapVecVecString mocusSeq(MapVecVecString dictSeq, MapVecVecString dictMList);
// void developQuant(MapVecVecString dictFinal, std::string probText, std::string analysis,
// std::string hazard, int nbins, double im_lower, double im_upper, bool uncertainity, int nsamp,
// int seed);
void developQuant(FTAUtils::MapVecVecString dictFinal,
                  std::map<std::string, FTAUtils::MapVecVecDouble> & dictQuantificationParamsDouble,
                  std::map<std::string, FTAUtils::MapAnalysis> & dictQuantificationParamsAnalysis,
                  std::map<std::string, FTAUtils::MapInt> & dictQuantificationParamsInt,
                  std::map<std::string, FTAUtils::MapVecVecString> & dictQuantificationParamsString,
                  std::map<std::string, FTAUtils::MapBool> & dictQuantificationParamsBool,
                  std::string probText,
                  std::string analysis,
                  std::string hazard,
                  int nbins,
                  double im_lower,
                  double im_upper,
                  bool uncertainity,
                  int nsamp,
                  int seed,
                  std::string ccfList,
                  std::string ccfParameters,
                  std::string ccfftDetail);
} // namespace FTAUtils

#endif // _EVENTTREE_H