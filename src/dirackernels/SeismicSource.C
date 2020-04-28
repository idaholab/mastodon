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
#include "Function.h"
#include "MooseMesh.h"
#include "SeismicSource.h"

/**
 *  This class applies a force at the given point. The direction of the force is
 *  determined by the strike and dip angle of the fault and the slip direction
 *(rake).
 *  The magnitude of the force is dependent on the seismic moment
 *(shear_modulus*area*slip).
 *  Strike = 0 implies x axis is aligned with geographic North. If a 3-D model
 *is not being used,
 *  please adjust the strike, rake and dip angles accordingly.
 *  Reference: Quantitative Seismology by Keiiti Aki and Paul G. Richards,
 *Chapter 4.
 **/

registerMooseObject("MastodonApp", SeismicSource);

InputParameters
SeismicSource::validParams()
{
  InputParameters params = DiracKernel::validParams();
  params.addClassDescription("This class applies a seismic source at a given point.");
  params += SeismicSource::commonParameters();

  params.addRequiredParam<unsigned int>("component",
                                        "The direction in which the force is applied.");
  return params;
}

InputParameters
SeismicSource::commonParameters()
{
  InputParameters params = emptyInputParameters();
  params.addRequiredParam<Real>("strike", "The strike angle of the fault in degrees.");
  params.addRequiredParam<Real>("dip", "The dip angle of the fault in degrees.");
  params.addRequiredParam<Real>("rake", "The rake angle of the fault in degrees.");
  params.addRequiredParam<Real>("area", "The area over which slip is distributed.");
  params.addRequiredParam<FunctionName>("slip",
                                        "The function describing slip as a function of time.");
  params.addRequiredParam<Real>("shear_modulus",
                                "The shear modulus of the soil around the seismic source.");
  params.addParam<Real>("alpha", 0, "The Hilber Hughes Taylor (HHT) time integration parameter.");
  params.addParam<std::vector<Real>>("point",
                                     "The x, y and z coordinate of a single point source.");
  params.addParam<std::vector<FunctionName>>(
      "position_function",
      "The vector of function names that describes the x, "
      "y and z coordinates of the source. Only the x position"
      "function is required for 1 dimensional problems, x and y"
      "positions are required for 2 dimensional problems and x, "
      "y and z positions are required for 3D problems. The first"
      "column in these functions gives source number (starting"
      "with 1) and the second column contains the coordinate.");
  params.addParam<unsigned int>("number",
                                "The number of point sources. This number should be same as "
                                "number of entries in the function files describing the "
                                "position of the point source.");
  params.addParam<Real>("rupture_speed",
                        "The speed at which the earthquake rupture propogates "
                        "through the fault (usually around 0.8 * shear wave "
                        "speed).");
  params.addParam<std::vector<Real>>("epicenter", "The x, y and z coordinates of the epicenter.");
  return params;
}

SeismicSource::SeismicSource(const InputParameters & parameters)
  : DiracKernel(parameters),
    _component(getParam<unsigned int>("component")),
    _alpha(getParam<Real>("alpha")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _area(getParam<Real>("area")),
    _slip_function(&getFunction("slip")),
    _moment_magnitude(0.0),
    _force(0.0),
    _rupture_speed(std::numeric_limits<unsigned int>::max()),
    _epicenter(_mesh.dimension(), 0.0)
{
  if (!isParamValid("point") && !isParamValid("position_function"))
    mooseError("Error in " + name() +
               ". Either a point of a set of points should be given as input.");

  if (isParamValid("position_function") && !isParamValid("number"))
    mooseError("Error in " + name() +
               ". Please specify number of source points defined in the function.");

  if (isParamValid("position_function"))
  {
    std::vector<FunctionName> pos_function =
        getParam<std::vector<FunctionName>>("position_function");
    if (pos_function.size() != _mesh.dimension())
      mooseError("Error in " + name() + ". The number of position functions "
                                        "should be equal to mesh dimension.");
  }

  if (_component >= _mesh.dimension())
    mooseError("Error in " + name() + ". component cannot exceed mesh dimensions.");

  // calulating seismic moment tensor
  Real pi = libMesh::pi;
  Real dip = getParam<Real>("dip") / 180.0 * pi;
  Real strike = getParam<Real>("strike") / 180.0 * pi;
  Real rake = getParam<Real>("rake") / 180.0 * pi;

  if (_mesh.dimension() == 3)
  {
    _moment.resize(3, std::vector<Real>(3, 0.0));
    _moment[0][0] = -(sin(dip) * cos(rake) * sin(2.0 * strike) +
                      sin(2.0 * dip) * sin(rake) * sin(strike) * sin(strike));
    _moment[0][1] = (sin(dip) * cos(rake) * cos(2.0 * strike) +
                     0.5 * sin(2.0 * dip) * sin(rake) * sin(2.0 * strike));
    _moment[1][0] = _moment[0][1];
    _moment[0][2] =
        -(cos(dip) * cos(rake) * cos(strike) + cos(2.0 * dip) * sin(rake) * sin(strike));
    _moment[2][0] = _moment[0][2];
    _moment[1][1] = (sin(dip) * cos(rake) * sin(2.0 * strike) -
                     sin(2.0 * dip) * sin(rake) * cos(strike) * cos(strike));
    _moment[1][2] =
        -(cos(dip) * cos(rake) * sin(strike) - cos(2.0 * dip) * sin(rake) * cos(strike));
    _moment[2][1] = _moment[1][2];
    _moment[2][2] = sin(2.0 * dip) * sin(rake);
  }
  else if (_mesh.dimension() == 2)
  {
    // x direction is aligned with north and y direction is aligned with vertical.
    // This will result in an in-plane earthquake wave.
    if (strike != 0.0)
      mooseError("Error in " + name() +
                 ". A non-zero strike angle for 2D models will create an "
                 "out-of-plane earthquake wave. This is currently not "
                 "supported.");
    _moment.resize(2, std::vector<Real>(2, 0.0));
    _moment[0][0] = (sin(dip) * cos(rake) * sin(2.0 * strike) -
                     sin(2.0 * dip) * sin(rake) * cos(strike) * cos(strike));
    _moment[0][1] =
        -(cos(dip) * cos(rake) * sin(strike) - cos(2.0 * dip) * sin(rake) * cos(strike));
    _moment[1][0] = _moment[0][1];
    _moment[1][1] = sin(2.0 * dip) * sin(rake);
  }
  else
    mooseError("Error in " + name() + ". Only mesh dimensions of 2 and 3 are currently supported.");

  if (isParamValid("rupture_speed") && isParamValid("epicenter"))
  {
    if (isParamValid("point"))
      mooseError("Error in " + name() +
                 ". Rupture speed and epicenter should only be provided "
                 "when multiple point sources are specified.");

    std::vector<Real> epi = getParam<std::vector<Real>>("epicenter");
    if (epi.size() != _mesh.dimension())
      mooseError("Error in " + name() + ". Epicenter should be same size as mesh dimension.");

    _rupture_speed = getParam<Real>("rupture_speed");
    _epicenter = getParam<std::vector<Real>>("epicenter");

    if (_rupture_speed <= 0.0)
      mooseError("Error in " + name() + ". Rupture speed has to be positive.");
  }
  else if ((isParamValid("rupture_speed") & !isParamValid("epicenter")) ||
           (!isParamValid("rupture_speed") & isParamValid("epicenter")))
    mooseError("Error in " + name() +
               ". Either both rupture speed and epicenter should be "
               "provided or neither should be provided.");
}

void
SeismicSource::addPoints()
{
  if (isParamValid("point"))
  {
    std::vector<Real> point_param = getParam<std::vector<Real>>("point");
    _source_location(0) = point_param[0];
    if (point_param.size() > 1)
    {
      _source_location(1) = point_param[1];
      if (point_param.size() > 2)
        _source_location(2) = point_param[2];
    }
    addPoint(_source_location);
  }
  else if (isParamValid("position_function"))
  {
    unsigned int number = getParam<unsigned int>("number");
    std::vector<FunctionName> position_function =
        getParam<std::vector<FunctionName>>("position_function");
    std::vector<const Function *> pos_function(_mesh.dimension());

    for (unsigned int i = 0; i < _mesh.dimension(); ++i)
      pos_function[i] = &getFunctionByName(position_function[i]);

    for (unsigned int i = 0; i < number; ++i)
    {
      for (unsigned int j = 0; j < _mesh.dimension(); ++j)
        _source_location(j) = (pos_function[j])->value(i + 1, _qp);

      addPoint(_source_location, i);
    }
  }
}

Real
SeismicSource::computeQpResidual()
{
  /**
   * The time at which each point source ruptures is given by time_shift.
   * The time_shift parameter is calculated based on rupture speed and the distance
   * of the source from the epicenter.
   **/

  Real time_shift = 0.0;
  if (_rupture_speed != std::numeric_limits<unsigned int>::max())
  {
    Real distance = 0.0;
    if (_mesh.dimension() == 1)
      distance = std::abs(_physical_point[_qp](0) - _epicenter[0]);
    else if (_mesh.dimension() == 2)
      distance = std::sqrt(pow(_physical_point[_qp](0) - _epicenter[0], 2.0) +
                           pow(_physical_point[_qp](1) - _epicenter[1], 2.0));
    else if (_mesh.dimension() == 3)
      distance = std::sqrt(pow(_physical_point[_qp](0) - _epicenter[0], 2.0) +
                           pow(_physical_point[_qp](1) - _epicenter[1], 2.0) +
                           pow(_physical_point[_qp](2) - _epicenter[2], 2.0));

    time_shift = distance / _rupture_speed;
  }

  /**
   *  f_i(x,t) = - d M_ij(x,t)/ d x_j (summation over index j)
   *  For a point source applied at p, M_ij(x,t) = M_ij(t)*delta(x-p)
   *  Therefore f_i(x,t) = - M_ij(t)*d delta(x-p)/ d x_j
   *  This in weak form simplifies to the volume integral of M_ij(t)* delta(x-p)
   ** d test_i / d x_j
   **/
  if (_t > time_shift)
    _moment_magnitude =
        _shear_modulus * _area * _slip_function->value(_t + _alpha * _dt - time_shift, _qp);
  else
    _moment_magnitude = 0.0;

  _force = 0.0;
  for (unsigned int i = 0; i < _mesh.dimension(); i++)
    _force += _moment[_component][i] * _grad_test[_i][_qp](i);

  return -_force * _moment_magnitude;
}
