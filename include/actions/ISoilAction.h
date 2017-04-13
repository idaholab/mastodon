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
 * This action sets up the I-Soil material model. The stress-strain backbone
 *curve can
 * either be provided by the user or can be automatically generated using the
 *Darendeli
 * GQ/H method. This stress-strain curve is then divided into multiple
 *elastic-perfectly
 * plastic stress-strain curves and the information corresponding to each
 *elastic-perfectly
 * plastic curve is passed as input to the ComputeISoilStress.C material model
 **/

#ifndef ISOILACTION_H
#define ISOILACTION_H

#include "Action.h"

class ISoilAction : public Action {
public:
  ISoilAction(const InputParameters &params);

  virtual void act();

private:
  /// Calculates the Youngs modulus and yiled stress for each elastic-perfectly
  /// plastic curve for each soil layer from the data stored in stress and
  /// strain vectors.
  void computeSoilLayerProperties(
      std::vector<std::vector<Real>> &stress,
      std::vector<std::vector<Real>> &strain,
      std::vector<unsigned int> &layer_ids,
      std::vector<std::vector<Real>> &E0_component,
      std::vector<std::vector<Real>> &scaled_yield0_component,
      std::vector<Real> &poissons_ratio);

  /// Reads the user provided data file and stores the stress and strain values
  /// for each soil layer in the vectors stress and strain, respectively.
  void computeUserDefinedBackbone(std::vector<std::vector<Real>> &stress,
                                  std::vector<std::vector<Real>> &strain,
                                  std::vector<unsigned int> &layer_ids);

  /// Populates the stress and strain vectors automatically using the Darendeli
  /// backbone curve formulation.
  void computeDarendeliBackBone(std::vector<std::vector<Real>> &stress,
                                std::vector<std::vector<Real>> &strain,
                                std::vector<unsigned int> &layer_ids);

  /// Populates the stress and strain vectors automatically using the GQ/H
  /// backbone curve formulation.
  void computeGQHBackbone(std::vector<std::vector<Real>> &stress,
                          std::vector<std::vector<Real>> &strain,
                          std::vector<unsigned int> &layer_ids);

  /// parseNextLineReals reads one line of a file and stores it a vector.
  bool parseNextLineReals(std::ifstream &ifs, std::vector<Real> &myvec);

  /// parseColumns reads a given csv file with 2 columns and stores the first
  /// column in the vector x and the second column in vector y.
  void parseColumns(std::vector<Real> &x, std::vector<Real> &y,
                    FileName &data_file);
};

template <> InputParameters validParams<ISoilAction>();

#endif // ISOILACTION_H
