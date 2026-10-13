#include <CommonCauseFailuresUtils.h>

void
FTAUtils::CCF(
    std::string ccfList,
    std::string ccfParameters,
    std::string ccfFtDetails,
    std::string ftBasicProb,
    FTAUtils::MapVecString & ccfFtDetailsDict,
    std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe,
    std::vector<FTAUtils::VecVecString> & ftBasicProbVector)
{
  /*
   * Reading CCF Events File
   */

  FTAUtils::VecVecString ccfEventsVector = FTAUtils::generateVectorsFromFile(ccfList, "FORMAT_CSV");
  FTAUtils::MapInt ccfEventsNamesDict;

  // Creating CCF EVents Names Dictionary and checking for duplicate CCF Names in the CCF Events
  // File
  for (uint64_t row = 0; row < ccfEventsVector.size(); row++)
  {

    if (ccfEventsNamesDict[ccfEventsVector[row][0]] == 0)
    {
      ccfEventsNamesDict[ccfEventsVector[row][0]] = 1;
    }
    else
    {
      fprintf(stderr,
              "[ASSERT] In File: %s, Line: %d => Duplicate CCF name found in File %s.",
              __FILE__,
              __LINE__,
              ccfList.c_str());
      abort();
    }
  }

  /*
   * Generating CCF Events Dict
   */

  FTAUtils::MapVecString ccfEventsDict;

  for (uint64_t row = 0; row < ccfEventsVector.size(); row++)
  {

    FTAUtils::VecString ccfEventsRow;

    for (uint64_t column = 1; column < ccfEventsVector[row].size(); column++)
    {
      ccfEventsRow.push_back(ccfEventsVector[row][column]);
    }

    ccfEventsDict[ccfEventsVector[row][0]] = ccfEventsRow;
  }

  /*
   * Reading CCF Parameters (alpha) file
   */

  FTAUtils::VecVecString ccfParametersVector =
      FTAUtils::generateVectorsFromFile(ccfParameters, "FORMAT_CSV");
  FTAUtils::MapInt ccfParametersNamesDict;

  // Generating CCF Parameters Dictionary and cheking for duplicate CCF names in CCF Parameters File
  for (uint64_t row = 0; row < ccfParametersVector.size(); row++)
  {
    if (ccfParametersNamesDict[ccfParametersVector[row][0]] == 0)
    {
      ccfParametersNamesDict[ccfParametersVector[row][0]] = 1;
    }
    else
    {
      fprintf(stderr,
              "[ASSERT] In File: %s, Line: %d => Duplicate CCF name found in File %s.",
              __FILE__,
              __LINE__,
              ccfParameters.c_str());
      abort();
    }
  }

  /*
   * Generating CCF Events Dict
   */

  FTAUtils::MapVecString ccfParametersDict;

  for (uint64_t row = 0; row < ccfParametersVector.size(); row++)
  {

    FTAUtils::VecString ccfParametersRow;

    for (uint64_t column = 1; column < ccfParametersVector[row].size(); column++)
    {
      ccfParametersRow.push_back(ccfParametersVector[row][column]);
    }

    ccfParametersDict[ccfParametersVector[row][0]] = ccfParametersRow;
  }

  /*
   *   Checking that every CCF event has a corresponding parameters list,
   *   every CCF name found in parameter list is present in CCF File and
   *   there is a corresponding parameter for all CCF events.
   */

  for (FTAUtils::MapInt::iterator it = ccfEventsNamesDict.begin(); it != ccfEventsNamesDict.end();
       it++)
  {
    if (ccfParametersNamesDict[it->first] == 0)
    {
      fprintf(stderr, "%s present in Events file but not in Parameters file", it->first.c_str());
    }
  }

  for (FTAUtils::MapInt::iterator it = ccfParametersNamesDict.begin();
       it != ccfParametersNamesDict.end();
       it++)
  {
    if (ccfEventsNamesDict[it->first] == 0)
    {
      fprintf(stderr, "%s present in Parameters file but not in CCF file", it->first.c_str());
    }
  }

  for (FTAUtils::MapInt::iterator it = ccfEventsNamesDict.begin(); it != ccfEventsNamesDict.end();
       it++)
  {
    if (ccfParametersDict[it->first].size() != ccfEventsDict[it->first].size())
    {
      fprintf(stderr, "%s events and parameters size are not equal", it->first.c_str());
    }
  }

  /*
   * Reading CCF FT Details Parameters (alpha) file
   */

  FTAUtils::VecVecString ccfFtDetailsVector =
      FTAUtils::generateVectorsFromFile(ccfFtDetails, "FORMAT_CSV");

  /*
   * Generating CCF Ft Details Dictionary
   */

  for (uint64_t row = 0; row < ccfFtDetailsVector.size(); row++)
  {

    FTAUtils::VecString ccfFtDetailsRow;

    for (uint64_t column = 1; column < ccfFtDetailsVector[row].size(); column++)
    {
      ccfFtDetailsRow.push_back(ccfFtDetailsVector[row][column]);
    }

    ccfFtDetailsDict[ccfFtDetailsVector[row][0]] = ccfFtDetailsRow;
  }

  /*
   * Reading FT Basic Probability File
   */

  FTAUtils::VecVecString ftBasicProbVecVec =
      FTAUtils::generateVectorsFromFile(ftBasicProb, "FORMAT_CSV");

  for (uint64_t rowIndex = 0; rowIndex < ftBasicProbVecVec.size(); rowIndex++)
  {
    FTAUtils::VecVecString row;

    FTAUtils::VecString x{ftBasicProbVecVec[rowIndex][0]};
    row.push_back(x);

    FTAUtils::VecString y{FTAUtils::str2Upper(ftBasicProbVecVec[rowIndex][1], false)};
    row.push_back(y);

    FTAUtils::VecString values;
    for (uint64_t i = 2; i < ftBasicProbVecVec[rowIndex].size(); i++)
    {
      values.push_back(ftBasicProbVecVec[rowIndex][i]);
    }
    row.push_back(values);
    ftBasicProbVector.push_back(row);
  }

  FTAUtils::MapVecVecString newBeProbDict;

  createNewBasicEvents(newCorrespondingBe, ccfEventsDict, newBeProbDict, ccfParametersDict);

  updateCCFProb(newBeProbDict, ftBasicProbVector);
}

void
FTAUtils::CCF2(
    FTAUtils::VecVecString ftText,
    std::map<std::string, std::map<std::string, std::set<std::string>>> newCorrespondingBe,
    FTAUtils::MapVecString ccfFtDetailsDict,
    FTAUtils::VecVecString & ftMocusVector)
{

  FTAUtils::SetSetString ftCutSets = FTAUtils::FaultTree(ftText);
  std::string rootNode = ftText[0][0];

  ftMocusVector = FTAUtils::ftMocusList(rootNode, ftCutSets);

  int flag = 0;

  for (FTAUtils::MapVecString::iterator it = ccfFtDetailsDict.begin(); it != ccfFtDetailsDict.end();
       it++)
  {
    if (it->first == rootNode)
    {
      flag = 1;
      break;
    }
  }

  if (flag == 1)
  {
    addBeCCF(ftMocusVector, newCorrespondingBe, ccfFtDetailsDict);
  }
}

void
FTAUtils::addBeCCF(
    FTAUtils::VecVecString & ftMocusVector,
    std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe,
    FTAUtils::MapVecString & ccfFtDetailsDict)
{
  for (uint64_t i = 0; i < ccfFtDetailsDict[ftMocusVector[0][0]].size(); i++)
  {
    addBeCCFSupport(ftMocusVector,
                    newCorrespondingBe[ccfFtDetailsDict[ftMocusVector[0][0]][i]],
                    ccfFtDetailsDict[ftMocusVector[0][0]][i]);
  }
}

void
FTAUtils::addBeCCFSupport(FTAUtils::VecVecString & ftMocusVector,
                          std::map<std::string, std::set<std::string>> newCorrespondingBeForTheKey,
                          std::string key)
{
  uint64_t ftMocusVectorLength = ftMocusVector.size();
  FTAUtils::MapInt checklist;
  for (uint64_t i = 0; i < ftMocusVectorLength; i++)
  {
    for (uint64_t j = 2; j < ftMocusVector[i].size(); j++)
    {
      std::string element = ftMocusVector[i][j];

      uint64_t flag = 0;
      for (std::map<std::string, std::set<std::string>>::iterator it =
               newCorrespondingBeForTheKey.begin();
           it != newCorrespondingBeForTheKey.end();
           it++)
      {
        if (it->first == element)
        {
          flag = 1;
          break;
        }
      }

      if (flag == 1)
      {
        std::string new_element = element + "_" + key + "_Gate";
        ftMocusVector[i][j] = new_element;

        if (checklist[element] == 0)
        {
          FTAUtils::VecString temp;
          temp.push_back(new_element);
          temp.push_back("OR");

          FTAUtils::VecString temp2(newCorrespondingBeForTheKey[element].begin(),
                                    newCorrespondingBeForTheKey[element].end());
          for (uint64_t k = 0; k < temp2.size(); k++)
          {
            temp.push_back(temp2[k]);
          }

          ftMocusVector.push_back(temp);

          checklist[element] = 1;
        }
      }
    }
  }
}

void
FTAUtils::updateCCFProb(FTAUtils::MapVecVecString & newBeProbDict,
                        std::vector<FTAUtils::VecVecString> & ftBasicProbVector)
{
  FTAUtils::MapVecVecString ftBasicProbDict;
  FTAUtils::MapVecVecString pblsformat;
  FTAUtils::MapVecVecString newBeProbDict_copy = newBeProbDict;
  for (uint64_t i = 0; i < ftBasicProbVector.size(); i++)
  {
    FTAUtils::VecVecString ftBasicProbDictVal;
    ftBasicProbDictVal.push_back(ftBasicProbVector[i][1]);
    ftBasicProbDictVal.push_back(ftBasicProbVector[i][2]);

    ftBasicProbDict[ftBasicProbVector[i][0][0]] = ftBasicProbDictVal;
  }

  for (FTAUtils::MapVecVecString::iterator it = newBeProbDict.begin(); it != newBeProbDict.end();
       it++)
  {
    FTAUtils::VecVecString pblsformatVector;
    std::vector<std::string> temp;
    temp.push_back(it->first);
    pblsformatVector.push_back(temp);
    pblsformatVector.push_back(ftBasicProbDict[it->first][0]);
    pblsformatVector.push_back(ftBasicProbDict[it->first][1]);

    pblsformat[it->first] = pblsformatVector;

    for (uint64_t i = 0; i < newBeProbDict[it->first].size(); i++)
    {
      newBeProbDict_copy[it->first][i][1] =
          std::to_string(atof(newBeProbDict_copy[it->first][i][1].c_str()) *
                         atof(ftBasicProbDict[it->first][1][0].c_str()));
    }
  }

  std::vector<FTAUtils::VecVecString> pblsAll;

  for (FTAUtils::MapVecVecString::iterator it = newBeProbDict.begin(); it != newBeProbDict.end();
       it++)
  {
    for (uint64_t i = 0; i < newBeProbDict[it->first].size(); i++)
    {
      FTAUtils::VecVecString pblsformatVector_copy = pblsformat[it->first];

      std::vector<std::string> temp;
      temp.push_back(newBeProbDict_copy[it->first][i][0]);
      pblsformatVector_copy[0] = temp;
      pblsformatVector_copy[2][0] = newBeProbDict_copy[it->first][i][1];

      pblsAll.push_back(pblsformatVector_copy);
    }
  }

  for (uint64_t i = 0; i < ftBasicProbVector.size(); i++)
  {
    pblsAll.push_back(ftBasicProbVector[i]);
  }

  ftBasicProbVector = pblsAll;
}

void
FTAUtils::createNewBasicEvents(
    std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe,
    FTAUtils::MapVecString ccfEventsDict,
    FTAUtils::MapVecVecString & newBeProbDict,
    FTAUtils::MapVecString ccfParametersDict)
{

  for (std::map<std::string, std::vector<std::string>>::iterator it = ccfEventsDict.begin();
       it != ccfEventsDict.end();
       it++)
  {

    int totalBe = it->second.size();

    FTAUtils::VecString beCombinationsVector;

    combinations(beCombinationsVector,
                 it->second,
                 "",
                 0,
                 totalBe,
                 0,
                 it->first,
                 ccfEventsDict,
                 newCorrespondingBe);

    std::vector<double> beCombinationProb;

    for (uint64_t i = 0; i < totalBe; i++)
    {
      beCombinationProb.push_back(nCr(totalBe - 1, i) *
                                  atof(ccfParametersDict[it->first][i].c_str()));
    }

    FTAUtils::VecVecString newBeProbVector;

    for (uint64_t i = 0; i < beCombinationsVector.size(); i++)
    {
      std::vector<std::string> temp;
      temp.push_back(beCombinationsVector[i] + "_" + it->first);
      temp.push_back(std::to_string(beCombinationProb[beCombinationsVector[i].length() - 1 -
                                                      count(beCombinationsVector[i].begin(),
                                                            beCombinationsVector[i].end(),
                                                            '_')]));
      newBeProbVector.push_back(temp);
    }

    newBeProbDict[it->first] = newBeProbVector;
  }
}

// Utilities nCr and factorial

double
FTAUtils::nCr(int n, int r)
{
  return (factorial(n) / float(factorial(r) * factorial(n - r)));
}

int
FTAUtils::factorial(int num)
{
  int val = 1;

  for (uint64_t i = 1; i <= num; i++)
  {
    val = val * num;
  }

  return (val);
}

void
FTAUtils::combinations(
    FTAUtils::VecString & finalVector,
    FTAUtils::VecString & arr,
    std::string curString,
    int index,
    int arrSize,
    int curStringSize,
    std::string key,
    FTAUtils::MapVecString & ccfEventsDict,
    std::map<std::string, std::map<std::string, std::set<std::string>>> & newCorrespondingBe)
{
  if (arrSize == index)
  {
    if (curStringSize > 0)
    {
      std::string temp = "";

      for (uint64_t i = 0; i < curStringSize; i++)
      {
        if (curString[i] == '_')
        {

          if (find(ccfEventsDict[key].begin(), ccfEventsDict[key].end(), temp) !=
              ccfEventsDict[key].end())
          {
            newCorrespondingBe[key][temp].insert(curString + "_" + key);
          }

          temp = "";
        }
        else
        {
          temp = temp + curString[i];
        }
      }

      if (find(ccfEventsDict[key].begin(), ccfEventsDict[key].end(), temp) !=
          ccfEventsDict[key].end())
      {
        newCorrespondingBe[key][temp].insert(curString + "_" + key);
      }

      finalVector.push_back(curString);
    }
  }

  else
  {
    combinations(finalVector,
                 arr,
                 curString,
                 index + 1,
                 arrSize,
                 curStringSize,
                 key,
                 ccfEventsDict,
                 newCorrespondingBe);
    if (curString == "")
    {
      combinations(finalVector,
                   arr,
                   arr[index],
                   index + 1,
                   arrSize,
                   curStringSize + 1,
                   key,
                   ccfEventsDict,
                   newCorrespondingBe);
    }
    else
    {
      combinations(finalVector,
                   arr,
                   curString + "_" + arr[index],
                   index + 1,
                   arrSize,
                   curStringSize + 2,
                   key,
                   ccfEventsDict,
                   newCorrespondingBe);
    }
  }
}