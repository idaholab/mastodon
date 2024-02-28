# StressDivergenceIsolator

!syntax description /Kernels/StressDivergenceIsolator

## Description

This kernel assembles the Jacobian (stiffness matrix) and the residual for a two-noded isolator element (such as lead-rubber and friction-pendulum isolator elements) and should be defined for simulating an isolator element. This kernel also allows for stiffness damping and HHT integration when numerical damping is desired. It can also perform static initialization (by setting `static_initialization = true`) to ignore stiffness damping for the first time step. This is useful when a gravity analysis is performed. Further information on HHT damping and static initialization is provided [here](modules/solid_mechanics/Dynamics.md). The syntax and input parameters for this object are described below.

!syntax parameters /Kernels/StressDivergenceIsolator

!syntax inputs /Kernels/StressDivergenceIsolator

!syntax children /Kernels/StressDivergenceIsolator
