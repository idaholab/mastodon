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

// MASTODON includes
#include "ISoilUtils.h"
#include "MastodonUtils.h"

// MOOSE includes
#include "ColumnMajorMatrix.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "Factory.h"
#include "MooseEnum.h"
#include "DelimitedFileReader.h"

void
ISoilUtils::computeUserDefinedBackbone(std::vector<std::vector<Real>> & backbone_stress,
                                       std::vector<std::vector<Real>> & backbone_strain,
                                       const std::vector<unsigned int> & layer_ids,
                                       const std::vector<FileName> & backbone_curve_files,
                                       const std::string & name)
{
  backbone_strain.resize(layer_ids.size());
  backbone_stress.resize(layer_ids.size());
  for (std::size_t i = 0; i < layer_ids.size(); i++)
  {
    MooseUtils::DelimitedFileReader backbone_curve(backbone_curve_files[i]);
    backbone_curve.setHeaderFlag(MooseUtils::DelimitedFileReader::HeaderFlag::OFF);
    backbone_curve.read();
    backbone_strain[i] = backbone_curve.getData("column_0");
    backbone_stress[i] = backbone_curve.getData("column_1");
    if (backbone_strain[i][0] == 0 && backbone_stress[i][0] == 0)
      mooseError("In " + name +
                 ". Found 0, 0 in the stress-strain curve provided through the file " +
                 backbone_curve_files[i] +
                 ". Please make sure that the backbone curve does not start at 0, 0.");
  }
  if (!MastodonUtils::checkEqualSize(backbone_strain) ||
      !MastodonUtils::checkEqualSize(backbone_stress))
    mooseError("In " + name + ". Please make sure that the backbone curves of all layers in "
                              "backbone_curve_files have the same number of points.");
}

void
ISoilUtils::computeDarendeliBackbone(std::vector<std::vector<Real>> & backbone_stress,
                                     std::vector<std::vector<Real>> & backbone_strain,
                                     const std::vector<unsigned int> & layer_ids,
                                     const std::vector<Real> & initial_shear_modulus,
                                     const std::vector<Real> & over_consolidation_ratio,
                                     const std::vector<Real> & plasticity_index,
                                     const std::vector<Real> & p_ref,
                                     const unsigned int number_of_points,
                                     const std::string & name)
{
  backbone_strain.resize(layer_ids.size());
  backbone_stress.resize(layer_ids.size());
  Real phi1 = 0.0352;
  Real phi2 = 0.001;
  Real phi3 = 0.3246;
  Real phi4 = 0.3483;
  Real phi5 = 0.9190;
  for (std::size_t i = 0; i < layer_ids.size(); i++)
  {
    if (plasticity_index[i] < 0)
      mooseError("Error in " + name + ". plasticity_index should be >= 0.");
    backbone_strain[i].resize(number_of_points);
    backbone_stress[i].resize(number_of_points);
    Real ref_strain = (phi1 + phi2 * plasticity_index[i] * pow(over_consolidation_ratio[i], phi3)) *
                      pow(p_ref[i] * 0.00987, phi4);
    for (std::size_t j = 0; j < number_of_points; j++)
    {
      backbone_strain[i][j] = pow(10.0, -6.0 + 5.0 / (number_of_points - 1) * j);
      backbone_stress[i][j] =
          initial_shear_modulus[i] *
          (1.0 / (1.0 + pow(100.0 * backbone_strain[i][j] / ref_strain, phi5))) *
          backbone_strain[i][j];
    }
  }
}

void
ISoilUtils::computeGQHBackbone(std::vector<std::vector<Real>> & backbone_stress,
                               std::vector<std::vector<Real>> & backbone_strain,
                               const std::vector<unsigned int> & layer_ids,
                               const std::vector<Real> & initial_shear_modulus,
                               const unsigned int & number_of_points,
                               const std::vector<Real> & theta_1,
                               const std::vector<Real> & theta_2,
                               const std::vector<Real> & theta_3,
                               const std::vector<Real> & theta_4,
                               const std::vector<Real> & theta_5,
                               const std::vector<Real> & taumax)
{
  backbone_strain.resize(layer_ids.size());
  backbone_stress.resize(layer_ids.size());
  std::vector<std::vector<Real>> modulus(layer_ids.size());
  for (std::size_t i = 0; i < layer_ids.size(); i++)
  {
    backbone_strain[i].resize(number_of_points);
    backbone_stress[i].resize(number_of_points);
    modulus[i].resize(number_of_points);
    for (std::size_t j = 0; j < number_of_points; ++j)
    {
      backbone_strain[i][j] = pow(10.0, -6.0 + 5.0 / (number_of_points - 1) * j);
      Real theta =
          theta_1[i] +
          theta_2[i] * theta_4[i] *
              pow(backbone_strain[i][j] / (taumax[i] / initial_shear_modulus[i]), theta_5[i]) /
              (pow(theta_3[i], theta_5[i]) +
               pow(backbone_strain[i][j] / (taumax[i] / initial_shear_modulus[i]), theta_5[i]));
      modulus[i][j] =
          (theta == 0)
              ? 1.0 / (1.0 + backbone_strain[i][j] / (taumax[i] / initial_shear_modulus[i]))
              : (1.0 / (backbone_strain[i][j] / (taumax[i] / initial_shear_modulus[i]))) *
                    (0.5 / theta *
                     (1.0 + backbone_strain[i][j] / (taumax[i] / initial_shear_modulus[i]) -
                      sqrt(pow(1.0 + backbone_strain[i][j] / (taumax[i] / initial_shear_modulus[i]),
                               2.0) -
                           4.0 * theta * backbone_strain[i][j] /
                               (taumax[i] / initial_shear_modulus[i]))));
      backbone_stress[i][j] = modulus[i][j] * initial_shear_modulus[i] * backbone_strain[i][j];
    }
  }
}

void
ISoilUtils::computeCoulombBackbone(std::vector<std::vector<Real>> & backbone_stress,
                                   std::vector<std::vector<Real>> & backbone_strain,
                                   const std::vector<unsigned int> & layer_ids,
                                   const std::vector<Real> & initial_shear_modulus,
                                   const std::vector<Real> & friction_coefficient,
                                   const std::vector<Real> & hardening_ratio,
                                   const std::vector<Real> & p_ref,
                                   const std::string & name)
{
  backbone_strain.resize(layer_ids.size());
  backbone_stress.resize(layer_ids.size());
  for (std::size_t i = 0; i < layer_ids.size(); i++)
  {
    backbone_strain[i].resize(2);
    backbone_stress[i].resize(2);
    if (hardening_ratio[i] >= 1.0)
      mooseError("Error in " + name + ". hardening_ratio cannot be greater than or equal to 1.0.");
    backbone_strain[i][0] = p_ref[i] * friction_coefficient[i] / initial_shear_modulus[i];
    backbone_stress[i][0] = p_ref[i] * friction_coefficient[i];
    backbone_strain[i][1] = 100.0;
    backbone_stress[i][1] = backbone_stress[i][0] +
                            (backbone_strain[i][1] - backbone_strain[i][0]) * hardening_ratio[i] *
                                initial_shear_modulus[i];
  }
}

void
ISoilUtils::computeSoilLayerProperties(
    std::vector<std::vector<Real>> & youngs,
    std::vector<std::vector<Real>> & yield_stress, // *** YIELD STRAIN, NOT YIELD STRESS***
    const std::vector<std::vector<Real>> & backbone_stress,
    const std::vector<std::vector<Real>> & backbone_strain,
    const std::vector<unsigned int> & layer_ids,
    const std::vector<Real> & poissons_ratio,
    const std::string & name)
{
  for (std::size_t k = 0; k < layer_ids.size(); k++)
  {
    Real number = backbone_strain[k].size();

    // Calculating the Youngs modulus for each of the n curves for soil layer k
    ColumnMajorMatrix A(number, number);
    ColumnMajorMatrix InvA(number, number);

    for (std::size_t i = 0; i < number; i++)
    {
      for (std::size_t j = 0; j < number; j++)
      {
        if (i <= j)
          A(i, j) = backbone_strain[k][i];
        else
          A(i, j) = backbone_strain[k][j];
      }
    }

    // create upper triangular matrix and modified stress
    youngs[k].resize(number);
    std::vector<Real> G0_component(number);
    yield_stress[k].resize(number);
    std::vector<Real> modified_backbone_stress(number);
    InvA = A;
    for (std::size_t i = 1; i < number; i++)
    {
      for (std::size_t j = 0; j < number; j++)
      {
        InvA(i, j) = A(i, j) - A(i - 1, j);
        modified_backbone_stress[i] = backbone_stress[k][i] - backbone_stress[k][i - 1];
      }
    }
    modified_backbone_stress[0] = backbone_stress[k][0];

    // backward substitution
    G0_component[number - 1] = modified_backbone_stress[number - 1] / InvA(number - 1, number - 1);
    yield_stress[k][number - 1] = backbone_strain[k][number - 1];
    int i = number - 2;
    while (i >= 0)
    {
      G0_component[i] = 0.0;
      for (std::size_t j = i + 1; j < number; j++)
        G0_component[i] += InvA(i, j) * G0_component[j];

      G0_component[i] = (modified_backbone_stress[i] - G0_component[i]) / InvA(i, i);
      yield_stress[k][i] = backbone_strain[k][i];
      i = i - 1;
    }

    // scaling
    for (std::size_t i = 0; i < number; i++)
    {
      youngs[k][i] = G0_component[i] * 2.0 * (1.0 + poissons_ratio[k]);
      yield_stress[k][i] = yield_stress[k][i] * std::sqrt(3.0) / (2.0 * (1.0 + poissons_ratio[k]));
    }
  }
  for (std::size_t k = 0; k < layer_ids.size(); k++)
  {
    for (std::size_t j = 0; j < backbone_strain[k].size(); j++)
    {
      if (youngs[k][j] == 0)
        mooseError("In " + name + "Found two segments in the backbone curve with the same slope. ",
                   "Please recheck input backbone curves.");
    }
  }
}
