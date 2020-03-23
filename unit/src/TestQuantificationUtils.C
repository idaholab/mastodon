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
std::vector<FileName> file_lists_quantification_utils;
std::vector<double> IM;
int nbins;
bool uncertainty;
int nsamp;
int seed;

// For output
std::map<std::string, std::vector<std::vector<double>>> params_double_1, 
                                                        params_double_2,
                                                        params_double_3,
                                                        params_double_4,
                                                        params_double_5,
                                                        params_double_6,
                                                        params_double_7,
                                                        params_double_8,
                                                        params_double_9;

std::map<std::string, std::vector<std::vector<std::string>>> params_string_1,
                                                             params_string_2,
                                                             params_string_3,
                                                             params_string_4,
                                                             params_string_5,
                                                             params_string_6,
                                                             params_string_7,
                                                             params_string_8,
                                                             params_string_9;

std::map<std::string, bool> params_bool_1,
                            params_bool_2,
                            params_bool_3,
                            params_bool_4,
                            params_bool_5,
                            params_bool_6,
                            params_bool_7,
                            params_bool_8,
                            params_bool_9;

std::map<std::string, FTAUtils::Quantification::_analysis_t> params_analysis_t_1,
                                                             params_analysis_t_2,
                                                             params_analysis_t_3,
                                                             params_analysis_t_4,
                                                             params_analysis_t_5,
                                                             params_analysis_t_6,
                                                             params_analysis_t_7,
                                                             params_analysis_t_8,
                                                             params_analysis_t_9;

std::map<std::string, int> params_int_1,
                           params_int_2,
                           params_int_3,
                           params_int_4,
                           params_int_5,
                           params_int_6,
                           params_int_7,
                           params_int_8,
                           params_int_9;


TEST(FTAUtils, Quantification)
{
    // ==================== Test the Quantification object ===================

    // ---------------------------- Test Inputs ------------------------------
    
    // ++++++++++++ Input values ++++++++++++

    // ###### File Inputs ######

    // Note: 1. Correspond to the Line 18-23 in test_quantification.py
    try
    {
        file_lists_quantification_utils = {"not_a_valid_filename.txt",
                                           "not_a_valid_filename.txt",
                                           "not_a_valid_filename.txt"};        
        IM = {0.1, 4};
        nbins = 15;

        FTAUtils::Quantification(params_double_1,
                                 params_string_1,
                                 params_int_1,
                                 params_bool_1,
                                 params_analysis_t_1,
                                 file_lists_quantification_utils[0],
                                 file_lists_quantification_utils[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists_quantification_utils[2],
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
        file_lists_quantification_utils = {"",
                                           "",
                                           ""};
        IM = {0.1, 4};
        nbins = 15;

        FTAUtils::Quantification(params_double_2,
                                 params_string_2,
                                 params_int_2,
                                 params_bool_2,
                                 params_analysis_t_2,
                                 file_lists_quantification_utils[0],
                                 file_lists_quantification_utils[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists_quantification_utils[2],
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
    
    
    // ###### Inputs for Fragility ######

    file_lists_quantification_utils = {"logic1.txt",
                                       "logic1_bas_events_LNORM.txt",
                                       "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    FTAUtils::Quantification(params_double_3,
                             params_string_3,
                             params_int_3,
                             params_bool_3,
                             params_analysis_t_3,
                             file_lists_quantification_utils[0],
                             file_lists_quantification_utils[1],
                             FTAUtils::Quantification::FRAGILITY,
                             file_lists_quantification_utils[2],
                             IM[0],
                             IM[1],
                             nbins);
    

    // >>>>>>>> logic Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 32-34 in test_quantification.py
    /*
    filename    | content            | type
    ------------------------------------------------------------
    logic1.txt  | [[TE,OR,IE3,IE4],  | vector<vector<string>>
                |  [IE4,OR,C4],      |
                |  [IE3,OR,C3,IE2],  |
                |  [IE2,AND,C2,IE1], | 
                |  [IE1,OR,C1]]      |

    ==== Line 48-73 in FaultTree.C ====
    void FTAUtils::FaultTree::buildTree(FTAUtils::Parser parser)
    {
    std::vector<std::string> line;
    while (true) {
        line = parser.yieldLine();

        ...
        
        // Stash name, operator
        _node *node = new _node(line[0], str2Operator(line[1]));

        ...

        }
    }

    For vector<string> line = [TE,OR,IE3,IE4],
    line[0] is TE,
    line[1] is OR.
    
    * The "OR/AND" with type of string is converted to "OR/AND" with type of _opDict 
    * by the translation function of "FTAUtils::FaultTree::str2Operator()",
    * which is shown below, it needn't be converted to 0/1 with type of integer.

    ==== Line 79-86 in Faulttree.C ====
    // Translates string to opeartor
    FTAUtils::FaultTree::_operator_t FTAUtils::FaultTree::str2Operator(std::string op)
    {
    std::string op_s = FTAUtils::str2Upper(op, true);
    ASSERT(_opDict.count(op_s) != 0, "Illegal Operator found: %s", op.c_str());

    return _opDict[op_s];
    }
    
    ==== Line 49-50 in FaultTree.h ====
    // Hash map for operators
    std::map<std::string, _operator_t> _opDict = {{"AND", AND}, {"OR", OR}};
    */

    std::vector<std::vector<std::string>> event_files_matrix3 {{"TE","OR","IE3","IE4"},
                                                               {"IE4","OR","C4"},
                                                               {"IE3","OR","C3","IE2"},
                                                               {"IE2","AND","C2","IE1"}, 
                                                               {"IE1","OR","C1"}};
    EXPECT_EQ(params_string_3["events_files"], event_files_matrix3);

    
    // >>>>>>>> Basic Events Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 35-38 in test_quantification.py
    /*
    * The function of "parser.yieldLines()" are called 
    * to read the data in the file "logic1_bas_events_LNORM.txt",
    * and returns the data with the type of "vector<vector<string>>".

    filename                     | content                | type
    --------------------------------------------------------------------------------
    logic1_bas_events_LNORM.txt  | [[C1,LNORM,1.88,0.5],  | vector<vector<string>>
                                 |  [C2,LNORM,3.78,0.79], |
                                 |  [C3,LNORM,2.33,0.76], |
                                 |  [C4,LNORM,3.66,0.45]] |    
    
    * For each line, when elements are used,
    * they will be separatedly converted to the needed type,
    * for example: 
    * "LNORM" is converted from the type of "string" to the type of "_dist_t" ("enum" type),
    * "1.88", "0.5" are converted from the type of "string" to the type of "double".
        '''
            // Line 267-269 in QuantificationUtils.C
            // Stash name, probability vector
            double b = line.size() > 3 ? stod(line[3]) : 0;
            _b_nodes[line[0]] = getProbVector(_str2dist[line[1]], stod(line[2]), b,
                                      n_sample, seed, intmes, analysis, uncert);
            line[1] is LNORM
            line[2] is 1.88
        '''

    * In test_quantification.py
    * The function of "__readEventsFile()" are called
    * to read the data in the file "logic1_bas_events_LNORM.txt",
    * and returns the data with the type of "[string, string, float, float]".

    ==== Line 503 in test_quantification.py ====
    # read basic events file
    self.__bas_events = self.__readEventsFile(basic_events)

    ==== Line 1113-1115 in FTA.py ====
    @staticmethod
    def __readEventsFile(fname):

        ...
            data.append([items[0].strip(),
                         items[1].strip().upper(),
                         [float(x.strip()) for x in items[2:]]])
        ...

        return data

    
    * Besause 
    * 1. the type of data in the file "logic1_bas_events_LNORM.txt" 
    *    read by function of "parser.yieldLines()" is "vector<vector<string>>", 
    *    and for each element, the types of the data 
    *    are separately converted to the needed type in the called functions,
    *    which is different from the fact that the returned type of data 
    *    by function of "__readEventsFile()" is "[string, string, float, float]" 
    *    in test_quantification.py
    * 2. there is no further operation after reading data with type of vector<vector<string>>.
    *    and before using data in other called functions,
    * 3. there is no type in c++ containing all types of string, enum, double
    *    which is as similar as "[string, string, float, float]" in python.
    * we needn't to check this input.
     */


    // >>>>>>>> antype Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 39 in test_quantification.py
    /*
    * It is not nesessary to check this input, 
    * because there is no further opeartion for this input value.    

    ==== Line 657-662 in FTA.py ==== 
    @property
    def antype(self):
        """
        Return the analysis type
        """
        return self.__antype    
    
    ==== Line 506 in FTA.py ==== 
    self.__antype = analysis
    */
    EXPECT_NE(params_analysis_t_3["analysis"], FTAUtils::Quantification::FRAGILITY);


    // >>>>>>>> hazard Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 40-41 in test_quantification.py
    std::vector<std::vector<double>> matrix_hazard {{0.0608, 0.01},
                                                    {0.2124, 0.001},
                                                    {0.4, 0.0001},
                                                    {0.629, 1e-05}, 
                                                    {0.9344, 1e-06}, 
                                                    {1.3055, 1e-07}};
    std::vector<std::vector<double>> hazard = params_double_3["hazard"];
    EXPECT_EQ(hazard, matrix_hazard);


    // >>>>>>>> imrange Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 42 in test_quantification.py
    /*
    * It is not nesessary to check this input, 
    * because there is no further opeartion for this input value.
    
    ==== Line 671-674 in FTA.py ==== 
    @property
    def imrang(self):
        """Return the Intensity Measure range"""
        return self.__imrang
    
    ==== Line 536-544 in FTA.py ==== 
    if IM is None:
        self.__imrang = [0.1, 4]
    # When provided, check that it is a list
    if not isinstance(IM, list):
        raise TypeError("The supplied items of IM range must be a list.")
    # If 2 elements are provided, assume that it is a range and
    # calculate the IMs and IMextents according to the number of bins
    if len(IM) == 2:
        self.__imrang = IM    
    */
    EXPECT_EQ(params_double_3["IM"][0][0], 0.1);
    EXPECT_EQ(params_double_3["IM"][0][1], 4);


    // >>>>>>>> nbins Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 43 in test_quantification.py
    /*
    * It is not nesessary to check this input, 
    * because there is no further opeartion for this input value.
    
    ==== Line 676-679 in FTA.py ==== 
    @property
    def nbins(self):
        """Return number of bins"""
        return self.__nbins
    
    ==== Line 546 in FTA.py ==== 
    self.__nbins = self.__parValue(nbins, 'nbins')

    ==== Line 1054-1070 in FTA.py ====
    @staticmethod
    def __parValue(value, name):
        """
        Function for raising input errors (nbins, nsamp, seed)
        """
        if name == 'seed':
            if value != None:
                if not isinstance(value, int):
                    raise TypeError("The supplied value of %s must be an integer." % (name))
                if (value < 0) or (value > 2**32-1):
                    raise ValueError("The supplied value of %s must be a +ve integer." % (name))
        else:
            if not isinstance(value, int):
                raise TypeError("The supplied value of %s must be an integer." % (name))
            if value <= 0:
                raise ValueError("The supplied value of %s must be a +ve integer." % (name))
        return value
    */
    EXPECT_EQ(params_int_3["n_bins"], 15);

    // >>>>>>>> IM range Type Check <<<<<<<<
    // Note: 1. Correspond to the Line 45-48 in test_quantification.py
    //       2. No check "The supplied items of IM range must be a list.");
    /*
    FTAUtils::Quantification::Quantification(
            std::map<std::string, std::vector<std::vector<double>>> & params,
            std::string events_file, 
            std::string events_prob_file,
            _analysis_t analysis, 
            std::string hazard_file,
            double im_lower,
            double im_upper,
            int n_bins,
            bool uncertainty, 
            std::string root, 
            int n_sample,
            int seed)
    */
    

    // >>>>>>>> nbins type Check <<<<<<<<
    // Note: 1. Correspond to the Line 50-53 in test_quantification.py
    //       2. No check "The supplied value of nbins must be an integer.");


    // >>>>>>>> nbins Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 55-59 in test_quantification.py
    try
    {
        file_lists_quantification_utils = {"logic1.txt",
                                           "logic1_bas_events_LNORM.txt",
                                           "hazard.txt"};
        IM = {0.1, 4};

        nbins = -15;
        if (!(nbins > 0))
            throw FTAUtils::CException(
                "[THROW] The supplied value of nbins must be a +ve integer.");
        
        FTAUtils::Quantification(params_double_4,
                                 params_string_4,
                                 params_int_4,
                                 params_bool_4,
                                 params_analysis_t_4,
                                 file_lists_quantification_utils[0],
                                 file_lists_quantification_utils[1],
                                 FTAUtils::Quantification::FRAGILITY,
                                 file_lists_quantification_utils[2],
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
    


    // ###### Inputs for Risk analysis (not fragility) ######

    file_lists_quantification_utils = {"logic2.txt",
                                       "logic2_bas_events_PE.txt"};
    FTAUtils::Quantification(params_double_5,
                             params_string_5,
                             params_int_5,
                             params_bool_5,
                             params_analysis_t_5,
                             file_lists_quantification_utils[0],
                             file_lists_quantification_utils[1],
                             FTAUtils::Quantification::RISK
                             );

    // >>>>>>>> logic Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 63-69 in test_quantification.py
    std::vector<std::vector<std::string>> event_files_matrix5 {{"TOP", "AND", "GATE1", "GATE2"},
                                                               {"GATE1", "OR", "FT-N/m-1", "FT-N/m-2", "FT-N/m-3"},
                                                               {"GATE2", "OR", "B1", "B3", "B4"},
                                                               {"GATE3", "OR", "B2", "B4"}, 
                                                               {"FT-N/m-1", "AND", "GATE3", "B3", "B5"},
                                                               {"FT-N/m-2", "AND", "GATE3", "B1"},
                                                               {"FT-N/m-3", "AND", "B3", "B5", "B1"}};

    EXPECT_EQ(params_string_5["events_files"], event_files_matrix5);

    // >>>>>>>> Basic Events Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 70-72 in test_quantification.py


    // >>>>>>>> antype Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 73 in test_quantification.py
    EXPECT_NE(params_analysis_t_5["analysis"], FTAUtils::Quantification::FRAGILITY);


    // >>>>>>>> uncertainty Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 74 in test_quantification.py
    EXPECT_EQ(params_bool_5["uncertainty"], false);


    // >>>>>>>> nsamp Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 75 in test_quantification.py
    EXPECT_EQ(params_int_5["nsamp"], 1);


    // >>>>>>>> seed Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 76 in test_quantification.py
    EXPECT_EQ(params_int_5["seed"], 0);


    
    // ###### Testing for input errors making sure parameters are input correctly ######

    // Note: 1. Correspond to the Line 79-84 in test_quantification.py
    file_lists_quantification_utils = {"logic2.txt",
                                       "logic2_bas_events_PE.txt",
                                       "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    uncertainty = true;
    nsamp = 1000;  
    seed = 436546754;

    FTAUtils::Quantification(params_double_6,
                             params_string_6,
                             params_int_6,
                             params_bool_6,
                             params_analysis_t_6,
                             file_lists_quantification_utils[0],
                             file_lists_quantification_utils[1],
                             FTAUtils::Quantification::RISK,
                             file_lists_quantification_utils[2],
                             IM[0],
                             IM[1],
                             nbins,
                             uncertainty,
                             "",
                             nsamp,
                             seed);

    // >>>>>>>> uncertainty Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 74 in test_quantification.py
    EXPECT_EQ(params_bool_6["uncertainty"], true);


    // >>>>>>>> nsamp Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 75 in test_quantification.py
    EXPECT_EQ(params_int_6["nsamp"], 1000);


    // >>>>>>>> seed Value Check <<<<<<<<
    // Note: 1. Correspond to the Line 76 in test_quantification.py
    EXPECT_EQ(params_int_6["seed"], 436546754);



    // ###### Testing for input type errors and value errors ######

    // >>>>>>>> nsamp Type Check <<<<<<<<
    // Note: 1. Correspond to the Line 87-90 in test_quantification.py


    // >>>>>>>> seed Type Check <<<<<<<<
    // Note: 1. Correspond to the Line 92-95 in test_quantification.py


    // >>>>>>>> nsamp Value Check <<<<<<<<    
    // Note: 1. Correspond to the Line 97-101 in test_quantification.py
    try{
        file_lists_quantification_utils = {"logic2.txt",
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

        FTAUtils::Quantification(params_double_7,
                                 params_string_7,
                                 params_int_7,
                                 params_bool_7,
                                 params_analysis_t_7,
                                 file_lists_quantification_utils[0],
                                 file_lists_quantification_utils[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists_quantification_utils[2],
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
        file_lists_quantification_utils = {"logic2.txt",
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

        FTAUtils::Quantification(params_double_8,
                                 params_string_8,
                                 params_int_8,
                                 params_bool_8,
                                 params_analysis_t_8,
                                 file_lists_quantification_utils[0],
                                 file_lists_quantification_utils[1],
                                 FTAUtils::Quantification::RISK,
                                 file_lists_quantification_utils[2],
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


    // ---------------------------- Test TOP Risk ------------------------------

    // ++++++++++++ Function for asserting FTA top event risk ++++++++++++


    // >>>>>>>> Check FTA top event risk <<<<<<<<
    file_lists_quantification_utils = {"logic2.txt",
                                       "logic2_bas_events_PE.txt",
                                       "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    FTAUtils::Quantification(params_double_9,
                             params_string_9,
                             params_int_9,
                             params_bool_9,
                             params_analysis_t_9,
                             file_lists_quantification_utils[0],
                             file_lists_quantification_utils[1],
                             FTAUtils::Quantification::RISK,
                             file_lists_quantification_utils[2],
                             IM[0],
                             IM[1],
                             nbins);

    // Note: 1. Correspond to the Line 118-127 in test_quantification.py
    //       2. Risk Value Check
    // >>>>>>>> min-max <<<<<<<<
    EXPECT_EQ(params_double_9["fta"][0][0], 0.000694024);


    // >>>>>>>> upper bound <<<<<<<<
    EXPECT_EQ(params_double_9["fta"][0][1], 0.000704853860316601);


    // >>>>>>>> rare event <<<<<<<<
    EXPECT_EQ(params_double_9["fta"][0][2], 0.0007050000000000001);


    // Note: 1. Correspond to the Line 129-158 in test_quantification.py
    //       2. IMratio Value Check
    // >>>>>>>> Fussell-Vesely for B1, B2, B3, B4, B5 <<<<<<<<
    EXPECT_EQ(params_double_9["fv"][0][0], 0.8723950251527652);
    EXPECT_EQ(params_double_9["fv"][1][0], 0.3263002630028771);
    EXPECT_EQ(params_double_9["fv"][2][0], 0.14896258066840812);
    EXPECT_EQ(params_double_9["fv"][3][0], 0.6525835012002473);
    EXPECT_EQ(params_double_9["fv"][4][0], 0.14896258066840812);


    // >>>>>>>> Risk Reduction Ratio for B1, B2, B3, B4, B5 <<<<<<<<
    EXPECT_EQ(params_double_9["rrr"][0][0], 7.831866196407231);
    EXPECT_EQ(params_double_9["rrr"][1][0], 1.4839994007665414);
    EXPECT_EQ(params_double_9["rrr"][2][0], 1.174913088939574);
    EXPECT_EQ(params_double_9["rrr"][3][0], 2.87706550337578);
    EXPECT_EQ(params_double_9["rrr"][4][0], 1.174913088939574);


    // >>>>>>>> Risk Increase Ratio for B1, B2, B3, B4, B5 <<<<<<<<
    EXPECT_EQ(params_double_9["rir"][0][0], 86.11110290839181);
    EXPECT_EQ(params_double_9["rir"][1][0], 16.960273261286435);
    EXPECT_EQ(params_double_9["rir"][2][0], 5.808754991114076);
    EXPECT_EQ(params_double_9["rir"][3][0], 16.637742224118053);
    EXPECT_EQ(params_double_9["rir"][4][0], 3.826894185637253);


    // Note: 1. Correspond to the Line 160-183 in test_quantification.py
    //       2. IMdiff Value Check
    // >>>>>>>> Risk Reduction Difference for B1, B2, B3, B4, B5 <<<<<<<<
    EXPECT_EQ(params_double_9["rri"][0][0], 0.0006148556603167155);
    EXPECT_EQ(params_double_9["rri"][1][0], 0.0002298847599567777);
    EXPECT_EQ(params_double_9["rri"][2][0], 0.000104933860316625);
    EXPECT_EQ(params_double_9["rri"][3][0], 0.000459863310226738);
    EXPECT_EQ(params_double_9["rri"][4][0], 0.000104933860316625);


    // >>>>>>>> Risk Increase Difference for B1, B2, B3, B4, B5 <<<<<<<<
    EXPECT_EQ(params_double_9["rii"][0][0], 0.05999088944078346);
    EXPECT_EQ(params_double_9["rii"][1][0], 0.011249660219925572);
    EXPECT_EQ(params_double_9["rii"][2][0], 0.0033894695188034785);
    EXPECT_EQ(params_double_9["rii"][3][0], 0.01102232297330552);
    EXPECT_EQ(params_double_9["rii"][4][0], 0.001992547279452972);


    // >>>>>>>> Birnbaum Difference for B1, B2, B3, B4, B5 <<<<<<<<
    EXPECT_EQ(params_double_9["bi"][0][0], 0.06060574510110017);
    EXPECT_EQ(params_double_9["bi"][1][0], 0.01147954497988235);
    EXPECT_EQ(params_double_9["bi"][2][0], 0.0034944033791201035);
    EXPECT_EQ(params_double_9["bi"][3][0], 0.011482186283532259);
    EXPECT_EQ(params_double_9["bi"][4][0], 0.002097481139769597);
}