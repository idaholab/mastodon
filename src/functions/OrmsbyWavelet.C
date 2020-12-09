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

#include "OrmsbyWavelet.h"

registerMooseObject("MastodonApp", OrmsbyWavelet);

InputParameters
OrmsbyWavelet::validParams()
{
  InputParameters params = Function::validParams();
  params.addRequiredParam<Real>("f1", "First frequency for defining the Ormsby wavelet.");
  params.addRequiredParam<Real>("f2", "Second frequency for defining the Ormsby wavelet.");
  params.addRequiredParam<Real>("f3", "Third frequency for defining the Ormsby wavelet.");
  params.addRequiredParam<Real>("f4", "Fourth frequency for defining the Ormsby wavelet.");
  params.addRequiredParam<Real>("ts", "Time of the peak of the Ormsby wavelet.");
  params.addParam<Real>("scale_factor", 1.0, "Amplitude scale factor to be applied to wavelet.");
  params.addClassDescription("Calculates an amplitude normalized Ormsby wavelet with the given input parameters.");
  return params;
}

OrmsbyWavelet::OrmsbyWavelet(const InputParameters & parameters)
  : Function(parameters),
    _scale_factor(getParam<Real>("scale_factor"))
{
}

Real
OrmsbyWavelet::value(Real t, const Point &) const
{
  Real f1 = getParam<Real>("f1");
  Real f2 = getParam<Real>("f2");
  Real f3 = getParam<Real>("f3");
  Real f4 = getParam<Real>("f4");
  Real ts = getParam<Real>("ts");

  Real c1, c2, c3, c4, c;

  c1 = libMesh::pi * f1 * f1 / (f2 - f1) * sinc(libMesh::pi * f1 * (t - ts)) * sinc(libMesh::pi * f1 * (t - ts));
  c2 = libMesh::pi * f2 * f2 / (f2 - f1) * sinc(libMesh::pi * f2 * (t - ts)) * sinc(libMesh::pi * f2 * (t - ts));
  c3 = libMesh::pi * f3 * f3 / (f3 - f4) * sinc(libMesh::pi * f3 * (t - ts)) * sinc(libMesh::pi * f3 * (t - ts));
  c4 = libMesh::pi * f4 * f4 / (f3 - f4) * sinc(libMesh::pi * f4 * (t - ts)) * sinc(libMesh::pi * f4 * (t - ts));

  // c is the max value
  c = (libMesh::pi * f4 * f4 / (f3 - f4) - libMesh::pi * f3 * f3 / (f3 - f4)) - (libMesh::pi * f2 * f2 / (f2 - f1) - libMesh::pi * f1 * f1 / (f2 - f1)) ;

  return _scale_factor / c * ((c4 - c3) - (c2 - c1));
}

// sinc function (tends to 1 as x -> 0)
inline Real
OrmsbyWavelet::sinc(Real x) const
{
  return (x == 0) ? 1.0 : sin(x)/x;
}
