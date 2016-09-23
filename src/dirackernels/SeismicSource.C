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
#include "SeismicSource.h"
#include "Function.h"
#include "MooseMesh.h"

/**
 *  This class applies a force at the given point. The direction of the force is
 *  determined by the strike and dip angle of the fault and the slip direction (rake).
 *  The magnitude of the force is dependent on the seismic moment (shear_modulus*area*slip).
 *  Strike = 0 implies x axis is aligned with geographic North. If a 3-D model is not being used,
 *  please adjust the strike, rake and dip angles accordingly.
 *  Reference: Quantitative Seismology by Keiiti Aki and Paul G. Richards, Chapter 4.
 **/

template<>
InputParameters validParams<SeismicSource>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addClassDescription("This class applies a seismic source at a given point.");
  params.addRequiredParam<Real>("strike", "The strike angle of the fault in degrees.");
  params.addRequiredParam<Real>("dip", "The dip angle of the fault in degrees.");
  params.addRequiredParam<Real>("rake", "The rake angle of the fault in degrees.");
  params.addRequiredParam<Real>("area", "The area over which slip is distributed.");
  params.addRequiredParam<FunctionName>("slip", "The function describing slip as a function of time.");
  params.addRequiredParam<Real>("shear_modulus", "The shear modulus of the soil around the seismic source.");
  params.addParam<Real>("alpha", 0, "The Hilber Hughes Taylor (HHT) time integration parameter.");
  params.addRequiredParam<unsigned int>("component", "The direction in which the force is applied.");
  params.addParam<std::vector<Real> >("point", "The x,y,z coordinates of the source.");
  params.addParam<FunctionName>("x_position", "The function containing the x coordinates of the points. The first column gives source number (starting with 1) and second column gives x coordinate.");
  params.addParam<FunctionName>("y_position", "The function containing the y coordinates of the points. The first column gives source number (starting with 1) and second column gives y coordinate.");
  params.addParam<FunctionName>("z_position", "The function containing the z coordinates of the points. The first column gives source number (starting with 1) and second column gives z coordinate.");
  params.addParam<unsigned int>("number", "The number of source points.");
  return params;
}

SeismicSource::SeismicSource(const InputParameters & parameters) :
    DiracKernel(parameters),
    _component(getParam<unsigned int>("component")),
    _alpha(getParam<Real>("alpha")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _area(getParam<Real>("area")),
    _slip_function(&getFunction("slip")),
    _moment_magnitude(0.0),
    _force(0.0)
{
  if (!isParamValid("point") && !isParamValid("x_position"))
    mooseError("Error in " + name() + ". Either a point of a set of points should be given as input.");

  if (isParamValid("x_position") && !isParamValid("number"))
    mooseError("Error in " + name() + ". Please specify number of source points defined in the function.");

  if (_mesh.dimension() > 1)
  {
    if (isParamValid("x_position") && !isParamValid("y_position"))
       mooseError("Error in " + name() + ". The number of position functions should be equal to mesh dimension.");

    if (_mesh.dimension() > 2)
    {
      if (isParamValid("x_position") && !isParamValid("z_position"))
        mooseError("Error in " + name() + ". The number of position functions should be equal to mesh dimension.");
    }
  }

  if (_component >= _mesh.dimension())
    mooseError("Error in " + name() + ". component cannot exceed mesh dimensions.");

  if (_mesh.dimension() != 3)
    mooseWarning("Warning in " + name() + ". For accurate results, please ensure strike, rake and dip are interpreted correctly. By default, stike = 0 degrees implies x axis is aligned with geographic north.");

  // calulating seismic moment tensor
  Real pi = libMesh::pi;
  Real dip = getParam<Real>("dip") / 180.0 * pi;
  Real strike = getParam<Real>("strike") / 180.0 * pi;
  Real rake = getParam<Real>("rake") / 180.0 * pi;

  _moment.resize(3, std::vector<Real> (3, 0.0));
  _moment[0][0] = -(sin(dip) * cos(rake) * sin(2.0 * strike) + sin(2.0 * dip) * sin(rake) * sin(strike) * sin(strike));
  _moment[0][1] = (sin(dip) * cos(rake) * cos(2.0 * strike) + 0.5 * sin(2.0 * dip) * sin(rake) * sin(2.0 * strike));
  _moment[1][0] = _moment[0][1];
  _moment[0][2] = -(cos(dip) * cos(rake) * cos(strike) + cos(2.0 * dip) * sin(rake) * sin(strike));
  _moment[2][0] = _moment[0][2];
  _moment[1][1] = (sin(dip) * cos(rake) * sin(2.0 * strike) - sin(2.0 * dip) * sin(rake) * cos(strike) * cos(strike));
  _moment[1][2] = -(cos(dip) * cos(rake) * sin(strike) - cos(2.0 * dip) * sin(rake) * cos(strike));
  _moment[2][1] = _moment[1][2];
  _moment[2][2] = sin(2.0 * dip) * sin(rake);
}

void
SeismicSource::addPoints()
{
  if (isParamValid("point"))
  {
    std::vector<Real> point_param = getParam<std::vector<Real> >("point");
    _source_location(0) = point_param[0];
    if (point_param.size() > 1)
    {
      _source_location(1) = point_param[1];
      if (point_param.size() > 2)
        _source_location(2) = point_param[2];
    }
    addPoint(_source_location);
  }
  else if (isParamValid("x_position"))
  {
    unsigned int number = getParam<unsigned int>("number");

    for (unsigned int i = 0; i < number; ++i)
    {
      Function * const function_x = &getFunction("x_position");
      _source_location(0) = function_x->value(i+1, _qp);

      if (_mesh.dimension() > 1)
      {
        Function * const function_y = &getFunction("y_position");
        _source_location(1) = function_y->value(i+1, _qp);

        if (_mesh.dimension() > 2)
        {
          Function * const function_z = &getFunction("z_position");
          _source_location(2) = function_z->value(i+1, _qp);
        }
      }
      addPoint(_source_location, i);
    }
  }
}

Real
SeismicSource::computeQpResidual()
{
  /**
   *  f_i(x,t) = - d M_ij(x,t)/ d x_j (summation over index j)
   *  For a point source applied at p, M_ij(x,t) = M_ij(t)*delta(x-p)
   *  Therefore f_i(x,t) = - M_ij(t)*d delta(x-p)/ d x_j
   *  This in weak form simplifies to the volume integral of M_ij(t)* delta(x-p) * d test_i / d x_j
   **/

  _moment_magnitude = _shear_modulus * _area * _slip_function->value(_t + _alpha * _dt, _qp);

  _force = 0.0;
  for (unsigned int i = 0; i < _mesh.dimension(); i++)
    _force += _moment[_component][i] * _grad_test[_i][_qp](i);

  return -_force * _moment_magnitude;
}
