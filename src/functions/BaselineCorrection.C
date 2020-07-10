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

// This code was implemented in collaboration with Christopher J. Wong
// (chris.wong@utah.edu) from the University of Utah.

// MASTODON includes
#include "BaselineCorrection.h"
#include "BaselineCorrectionUtils.h"

// MOOSE includes
#include "DelimitedFileReader.h"

registerMooseObject("MastodonApp", BaselineCorrection);

InputParameters
BaselineCorrection::validParams()
{
  InputParameters params = Function::validParams();
  params.addClassDescription("Applies a baseline correction to an accceleration time history "
                             "using least squares polynomial fits and outputs the adjusted "
                             "acceleration with linear interpolation.");

  params.addParam<FileName>(
      "data_file", "The name of a CSV file containing raw acceleration time history data.");
  params.addParam<std::string>(
      "time_name",
      "The header name of the column which contains the time values in the data file. If not "
      "specified, they are assumed to be in the first column index.");
  params.addParam<std::string>(
      "acceleration_name",
      "The header name for the column which contains the acceleration values in the data file. If "
      "not specified, they are assumed to be in the second column index.");

  params.addParam<std::vector<Real>>("time_values", "The time abscissa values.");
  params.addParam<std::vector<Real>>("acceleration_values", "The acceleration ordinate values.");

  params.addRequiredParam<Real>("gamma", "The gamma parameter for Newmark time integration.");
  params.addRequiredParam<Real>("beta", "The beta parameter for Newmark time integration.");

  params.addRangeCheckedParam<unsigned int>(
      "accel_fit_order",
      "(0 <= accel_fit_order) & (accel_fit_order < 10)",
      "If this is provided, the acceleration time history will be adjusted using an nth-order "
      "polynomial fit of the nominal acceleration data, where n = accel_fit_order (only integer "
      "values from 0 to 9 are supported).");
  params.addRangeCheckedParam<unsigned int>(
      "vel_fit_order",
      "(0 <= vel_fit_order) & (vel_fit_order < 10)",
      "If this is provided, the acceleration time history will be adjusted using an nth-order "
      "polynomial fit of the nominal velocity data, where n = vel_fit_order (only integer values "
      "from 0 to 9 are supported).");
  params.addRangeCheckedParam<unsigned int>(
      "disp_fit_order",
      "(0 <= disp_fit_order) & (disp_fit_order < 10)",
      "If this is provided, the acceleration time history will be adjusted using an nth-order "
      "polynomial fit of the nominal displacement data, where n = disp_fit_order (only integer "
      "values from 0 to 9 are supported).");

  params.addParam<Real>("scale_factor",
                        1.0,
                        "A scale factor to be applied to the adjusted acceleration time history.");
  params.declareControllable("scale_factor");

  return params;
}

BaselineCorrection::BaselineCorrection(const InputParameters & parameters)
  : Function(parameters),
    _gamma(getParam<Real>("gamma")),
    _beta(getParam<Real>("beta")),
    _scale_factor(getParam<Real>("scale_factor"))
{
  // determine data source and check parameter consistency
  if (isParamValid("data_file") && !isParamValid("time_values") &&
      !isParamValid("acceleration_values"))
    buildFromFile();
  else if (!isParamValid("data_file") && isParamValid("time_values") &&
           isParamValid("acceleration_values"))
    buildFromXandY();
  else
    mooseError("In BaselineCorrection ",
               _name,
               ": Either `data_file` or `time_values` and `acceleration_values` must be specified "
               "exclusively.");

  // size checks
  if (_time.size() != _accel.size())
    mooseError("In BaselineCorrection ",
               _name,
               ": The length of time and acceleration data must be equal.");
  if (_time.size() == 0 || _accel.size() == 0)
    mooseError(
        "In BaselineCorrection ", _name, ": The length of time and acceleration data must be > 0.");

  // check that at lease one least squares fit will be applied
  if (!isParamValid("accel_fit_order") && !isParamValid("vel_fit_order") &&
      !isParamValid("disp_fit_order"))
    mooseError("In BaselineCorrection ",
               _name,
               ": No values were input for parameters 'accel_fit_order', 'vel_fit_order', or "
               "'disp_fit_order'. Please specify an integer value from 0 to 9 for at least one "
               "of these parameters.");

  // apply baseline correction to raw acceleration time history
  applyCorrection();

  // try building a linear interpolation object
  try
  {
    _linear_interp = libmesh_make_unique<LinearInterpolation>(_time, _adj_accel);
  }
  catch (std::domain_error & e)
  {
    mooseError("In BaselineCorrection ", _name, ": ", e.what());
  }
}

Real
BaselineCorrection::value(Real t, const Point & /*P*/) const
{
  return _scale_factor * _linear_interp->sample(t);
}

void
BaselineCorrection::applyCorrection()
{
  // store a reference to final array index
  unsigned int index_end = _accel.size() - 1;

  // Compute unadjusted velocity and displacment time histories
  Real dt;
  std::vector<Real> vel, disp;
  vel.push_back(0);
  disp.push_back(0);
  for (unsigned int i = 0; i < index_end; ++i)
  {
    dt = _time[i + 1] - _time[i];

    vel.push_back(BaselineCorrectionUtils::newmarkGammaIntegrate(
        _accel[i], _accel[i + 1], vel[i], _gamma, dt));
    disp.push_back(BaselineCorrectionUtils::newmarkBetaIntegrate(
        _accel[i], _accel[i + 1], vel[i], disp[i], _beta, dt));
  }

  // initialize polyfits and adjusted time history arrays as the nominal ones
  DenseVector<Real> coeffs;
  _adj_accel = _accel;
  std::vector<Real> p_fit, adj_vel = vel, adj_disp = disp;

  // adjust time histories with acceleration fit if desired
  if (isParamValid("accel_fit_order"))
  {
    _order = getParam<unsigned int>("accel_fit_order");
    coeffs = BaselineCorrectionUtils::getAccelerationFitCoeffs(
        _order, _adj_accel, _time, index_end, _gamma);

    for (unsigned int i = 0; i <= index_end; ++i)
    {
      p_fit = BaselineCorrectionUtils::computePolynomials(_order, coeffs, _time[i]);
      _adj_accel[i] -= p_fit[0];
      adj_vel[i] -= p_fit[1];
      adj_disp[i] -= p_fit[2];
    }
  }

  // adjust with velocity fit
  if (isParamValid("vel_fit_order"))
  {
    _order = getParam<unsigned int>("vel_fit_order");
    coeffs = BaselineCorrectionUtils::getVelocityFitCoeffs(
        _order, _adj_accel, adj_vel, _time, index_end, _beta);

    for (unsigned int i = 0; i <= index_end; ++i)
    {
      p_fit = BaselineCorrectionUtils::computePolynomials(_order, coeffs, _time[i]);
      _adj_accel[i] -= p_fit[0];
      adj_disp[i] -= p_fit[2];
    }
  }

  // adjust with displacement fit
  if (isParamValid("disp_fit_order"))
  {
    _order = getParam<unsigned int>("disp_fit_order");
    coeffs = BaselineCorrectionUtils::getDisplacementFitCoeffs(_order, adj_disp, _time, index_end);

    for (unsigned int i = 0; i <= index_end; ++i)
    {
      p_fit = BaselineCorrectionUtils::computePolynomials(_order, coeffs, _time[i]);
      _adj_accel[i] -= p_fit[0];
    }
  }
}

void
BaselineCorrection::buildFromFile()
{
  // Read data from CSV file
  MooseUtils::DelimitedFileReader reader(getParam<FileName>("data_file"), &_communicator);
  reader.read();

  // Check if specific column headers were input
  const bool time_header = isParamValid("time_name"),
             accel_header = isParamValid("acceleration_name");

  if (time_header && !accel_header)
    mooseError("In BaselineCorrection ",
               _name,
               ": A column header name was specified for the for the time data. Please specify a ",
               "header for the acceleration data using the 'acceleration_name' parameter.");
  else if (!time_header && accel_header)
    mooseError("In BaselineCorrection ",
               _name,
               ": A column header name was specified for the for the acceleration data. Please "
               "specify a header for the time data using the 'time_name' parameter.");

  // Obtain acceleration time history from file data
  if (time_header && accel_header)
  {
    _time = reader.getData(getParam<std::string>("time_name"));
    _accel = reader.getData(getParam<std::string>("acceleration_name"));
  }
  else
  {
    _time = reader.getData(0);
    _accel = reader.getData(1);
  }
}

void
BaselineCorrection::buildFromXandY()
{
  _time = getParam<std::vector<Real>>("time_values");
  _accel = getParam<std::vector<Real>>("acceleration_values");
}
