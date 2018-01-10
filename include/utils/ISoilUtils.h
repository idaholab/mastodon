/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     MASTODON                  */
/*                                               */
/*    (c) 2015 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

// This code was implemented in colloboration with Ozgun Numanoglu
// (numanog2@illinois.edu) and Omar Baltaji (baltaji2@illinois.edu) from UIUC.

/**
 * This namespace contains the functions used for the calculations corresponding
 * to the I-Soil material model in ComputeISoilStress.
 **/

#ifndef ISOILUTILS_H
#define ISOILUTILS_H

// MOOSE includes
#include "GeneralVectorPostprocessor.h"

// Forward declarations
namespace ISoilUtils
{
/// Deconstructs the backbone curve into a set of elastic-perfectly-plastic
/// curves, each comprising a Young's modulus and yield stress.
void computeSoilLayerProperties(std::vector<std::vector<Real>> & youngs,
                                std::vector<std::vector<Real>> & yield_stress,
                                const std::vector<std::vector<Real>> & backbone_stress,
                                const std::vector<std::vector<Real>> & backbone_strain,
                                const std::vector<unsigned int> & layer_ids,
                                const std::vector<Real> & poissons_ratio,
                                const std::string & name);

/// Reads the user provided backbone curve data files and stores the stress
/// and strain values for each soil layer in the vectors stress and strain,
/// respectively.
void computeUserDefinedBackbone(std::vector<std::vector<Real>> & backbone_stress,
                                std::vector<std::vector<Real>> & backbone_strain,
                                const std::vector<unsigned int> & layer_ids,
                                const std::vector<FileName> & backbone_curve_files,
                                const std::string & name);

/// Calculates the backbone curve automatically using the Darendeli
/// formulation.
void computeDarendeliBackbone(std::vector<std::vector<Real>> & backbone_stress,
                              std::vector<std::vector<Real>> & backbone_strain,
                              const std::vector<unsigned int> & layer_ids,
                              const std::vector<Real> & initial_shear_modulus,
                              const std::vector<Real> & over_consolidation_ratio,
                              const std::vector<Real> & plasticity_index,
                              const std::vector<Real> & p_ref,
                              const unsigned int number_of_points,
                              const std::string & name);

/// Calculates the backbone curve automatically using the GQ/H
/// formulation.
void computeGQHBackbone(std::vector<std::vector<Real>> & backbone_stress,
                        std::vector<std::vector<Real>> & backbone_strain,
                        const std::vector<unsigned int> & layer_ids,
                        const std::vector<Real> & initial_shear_modulus,
                        const unsigned int & number_of_points,
                        const std::vector<Real> & theta_1,
                        const std::vector<Real> & theta_2,
                        const std::vector<Real> & theta_3,
                        const std::vector<Real> & theta_4,
                        const std::vector<Real> & theta_5,
                        const std::vector<Real> & taumax);

/// Calculates the backbone curve automatically for the thin-layer
/// to simulate a Coulomb-type behavior.
void computeCoulombBackbone(std::vector<std::vector<Real>> & backbone_stress,
                            std::vector<std::vector<Real>> & backbone_strain,
                            const std::vector<unsigned int> & layer_ids,
                            const std::vector<Real> & initial_shear_modulus,
                            const std::vector<Real> & friction_coefficient,
                            const std::vector<Real> & hardening_ratio,
                            const std::vector<Real> & p_ref,
                            const std::string & name);
}

#endif // ISOILUTILS_H
