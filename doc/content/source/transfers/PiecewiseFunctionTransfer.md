# PiecewiseFunctionTransfer

## Description

A simple transfer that moves the data of a PiecewiseLinear in the master to or from another
PiecewiseLinear function in the sub application. Note that the function is transferred at the second time step (time step 1) since the multiapps are initialized at the first time step.


## Example Input File Syntax

!listing /test/tests/transfers/piecewise_function/to_master.i


!syntax parameters /Transfers/PiecewiseFunctionTransfer

!syntax inputs /Transfers/PiecewiseFunctionTransfer

!syntax children /Transfers/PiecewiseFunctionTransfer
