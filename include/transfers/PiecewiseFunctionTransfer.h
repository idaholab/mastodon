#ifndef PIECEWISEFUNCTIONTRANSFER_H
#define PIECEWISEFUNCTIONTRANSFER_H

// MOOSE includes
#include "MultiAppTransfer.h"

class PiecewiseFunctionTransfer;
class PiecewiseBase;

template <>
InputParameters validParams<PiecewiseFunctionTransfer>();

/**
 * Transfers scale hazard curve to a Piecewise function.
 */
class PiecewiseFunctionTransfer : public MultiAppTransfer
{
public:
  PiecewiseFunctionTransfer(const InputParameters & parameters);
  virtual void execute() override;

  /**
   * Performs data transfer between two Piecewsie functions.
   */
  void transfer(PiecewiseBase & from_function, PiecewiseBase & to_function);
};

#endif
