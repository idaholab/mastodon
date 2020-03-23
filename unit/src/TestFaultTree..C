// MOOSE includes
#include "gtest/gtest.h"
#include "MooseUtils.h"
#include "Conversion.h"

// Boost distribution includes
#include "BoostDistribution.h"

// MASTODON includes
#include "MastodonUtils.h"

// QUANTIFICATIONUTILS includes
#include "FaultTree.h"

// For input
std::vector<FileName> file_lists_fault_tree;

TEST(FTAUtils, FaultTree)
{

    // ==================== Test the FaultTree object ===================

    // ---------------- Test error message for events input -------------------

    // ###### File Inputs ######

    // Note: 1. Correspond to the Line 31-33 in test_fault_tree.py
    try
    {
        file_lists_fault_tree = {"not_a_valid_filename.txt", ""};        

        FTAUtils::FaultTree(file_lists_fault_tree[0],
                            file_lists_fault_tree[1]);
    }
    catch (FTAUtils::CException e)
    {   
        std::cout << "------- IO Error -------" << std::endl;
        std::cout << e.msg << std::endl;
        std::cout << "does not exist." << std::endl;
        std::cout << std::endl;
        
    }    


    // ---------------- Test creating tree from file -------------------

    // Note: 1. Correspond to the Line 48-54 in test_fault_tree.py
    file_lists_fault_tree = {"logic2.txt", ""};

    FTAUtils::FaultTree(file_lists_fault_tree[0],
                        file_lists_fault_tree[1]);
    
    FTAUtils::FaultTree ft = FTAUtils::FaultTree(file_lists_fault_tree[0], 
                                                 file_lists_fault_tree[1]);
    std::set<std::set<std::string>> cut_sets = ft.getCutSets();
    ft.printSets();


    std::cout << "ddddddddddd" << std::endl;
}
