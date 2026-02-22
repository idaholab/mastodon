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

// This code was implemented in collaboration with Christopher J. Wong (chris.wong@utah.edu) from
// the University of Utah as part of NEUP Project: 17-12939.

#include "BaselineCorrection.h"

registerMooseObject("MastodonApp", BaselineCorrection);

InputParameters
BaselineCorrection::validParams()
{
  InputParameters params = PiecewiseBase::validParams();
  params.addClassDescription("Applies a baseline correction to an acceleration time history using "
                             "the type-oriented algorithm provided by Wong and Ibarra (2022) and "
                             "evaluates the corrected ordinates of a kinematic variable.");

  params.addRequiredParam<FunctionName>(
      "function", "The PiecewiseLinear function providing the acceleration to be corrected.");

  params.addParam<unsigned int>(
      "accel_fit_order",
      3,
      "The degree of the polynomial used to adjust the acceleration.\n\nIf not specified while "
      "'vel_fit_order' and/or 'disp_fit_order' are, then no acceleration fit is applied.");
  params.addRangeCheckedParam<unsigned int>(
      "vel_fit_order",
      "vel_fit_order > 0",
      "The degree of the polynomial used to adjust the velocity. The minimum is one.");
  params.addRangeCheckedParam<unsigned int>(
      "disp_fit_order",
      "disp_fit_order > 1",
      "The degree of the polynomial used to adjust the displacement. The minimum is two.");
  params.addParamNamesToGroup("accel_fit_order vel_fit_order disp_fit_order", "Correction Type");

  MooseEnum kinematic_variables("acceleration velocity displacement", "acceleration");
  params.addParam<MooseEnum>(
      "series_output",
      kinematic_variables,
      "The kinematic variable whose corrected time history is to be evaluated.");

  params.addRangeCheckedParam<Real>(
      "gamma",
      0.5,
      "(0 <= gamma) & (gamma <= 1)",
      "The gamma parameter for numerical integration of acceleration by the Newmark-beta rule.");
  params.addRangeCheckedParam<Real>(
      "beta",
      0.25,
      "(0 <= beta) & (beta <= 0.5)",
      "The beta parameter for numerical integration of velocity by the Newmark-beta rule.");

  params.addRangeCheckedParam<Real>(
      "error_on_residual",
      1e-04,
      "(0 < error_on_residual) & (error_on_residual < 1)",
      "The relative residual of the solution to the normal equations that invokes an error.\n\n"
      "Large residuals may indicate poor or invalid least squares approximations, which can lead "
      "to unstable and futile adjustments to the time histories.");
  params.addParamNamesToGroup("error_on_residual", "Advanced");

  return params;
}

BaselineCorrection::BaselineCorrection(const InputParameters & parameters)
  : PiecewiseBase(parameters), FunctionInterface(this)
{
  const Function * func_ptr = &getFunctionByName(getParam<FunctionName>("function"));
  if (func_ptr->type() != "PiecewiseLinear")
    paramError("function",
               "This must be the name of a PiecewiseLinear object (use 'type = PiecewiseLinear').");

  // Convert the function pointer to an instance of PiecewiseBase so its data can be retrieved
  const PiecewiseBase * const accel_func = dynamic_cast<const PiecewiseBase *>(func_ptr);

  // Initialize containers for storing the time histories and the natural time abscissa
  const auto size = accel_func->functionSize();
  std::vector<Real> time(size), accel(size), vel(size), disp(size), tau(size);
  time[0] = accel_func->domain(0);
  accel[0] = accel_func->range(0);

  // Compute the Jacobian to transform differentials onto the natural time domain
  const auto jmap = accel_func->domain(size - 1) - time[0];

  // Setup the basic values needed for time integration
  Real dt, dv, dvo;
  const auto gamma = getParam<Real>("gamma"), beta = getParam<Real>("beta"), cogamma = 1.0 - gamma,
             cobeta = 0.5 - beta;

  for (std::size_t i = 1; i < size; ++i)
  {
    time[i] = accel_func->domain(i);
    dt = time[i] - time[i - 1];
    accel[i] = accel_func->range(i);

    // Compute the nominal velocity and displacement by integrating with Newmark's method
    dvo = dt * accel[i - 1];
    dv = dt * accel[i];
    vel[i] = vel[i - 1] + cogamma * dvo + gamma * dv;
    disp[i] = disp[i - 1] + dt * vel[i - 1] + cobeta * dt * dvo + beta * dt * dv;

    // Compute natural time
    tau[i] = tau[i - 1] + dt / jmap;
  }

  std::vector<Real> coeffs;
  const Real rtol = getParam<Real>("error_on_residual");
  if (parameters.isParamSetByUser("accel_fit_order") ||
      (!isParamValid("vel_fit_order") && !isParamValid("disp_fit_order")))
  {
    coeffs.resize(getParam<unsigned int>("accel_fit_order") + 1);
    fitTimeSeries(&coeffs, 0, tau, accel, jmap, rtol);
    applyAdjustment(coeffs, tau, accel, vel, disp, jmap);
  }

  if (isParamValid("vel_fit_order"))
  {
    coeffs.resize(getParam<unsigned int>("vel_fit_order"));
    fitTimeSeries(&coeffs, 1, tau, vel, jmap, rtol);
    applyAdjustment(coeffs, tau, accel, vel, disp, jmap);
  }

  if (isParamValid("disp_fit_order"))
  {
    coeffs.resize(getParam<unsigned int>("disp_fit_order") - 1);
    fitTimeSeries(&coeffs, 2, tau, disp, jmap, rtol);
    applyAdjustment(coeffs, tau, accel, vel, disp, jmap);
  }

  switch (getParam<MooseEnum>("series_output"))
  {
    case 0:
      setData(time, accel);
      break;
    case 1:
      setData(time, vel);
      break;
    case 2:
      setData(time, disp);
      break;
  }
}

Real
BaselineCorrection::value(Real t, const Point & /*p*/) const
{
  return _corrected_series.sample(t);
}

void
BaselineCorrection::fitTimeSeries(std::vector<Real> * c,
                                  const int & nmin,
                                  const std::vector<Real> & tau,
                                  const std::vector<Real> & u,
                                  const Real & jmap,
                                  const Real & rtol)
{
  mooseAssert(0 <= nmin && nmin <= 2,
              "The value for the minimum polynomial degree 'nmin' must be 0 for acceleration, 1 "
              "for velocity, or 2 for displacement.");
  mooseAssert(tau.size() == u.size(),
              "The containers for the time series data, 'tau' and 'u', must be of equal size.");
  mooseAssert(std::round(tau.front() / rtol) * rtol == 0.0 &&
                  std::round(tau.back() / rtol) * rtol == 1.0 && jmap > 0.0,
              "The natural time 'tau' domain must be [0, 1] and its Jacobian transform 'jmap' must "
              "be strictly positive.");

  auto num_coeffs = c->size();
  DenseMatrix<Real> mat(num_coeffs, num_coeffs);
  DenseVector<Real> rhs(num_coeffs), coeffs(num_coeffs);

  // Setup the basic values needed for assembling the system of normal equations
  Real dtau;
  const auto nminsq = MathUtils::pow(nmin, 2), a0 = (nminsq - 3 * nmin + 2) / 2,
             a1 = (nminsq - 5 * nmin + 6) / 2, a2 = (nminsq - nmin - 4) / 2;

  for (MooseIndex(coeffs) k = 0; k < num_coeffs; ++k)
  {
    // Compute the normal matrix
    for (MooseIndex(coeffs) j = 0; j < num_coeffs; ++j)
      mat(k, j) = (a0 * MathUtils::pow(j, 2) + a1 * j - a2) / (k + j + 2.0 * nmin + 1.0);

    // Compute the vector on the right-hand side by trapezoidal integration
    for (MooseIndex(tau) i = 0; i < tau.size() - 1; ++i)
    {
      dtau = tau[i + 1] - tau[i];
      MathUtils::addScaled(dtau, MathUtils::pow(tau[i], k + nmin) * u[i], rhs(k));
      MathUtils::addScaled(dtau, MathUtils::pow(tau[i + 1], k + nmin) * u[i + 1], rhs(k));
    }
    rhs(k) /= 2.0 * MathUtils::pow(jmap, nmin);
  }

  // Solve the system for the polynomial coefficients using LU factorization with partial pivoting
  //
  // Note: A copy of the normal matrix is created because it will be modified by the 'lu_solve()'
  //       method, and yet it is still needed afterward to calculate the residual.
  const DenseMatrix<Real> mat_copy = mat;
  mat.lu_solve(rhs, coeffs);
  *c = coeffs.get_values();

  if (std::any_of(c->cbegin(), c->cend(), [](const Real & k) { return !std::isfinite(k); }))
    mooseError("Failed to compute a least squares polynomial of degree (",
               num_coeffs + nmin - 1,
               ") - one or more of the coefficients are undefined.\n\nTry using a lower order for "
               "the fit. Otherwise, there may be an issue with the raw time series data.");

  // Compute the relative residual to assess the accuracy of the best-fit
  DenseVector<Real> resultant(num_coeffs);
  mat_copy.vector_mult(resultant, coeffs);
  resultant.add(-1.0, rhs);
  const auto res = resultant.l2_norm() / rhs.l2_norm();
  if (res > rtol)
    mooseError("Failed to compute a least squares polynomial of degree (",
               num_coeffs + nmin - 1,
               ") within the specified tolerance (",
               rtol,
               " < ",
               res,
               ").\n\nConsider using a lower order or a greater tolerance.");
}

void
BaselineCorrection::applyAdjustment(const std::vector<Real> & c,
                                    const std::vector<Real> & tau,
                                    std::vector<Real> & accel,
                                    std::vector<Real> & vel,
                                    std::vector<Real> & disp,
                                    const Real & jmap)
{
  for (MooseIndex(tau) i = 0; i < tau.size(); ++i)
    for (MooseIndex(c) k = 0; k < c.size(); ++k)
    {
      accel[i] -= (MathUtils::pow(k, 2) + 3.0 * k + 2.0) * c[k] * MathUtils::pow(tau[i], k);
      vel[i] -= jmap * (k + 2.0) * c[k] * MathUtils::pow(tau[i], k + 1);
      disp[i] -= MathUtils::pow(jmap, 2) * c[k] * MathUtils::pow(tau[i], k + 2);
    }
}

void
BaselineCorrection::setData(const std::vector<Real> & t, const std::vector<Real> & u)
{
  PiecewiseBase::setData(t, u);
  _corrected_series.setData(_raw_x, _raw_y);
}
