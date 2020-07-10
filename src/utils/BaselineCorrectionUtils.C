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
#include "BaselineCorrectionUtils.h"

Real
BaselineCorrectionUtils::newmarkGammaIntegrate(const Real & u_ddot_old,
                                               const Real & u_ddot,
                                               const Real & u_dot_old,
                                               const Real & gamma,
                                               const Real & dt)
{
  return u_dot_old + (1 - gamma) * dt * u_ddot_old + gamma * dt * u_ddot;
}

Real
BaselineCorrectionUtils::newmarkBetaIntegrate(const Real & u_ddot_old,
                                              const Real & u_ddot,
                                              const Real & u_dot_old,
                                              const Real & u_old,
                                              const Real & beta,
                                              const Real & dt)
{
  return u_old + dt * u_dot_old + (0.5 - beta) * dt * dt * u_ddot_old + beta * dt * dt * u_ddot;
}

DenseVector<Real>
BaselineCorrectionUtils::getAccelerationFitCoeffs(unsigned int order,
                                                  const std::vector<Real> & accel,
                                                  const std::vector<Real> & t,
                                                  const unsigned int & num_steps,
                                                  const Real & gamma)
{
  unsigned int num_rows = order + 1;
  DenseMatrix<Real> mat(num_rows, num_rows);
  DenseVector<Real> rhs(num_rows);
  DenseVector<Real> coeffs(num_rows);

  // compute matrix of linear normal equation
  for (unsigned int row = 0; row < num_rows; ++row)
  {
    for (unsigned int col = 0; col < num_rows; ++col)
    {
      mat(row, col) =
          pow(t[t.size() - 1], row + col + 1) * (col * col + 3 * col + 2) / (row + col + 1);
    }
  }

  // compute vector of integrals on right-hand side of linear normal equation
  Real dt, u_ddot_old, u_ddot;
  for (unsigned int i = 0; i < num_steps; ++i)
  {
    dt = t[i + 1] - t[i];
    for (unsigned int row = 0; row < num_rows; ++row)
    {
      u_ddot_old = pow(t[i], row) * accel[i];
      u_ddot = pow(t[i + 1], row) * accel[i + 1];

      rhs(row) += newmarkGammaIntegrate(u_ddot_old, u_ddot, 0.0, gamma, dt);
    }
  }

  // solve the system using libMesh lu factorization
  mat.lu_solve(rhs, coeffs);
  return coeffs;
}

DenseVector<Real>
BaselineCorrectionUtils::getVelocityFitCoeffs(unsigned int order,
                                              const std::vector<Real> & accel,
                                              const std::vector<Real> & vel,
                                              const std::vector<Real> & t,
                                              const unsigned int & num_steps,
                                              const Real & beta)
{
  unsigned int num_rows = order + 1;
  DenseMatrix<Real> mat(num_rows, num_rows);
  DenseVector<Real> rhs(num_rows);
  DenseVector<Real> coeffs(num_rows);

  // compute matrix of linear normal equation
  for (unsigned int row = 0; row < num_rows; ++row)
  {
    for (unsigned int col = 0; col < num_rows; ++col)
    {
      mat(row, col) = pow(t[t.size() - 1], row + col + 3) * (col + 2) / (row + col + 3);
    }
  }

  // compute vector of integrals on right-hand side of linear normal equation
  Real dt, u_ddot_old, u_ddot, u_dot_old;
  for (unsigned int i = 0; i < num_steps; ++i)
  {
    dt = t[i + 1] - t[i];
    for (unsigned int row = 0; row < num_rows; ++row)
    {
      u_dot_old = pow(t[i], row + 1) * vel[i];
      u_ddot_old = pow(t[i], row + 1) * accel[i] + (row + 1) * pow(t[i], row) * vel[i];
      u_ddot = pow(t[i + 1], row + 1) * accel[i + 1] + (row + 1) * pow(t[i + 1], row) * vel[i + 1];

      rhs(row) += newmarkBetaIntegrate(u_ddot_old, u_ddot, u_dot_old, 0.0, beta, dt);
    }
  }

  // solve the system using libMesh lu factorization
  mat.lu_solve(rhs, coeffs);
  return coeffs;
}

DenseVector<Real>
BaselineCorrectionUtils::getDisplacementFitCoeffs(unsigned int order,
                                                  const std::vector<Real> & disp,
                                                  const std::vector<Real> & t,
                                                  const unsigned int & num_steps)
{
  unsigned int num_rows = order + 1;
  DenseMatrix<Real> mat(num_rows, num_rows);
  DenseVector<Real> rhs(num_rows);
  DenseVector<Real> coeffs(num_rows);

  // computer matrix of linear normal equation
  for (unsigned int row = 0; row < num_rows; ++row)
  {
    for (unsigned int col = 0; col < num_rows; ++col)
    {
      mat(row, col) = pow(t[t.size() - 1], row + col + 5) / (row + col + 5);
    }
  }

  // compute vector of integrals on right-hand side of linear normal equation
  Real dt, u_old, u;
  for (unsigned int i = 0; i < num_steps; ++i)
  {
    dt = t[i + 1] - t[i];
    for (unsigned int row = 0; row < num_rows; ++row)
    {
      u_old = pow(t[i], row + 2) * disp[i];
      u = pow(t[i + 1], row + 2) * disp[i + 1];

      // note: newmarkGamma with gamma = 0.5 is trapezoidal rule
      rhs(row) += newmarkGammaIntegrate(u_old, u, 0.0, 0.5, dt);
    }
  }

  // solve the system using libMesh lu factorization
  mat.lu_solve(rhs, coeffs);
  return coeffs;
}

std::vector<Real>
BaselineCorrectionUtils::computePolynomials(unsigned int order,
                                            const DenseVector<Real> & coeffs,
                                            const Real & t)
{
  // Compute the best-fit polynomial of the acceleration and its derivatives
  std::vector<Real> p_fit(3);
  for (unsigned int k = 0; k < order + 1; ++k)
  {
    p_fit[0] += (k * k + 3 * k + 2) * coeffs(k) * pow(t, k);
    p_fit[1] += (k + 2) * coeffs(k) * pow(t, k + 1);
    p_fit[2] += coeffs(k) * pow(t, k + 2);
  }

  return p_fit;
}
