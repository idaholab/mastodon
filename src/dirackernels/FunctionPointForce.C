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
#include "FunctionPointForce.h"
#include "Function.h"
#include "MooseMesh.h"

/**
 * This class applies a force at the given point/points in a given direction.
 * The force can be given as a function of space and/or time.
 **/

template<>
InputParameters validParams<FunctionPointForce>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addClassDescription("This class applies a force at the given point/points in a given direction. The force can be given as a function of space and/or time.");
  params.addParam<Real>("alpha", 0.0, "The Hilber Hughes Taylor (HHT) time integration parameter.");
  params.addParam<std::vector<Real> >("point", "The x,y,z coordinates of the point.");
  params.addParam<FunctionName>("x_position", "The function containing the x coordinates of the points. The first column gives point number (starting with 1) and second column gives x coordinate.");
  params.addParam<FunctionName>("y_position", "The function containing the y coordinates of the points. The first column gives point number (starting with 1) and second column gives y coordinate.");
  params.addParam<FunctionName>("z_position", "The function containing the z coordinates of the points. The first column gives point number (starting with 1) and second column gives z coordinate.");
  params.addParam<unsigned int>("number", "The number of points");
  params.addRequiredParam<FunctionName>("function", "The function defining the force as a function of space and/or time");
  return params;
}

FunctionPointForce::FunctionPointForce(const InputParameters & parameters) :
    DiracKernel(parameters)
{
  if (!isParamValid("point") && !isParamValid("x_position"))
    mooseError("Error in " + name() + ". Either a point of a set of points should be given as input");

  if (isParamValid("x_position") && !isParamValid("number"))
    mooseError("Error in " + name() + ". Please specify number of source points defined in the function");

  if (_mesh.dimension() > 1)
  {
    if (isParamValid("x_position") && !isParamValid("y_position"))
       mooseError("Error in " + name() + ". The number of position functions should be equal to mesh dimension");

    if (_mesh.dimension() > 2)
      if (isParamValid("x_position") && !isParamValid("z_position"))
        mooseError("Error in " + name() + ". The number of position functions should be equal to mesh dimension");
  }
}

void
FunctionPointForce::addPoints()
{
  if (isParamValid("point"))
  {
    std::vector<Real> point_param = getParam<std::vector<Real> >("point");
    _p(0) = point_param[0];
    if (point_param.size() > 1)
    {
      _p(1) = point_param[1];
      if (point_param.size() > 2)
        _p(2) = point_param[2];
    }
    addPoint(_p,0);
  }
  else if (isParamValid("x_position"))
  {
    unsigned int number = getParam<unsigned int>("number");

    for (unsigned int i = 0; i < number; ++i)
    {
      Function * const function_x = &getFunction("x_position");
      _p(0) = function_x->value(i+1, _qp);

      if (_mesh.dimension() > 1)
      {
        Function * const function_y = &getFunction("y_position");
        _p(1) = function_y->value(i+1, _qp);

        if (_mesh.dimension() > 2)
        {
          Function * const function_z = &getFunction("z_position");
          _p(2) = function_z->value(i+1, _qp);
        }
      }
      addPoint(_p, i);
    }
  }
}

Real
FunctionPointForce::computeQpResidual()
{
  Function * const force_function = &getFunction("function");
  Real alpha = getParam<Real>("alpha");

  Real force = force_function->value(_t + alpha*_dt, _qp);

  return -_test[_i][_qp]*force;
}
