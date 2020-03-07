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

TEST(Quantification, Quantification)
{
    // from QuantificationUtils.C
    // Quantification(std::string events_file, std::string events_prob_file,
    //              _analysis_t analysis = FRAGILITY, std::string hazard_file = "",
    //              double im_lower = 0.1, double im_upper = 4, int n_bins = 15,
    //              bool uncertainty = false, std::string root = "", int n_sample = 1,
    //              int seed = 0);

    // from main.cpp
    // Quantification q  = Quantification("inputs/SSI_logic.txt",
    //                                   "inputs/bas_events_ln.txt",
    //                                   Quantification::FRAGILITY,
    //                                   "inputs/hazard.txt",
    //                                   0.1,
    //                                   4,
    //                                   15);

    // from test_quantification.py
    // # Inputs for Fragility
    // inputs = Quantification('test', 'logic1.txt', 'logic1_bas_events_LNORM.txt', analysis='Fragility',
    //                             hazard='hazard.txt', IM=[0.1, 4], nbins=15)

    // Inputs for testing
    // Inputs for Fragility

    // Inputs for Risk analysis (not fragility)

    // outputs for testing
    Quantification FTAFragility, FTATopRisk;
    // outputs for Fragility
    // FTAFragility  = Quantification(events_file = "../logic1.csv",
    //                         events_prob_file = "../logic1_bas_events_LNORM.txt",
    //                         analysis = Quantification::FRAGILITY,
    //                         hazard_file = "../hazard.txt",
    //                         im_lower = 0.1,
    //                         im_upper = 4,
    //                         n_bins = 15);

    // outputs for Risk analysis (not fragility)
    // for asserting FTA top event risk.
    FTATopRisk  = Quantification("../logic2.csv",
                            "../logic2_bas_events_PE.txt",
                            analysis = Quantification::RISK,
                            im_lower = 0.1,
                            im_upper = 4,
                            n_bins = 15);    
    

    // Value check
    
}