#include <EventTreeUtils.h>
#include <QuantificationUtils.h>
void
FTAUtils::ETA(std::string teFtFile,
              std::string teSeqs,
              std::map<std::string, FTAUtils::MapVecVecDouble> & dictQuantificationParamsDouble,
              std::map<std::string, FTAUtils::MapAnalysis> & dictQuantificationParamsAnalysis,
              std::map<std::string, FTAUtils::MapInt> & dictQuantificationParamsInt,
              std::map<std::string, FTAUtils::MapVecVecString> & dictQuantificationParamsString,
              std::map<std::string, FTAUtils::MapBool> & dictQuantificationParamsBool,
              std::string probText,
              std::string ccfList,
              std::string ccfParameters,
              std::string ccfftDetail,
              std::string hazard,
              std::string analysis,
              double im_lower,
              double im_upper,
              int nbins,
              bool uncertainity,
              int nsamp,
              int seed)
{
  /*
   * Reading Top Event FaultTree file and putting it's contents
   * into eventsFM.
   */

  // eventsFM contains elements of TE_FTfile
  FTAUtils::VecVecString eventsFM;

  // populating eventsFM

  std::vector<std::string> line;

  eventsFM = FTAUtils::generateVectorsFromFile(teFtFile, "FORMAT_CSV");

  /*
   * Reading Top Event Sequences file and putting it's contents
   * into eventsSeq.
   */

  // eventsSeq
  std::vector<std::vector<std::vector<std::string>>> eventsSeq;
  FTAUtils::VecVecString eventsSeqVecVec = FTAUtils::generateVectorsFromFile(teSeqs, "FORMAT_CSV");
  for (uint64_t i = 0; i < eventsSeqVecVec.size(); i++)
  {
    line = eventsSeqVecVec[i];

    std::vector<std::string> seqName;
    std::vector<std::vector<std::string>> eventsSeqRow;

    seqName.push_back(line[0]);
    eventsSeqRow.push_back(seqName);

    std::vector<std::string> successEvents;
    std::vector<std::string> failureEvents;

    for (uint64_t i = 1; i < line.size(); i++)
    {
      std::string temp;

      if (line[i][0] == '_')
      {
        temp = line[i].substr(1);
        successEvents.push_back(temp);
      }
      else
      {
        temp = line[i];
        failureEvents.push_back(temp);
      }
    }

    eventsSeqRow.push_back(failureEvents);
    eventsSeqRow.push_back(successEvents);

    eventsSeq.push_back(eventsSeqRow);
  }

  FTAUtils::MapInt dictEventsName;

  FTAUtils::MapVecVecString dictMList;

  for (uint64_t i = 0; i < eventsFM.size(); i++)
  {
    dictMList[eventsFM[i][0]] =
        FTAUtils::ftMocusList(eventsFM[i][0], FTAUtils::FaultTree(eventsFM[i][1], eventsFM[i][0]));
    dictEventsName[eventsFM[i][0]] = 1;
  }

  FTAUtils::MapVecVecString dictSeq;

  for (uint64_t i = 0; i < eventsSeq.size(); i++)
  {
    FTAUtils::VecVecString temp;
    temp.push_back(eventsSeq[i][1]);
    temp.push_back(eventsSeq[i][2]);
    dictSeq[eventsSeq[i][0][0]] = temp;
  }

  FTAUtils::MapVecVecString dictFinal = FTAUtils::mocusSeq(dictSeq, dictMList);

  FTAUtils::developQuant(dictFinal,
                         dictQuantificationParamsDouble,
                         dictQuantificationParamsAnalysis,
                         dictQuantificationParamsInt,
                         dictQuantificationParamsString,
                         dictQuantificationParamsBool,
                         probText,
                         analysis,
                         hazard,
                         nbins,
                         im_lower,
                         im_upper,
                         uncertainity,
                         nsamp,
                         seed,
                         ccfList,
                         ccfParameters,
                         ccfftDetail);
}

FTAUtils::VecVecString
FTAUtils::ftMocusList(std::string key, FTAUtils::SetSetString cutSets)
{
  FTAUtils::VecVecString cutSetsVector;
  FTAUtils::VecVecString treeLogic;

  for (FTAUtils::SetSetString::iterator it = cutSets.begin(); it != cutSets.end(); it++)
  {
    std::vector<std::string> temp(it->begin(), it->end());
    cutSetsVector.push_back(temp);
  }

  uint64_t cutSetsVectorLength = cutSetsVector.size();
  std::string orGate = "OR";
  std::string andGate = "AND";
  if (cutSetsVectorLength == 1)
  {
    std::vector<std::string> row;
    row.push_back(key);

    if (cutSetsVector[0].size() == 1)
    {
      row.push_back(orGate);
      row.push_back(cutSetsVector[0][0]);
    }

    else
    {
      row.push_back(andGate);
      for (uint64_t i = 0; i < cutSetsVector[0].size(); i++)
      {
        row.push_back(cutSetsVector[0][i]);
      }
    }

    treeLogic.push_back(row);
  }

  else
  {
    std::map<std::string, std::vector<std::string>> gateConnectorDict;
    std::vector<std::string> row;
    row.push_back(key);
    row.push_back(orGate);
    int gateConnectorCount = 1;
    for (uint64_t i = 0; i < cutSetsVectorLength; i++)
    {
      if (cutSetsVector[i].size() == 1)
      {
        row.push_back(cutSetsVector[i][0]);
      }
      else
      {
        std::string temp = key + "_Gate" + std::to_string(gateConnectorCount);
        gateConnectorDict[temp] = cutSetsVector[i];
        row.push_back(temp);
        gateConnectorCount++;
      }
    }
    treeLogic.push_back(row);
    for (std::map<std::string, std::vector<std::string>>::iterator it = gateConnectorDict.begin();
         it != gateConnectorDict.end();
         it++)
    {
      std::vector<std::string> newRow;
      newRow.push_back(it->first);
      newRow.push_back(andGate);

      for (uint64_t i = 0; i < it->second.size(); i++)
      {
        newRow.push_back(it->second[i]);
      }

      treeLogic.push_back(newRow);
    }
  }

  return (treeLogic);
}

FTAUtils::MapVecVecString
FTAUtils::mocusSeq(FTAUtils::MapVecVecString dictSeq, FTAUtils::MapVecVecString dictMList)
{
  FTAUtils::MapVecVecString dictFinal;
  for (FTAUtils::MapVecVecString::iterator it = dictSeq.begin(); it != dictSeq.end(); it++)
  {
    FTAUtils::VecVecString failedSeq =
        FTAUtils::mocusSuccessFailed(dictSeq[it->first][0], "failed", dictMList);
    FTAUtils::VecVecString successSeq =
        FTAUtils::mocusSuccessFailed(dictSeq[it->first][1], "success", dictMList);
    FTAUtils::SetSetString failedCutSets = FTAUtils::FaultTree(failedSeq);
    FTAUtils::SetSetString successCutSets = FTAUtils::FaultTree(successSeq);

    FTAUtils::VecVecString cutSetsWithSubsetsRemoved =
        mocusRemoveSubsets(failedCutSets, successCutSets);
    FTAUtils::SetSetString cutSetsWithSubsetsRemovedSet;

    for (uint64_t i = 0; i < cutSetsWithSubsetsRemoved.size(); i++)
    {
      std::set<std::string> temp(cutSetsWithSubsetsRemoved[i].begin(),
                                 cutSetsWithSubsetsRemoved[i].end());
      cutSetsWithSubsetsRemovedSet.insert(temp);
    }

    dictFinal[it->first] = ftMocusList(it->first, cutSetsWithSubsetsRemovedSet);
  }

  return (dictFinal);
}

FTAUtils::VecVecString
FTAUtils::mocusSuccessFailed(std::vector<std::string> seq,
                             std::string flag,
                             FTAUtils::MapVecVecString dictMList)
{
  FTAUtils::VecVecString mocusSuccessFailedList;
  std::string orGate = "OR";
  std::string andGate = "AND";

  std::vector<std::string> row;

  if (flag == "success")
  {
    row.push_back("TOP");
    row.push_back(orGate);
  }
  else
  {
    row.push_back("TOP");
    row.push_back(andGate);
  }

  for (uint64_t i = 0; i < seq.size(); i++)
  {
    row.push_back(seq[i]);
  }

  mocusSuccessFailedList.push_back(row);

  for (uint64_t i = 0; i < seq.size(); i++)
  {
    for (uint64_t j = 0; j < dictMList[seq[i]].size(); j++)
    {
      mocusSuccessFailedList.push_back(dictMList[seq[i]][j]);
    }
  }

  return (mocusSuccessFailedList);
}

FTAUtils::VecVecString
FTAUtils::mocusRemoveSubsets(FTAUtils::SetSetString failedCutSets,
                             FTAUtils::SetSetString successCutSets)
{
  FTAUtils::VecVecString failedCutSetsVector;
  FTAUtils::VecVecString successCutSetsVector;
  std::vector<int> indexToBeRemoved;
  FTAUtils::VecVecString finalVector;

  for (FTAUtils::SetSetString::iterator it = failedCutSets.begin(); it != failedCutSets.end(); it++)
  {
    std::vector<std::string> temp(it->begin(), it->end());
    failedCutSetsVector.push_back(temp);
  }
  for (FTAUtils::SetSetString::iterator it = successCutSets.begin(); it != successCutSets.end();
       it++)
  {
    std::vector<std::string> temp(it->begin(), it->end());
    successCutSetsVector.push_back(temp);
  }

  for (uint64_t i = 0; i < successCutSetsVector.size(); i++)
  {
    std::sort(successCutSetsVector[i].begin(), successCutSetsVector[i].end());
  }

  for (uint64_t i = 0; i < failedCutSetsVector.size(); i++)
  {
    std::sort(failedCutSetsVector[i].begin(), failedCutSetsVector[i].end());
    for (uint64_t j = 0; j < successCutSetsVector.size(); j++)
    {
      bool isSubset = std::includes(failedCutSetsVector[i].begin(),
                                    failedCutSetsVector[i].end(),
                                    successCutSetsVector[j].begin(),
                                    successCutSetsVector[j].end());
      if (isSubset == true)
      {
        indexToBeRemoved.push_back(i);
        break;
      }
    }
  }

  if (indexToBeRemoved.size() == 0)
  {
    return (failedCutSetsVector);
  }
  else
  {
    int cnt = 0;
    int sizeOfRemoval = indexToBeRemoved.size();
    for (int i = 0; i < failedCutSetsVector.size(); i++)
    {
      if (cnt < sizeOfRemoval)
      {
        if (indexToBeRemoved[cnt] == i)
        {
          cnt++;
        }
        else
        {
          finalVector.push_back(failedCutSetsVector[i]);
        }
      }
      else
      {
        finalVector.push_back(failedCutSetsVector[i]);
      }
    }
  }

  return (finalVector);
}

void
FTAUtils::developQuant(
    FTAUtils::MapVecVecString dictFinal,
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
    std::string ccfftDetail)
{

  FTAUtils::MapVecString ccfFtDetailsDict; // corrfile
  std::map<std::string, std::map<std::string, std::set<std::string>>> newCorrespondingBe;
  std::vector<FTAUtils::VecVecString> ftBasicProbVector;

  FTAUtils::VecVecString newFtBasicProbVector;

  FTAUtils::_analysis_t analysisType;
  if (analysis == "FRAGILITY")
  {
    analysisType = FRAGILITY;
  }
  else
  {
    analysisType = RISK;
  }

  if (ccfList != "")
  {

    FTAUtils::CCF(ccfList,
                  ccfParameters,
                  ccfftDetail,
                  probText,
                  ccfFtDetailsDict,
                  newCorrespondingBe,
                  ftBasicProbVector);

    for (uint64_t i = 0; i < ftBasicProbVector.size(); i++)
    {
      FTAUtils::VecString tempRow;
      tempRow.push_back(ftBasicProbVector[i][0][0]);
      tempRow.push_back(ftBasicProbVector[i][1][0]);
      for (uint64_t j = 0; j < ftBasicProbVector[i][2].size(); j++)
      {
        tempRow.push_back(ftBasicProbVector[i][2][j]);
      }

      newFtBasicProbVector.push_back(tempRow);
    }
  }

  for (FTAUtils::MapVecVecString::iterator it = dictFinal.begin(); it != dictFinal.end(); it++)
  {
    if (false)
    {
    }
    else
    {
      if (ccfList != "")
      {

        FTAUtils::VecVecString ftMocusVector;
        FTAUtils::CCF2(it->second, newCorrespondingBe, ccfFtDetailsDict, ftMocusVector);

        FTAUtils::MapVecVecDouble params_double;
        FTAUtils::MapVecVecString params_string;
        FTAUtils::MapInt params_int;
        FTAUtils::MapBool params_bool;
        FTAUtils::MapAnalysis params_analysis_t;

        if (ftMocusVector.size() == 0)
        {
          FTAUtils::Quantification(params_double,
                                   params_string,
                                   params_int,
                                   params_bool,
                                   params_analysis_t,
                                   it->second,
                                   newFtBasicProbVector,
                                   analysisType,
                                   hazard,
                                   nbins,
                                   im_lower,
                                   im_upper,
                                   uncertainity,
                                   it->first,
                                   nsamp,
                                   seed);
        }
        else
        {
          FTAUtils::Quantification(params_double,
                                   params_string,
                                   params_int,
                                   params_bool,
                                   params_analysis_t,
                                   ftMocusVector,
                                   newFtBasicProbVector,
                                   analysisType,
                                   hazard,
                                   nbins,
                                   im_lower,
                                   im_upper,
                                   uncertainity,
                                   it->first,
                                   nsamp,
                                   seed);
        }
        dictQuantificationParamsAnalysis[it->first] = params_analysis_t;
        dictQuantificationParamsBool[it->first] = params_bool;
        dictQuantificationParamsDouble[it->first] = params_double;
        dictQuantificationParamsInt[it->first] = params_int;
        dictQuantificationParamsString[it->first] = params_string;
      }
      else
      {
        FTAUtils::MapVecVecDouble params_double;
        FTAUtils::MapVecVecString params_string;
        FTAUtils::MapInt params_int;
        FTAUtils::MapBool params_bool;
        FTAUtils::MapAnalysis params_analysis_t;

        FTAUtils::Quantification(params_double,
                                 params_string,
                                 params_int,
                                 params_bool,
                                 params_analysis_t,
                                 it->second,
                                 probText,
                                 analysisType,
                                 hazard,
                                 nbins,
                                 im_lower,
                                 im_upper,
                                 uncertainity,
                                 it->first,
                                 nsamp,
                                 seed);

        dictQuantificationParamsAnalysis[it->first] = params_analysis_t;
        dictQuantificationParamsBool[it->first] = params_bool;
        dictQuantificationParamsDouble[it->first] = params_double;
        dictQuantificationParamsInt[it->first] = params_int;
        dictQuantificationParamsString[it->first] = params_string;
      }
    }
  }
}
