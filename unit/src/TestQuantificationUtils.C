// MOOSE includes
#include "gtest/gtest.h"
#include "MooseUtils.h"
#include "Conversion.h"

// Boost distribution includes
#include "BoostDistribution.h"

// MASTODON includes
#include "MastodonUtils.h"

// QUANTIFICATIONUTILS includes
#include "QuantificationUtils.h"

// For input
std::vector<FileName> file_lists;
std::vector<double> IM;
int nbins;
bool uncertainty;
int nsamp;
int seed;

// For output
std::vector<std::vector<std::vector<double>>> results; // size = [7, 5, 3]

TEST(FTAUtils, Quantification)
{
    // Test the Quantification object.

    /*
    ------------------------Test Inputs--------------------------
    */

    // Input values
    // File Inputs
    // Note: 1. We comment the Line 345 in FaultTree.C
    //       2. Correspond to the Line 18-23 in test_quantification.py
    try
    {
        file_lists = {"not_a_valid_filename.txt",
                      "not_a_valid_filename.txt",
                      "not_a_valid_filename.txt"};        
        IM = {0.1, 4};
        nbins = 15;

        FTAUtils::Quantification(results,
                                 file_lists[0],
                                 file_lists[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists[2],
                                 IM[0],
                                 IM[1],
                                 nbins);
    }
    catch (FTAUtils::CException e)
    {
        std::cout << "------- IO Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << "does not exist." << std::endl;
        std::cout << std::endl;
    }    
    
    // Note: 1. Correspond to the Line 25-27 in test_quantification.py
    try
    {
        file_lists = {"",
                      "",
                      ""};
        IM = {0.1, 4};
        nbins = 15;

        FTAUtils::Quantification(results,
                                 file_lists[0],
                                 file_lists[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists[2],
                                 IM[0],
                                 IM[1],
                                 nbins);
    }
    catch (FTAUtils::CException e)
    {
        std::cout << "------- Type Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << "must be a filename or a list." << std::endl;
        std::cout << std::endl;
    }
    
    
    // Inputs for Fragility
    // Note: 1. Correspond to the Line 29-43 in test_quantification.py
    /*
    file_lists = {"logic1.txt",
                  "logic1_bas_events_LNORM.txt",
                  "hazard.txt"};
    IM = {0.1, 4};
    FTAUtils::Quantification(file_lists[0],
                             file_lists[1],
                             FTAUtils::Quantification::FRAGILITY,
                             file_lists[2],
                             IM[0],
                             IM[1],
                             15);
                             */

    // Note: 1. Correspond to the Line 45-48 in test_quantification.py
    try
    {
        file_lists = {"logic1.txt",
                      "logic1_bas_events_LNORM.txt",
                      "hazard.txt"};
        
        IM = {0.1};
        if (IM.size() != 2)
            throw FTAUtils::CException(
                "[THROW] The supplied items of IM range must be a list.");
        
        nbins = 15;

        FTAUtils::Quantification(results,
                                 file_lists[0],
                                 file_lists[1],
                                 FTAUtils::Quantification::FRAGILITY,
                                 file_lists[2],
                                 IM[0],
                                 IM[1],
                                 nbins);
    }
    catch (FTAUtils::CException e)
    {
        std::cout << "------- Type Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << std::endl;
    }
    

    
    // Note: 1. Correspond to the Line 50-53 in test_quantification.py
    //       2. No check "The supplied value of nbins must be an integer");


    // Note: 1. Correspond to the Line 55-59 in test_quantification.py
    try
    {
        file_lists = {"logic1.txt",
                      "logic1_bas_events_LNORM.txt",
                      "hazard.txt"};
        IM = {0.1, 4};

        nbins = -15;
        if (!(nbins > 0))
            throw FTAUtils::CException(
                "[THROW] The supplied value of nbins must be a +ve integer.");
        
        FTAUtils::Quantification(results,
                                 file_lists[0],
                                 file_lists[1],
                                 FTAUtils::Quantification::FRAGILITY,
                                 file_lists[2],
                                 IM[0],
                                 IM[1],
                                 nbins);
    }
    catch (FTAUtils::CException e)
    {
        std::cout << "------- Value Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << std::endl;
    }
    

    /*
    // Inputs for Risk analysis (not fragility)
    // Note: 1. Correspond to the Line 62-76 in test_quantification.py
    FTAUtils::Quantification("logic2.txt",
                             "logic2_bas_events_PE.txt",
                             FTAUtils::Quantification::RISK,
                             "hazard.txt",
                             0.1,
                             4,
                             15);
    */
    
    /*
    // Testing for input errors making sure parameters are input correctly
    // Note: 1. Correspond to the Line 78-84 in test_quantification.py
    file_lists = {"logic2.txt",
                  "logic2_bas_events_PE.txt",
                  "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    uncertainty = true;
    nsamp = 1000;
    seed = 436546754;
    FTAUtils::Quantification(file_lists[0],
                             file_lists[1],
                             FTAUtils::Quantification::RISK,
                             file_lists[2],
                             IM[0],
                             IM[1],
                             nbins,
                             uncertainty,
                             "",
                             nsamp,
                             seed);
    */

    // Testing for input type errors and value errors
    // Note: 1. NoCheck for nsamp type
    //       2. Correspond to the Line 87-90 in test_quantification.py

    // Note: 1. Correspond to the Line 92-95 in test_quantification.py
    //       2. NoCheck for seed value
     
    // Note: 1. Correspond to the Line 97-101 in test_quantification.py
    //       2. Check nsamp value
    try{
        file_lists = {"logic2.txt",
                      "logic2_bas_events_PE.txt",
                      "hazard.txt"};
        IM = {0.1, 4};
        nbins = 15;
        uncertainty = true;

        nsamp = -10;
        if (!(nsamp > 0))
            throw FTAUtils::CException(
                "[THROW] The supplied value of nsamp must be a +ve integer.");
  
        seed = 42.0;

        FTAUtils::Quantification(results,
                                 file_lists[0],
                                 file_lists[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists[2],
                                 IM[0],
                                 IM[1],
                                 nbins,
                                 uncertainty,
                                 "",
                                 nsamp,
                                 seed);
    }
    catch (FTAUtils::CException e)
    {
        std::cout << "------- Value Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << std::endl;
    }

    // Note: 1. Correspond to the Line 103-107 in test_quantification.py
    //       2. Check seed value


    try{
        file_lists = {"logic2.txt",
                      "logic2_bas_events_PE.txt",
                      "hazard.txt"};
        IM = {0.1, 4};
        nbins = 15;
        uncertainty = true;
        nsamp = 10;

        seed = -42;
        if (!(seed > 0))
            throw FTAUtils::CException(
                "[THROW] The supplied value of seed must be a +ve integer.");

        FTAUtils::Quantification(results,
                                 file_lists[0],
                                 file_lists[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists[2],
                                 IM[0],
                                 IM[1],
                                 nbins,
                                 uncertainty,
                                 "",
                                 nsamp,
                                 seed);
    }
    catch (FTAUtils::CException e)
    {
        std::cout << "------- Value Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << std::endl;
    }

    /*
    ------------------------Test Outputs--------------------------
    */

    // Check FTA top event risk.
    file_lists = {"logic2.txt",
                  "logic2_bas_events_PE.txt",
                  "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    FTAUtils::Quantification(results,
                             file_lists[0],
                             file_lists[1],
                             FTAUtils::Quantification::RISK,
                             file_lists[2],
                             IM[0],
                             IM[1],
                             nbins);

    // Note: 1. Correspond to the Line 114-43 in test_quantification.py
    //       2. Risk Value Check
    /*
    std::cout << "\n------- Check FTA top event risk -------" << std::endl;
    std::cout << "1. Exact solution: " << fta[0] << std::endl;
    std::cout << "2. Upper Bound   : " << fta[1] << std::endl;
    std::cout << "3. Rare Event    : " << fta[2] << std::endl;
    std::cout << std::endl;
    */
    
    // min-max
    EXPECT_EQ(results[0][0][0], 0.000694024);
    // upper bound
    EXPECT_EQ(results[0][0][1], 0.000704853860316601);
    // rare event
    EXPECT_EQ(results[0][0][2], 0.0007050000000000001);

    // Note: 1. Correspond to the Line 115- in test_quantification.py
    // ...
}