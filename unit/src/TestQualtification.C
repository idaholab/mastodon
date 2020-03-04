#include "Quantification.h"

TEST(FTAUtils, Quantification::Quantification)
{
  
}

/*
in "quantification.cpp"
FTAUtils::Quantification::Quantification(std::string events_file, std::string events_prob_file,
                               _analysis_t analysis, std::string hazard_file,
                               double im_lower, double im_upper, int n_bins,
                               bool uncertainty, std::string root, int n_sample,
                               int seed)

in "Quantification.C"
Quantification q  = Quantification("inputs/SSI_logic.txt",
                                   "inputs/bas_events_ln.txt",
                                   Quantification::FRAGILITY,
                                   "inputs/hazard.txt",
                                   0.1,
                                   4,
                                   15);

in "inputs/SSI_logic.txt"
TE,OR,IE3,IE4
IE4,OR,C4
IE3,OR,C3,IE2
IE2,AND,C2,IE1
IE1,OR,C1

in "inputs/bas_events_ln.txt"
C1,LNORM,1.88,0.5
C2,LNORM,3.78,0.79
C3,LNORM,2.33,0.76
C4,LNORM,3.66,0.45

in "inputs/hazard.txt"
0.0608,1.00E-02
0.2124,1.00E-03
0.4,1.00E-04
0.629,1.00E-05
0.9344,1.00E-06
1.3055,1.00E-07

*/
