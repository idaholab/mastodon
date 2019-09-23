## Getting Started

This section provides step-by-step instructions to define the basic components of a numerical
model for finite element analysis using the MASTODON framework.

### Kernels

Kernels are related to the physics involved in the particular analysis. This document focusses on
kernel commands related to the geotechnical earthquake engineering applications. Further information
can be found in [/Kernels/index.md].

The main kernel that is used for quasi-static and dynamic analyses is
[modules/tensor_mechanics/index.md]. This kernel is used to solve the equation of motion without the
inertial effects. It requires information about the unknowns that are solved for. The following chunk
of commands can be used to activate dynamic tensor mechanics kernel along with Newmark-beta
integration scheme, inertial effects, and two mode Rayleigh viscous damping (both stiffness and mass
proportional damping):

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Kernels
         end=BCs

displacements = 'disp_x disp_y disp_z' line is not necessary if the displacements are already defined
as global parameters but are provided here for the sake of completeness. Beta and gamma are the
parameters of Newmark-beta integration scheme. Zeta and eta are the stiffness and mass matrix
coefficients of Rayleigh damping formulation (see [modules/tensor_mechanics/index.md] and
[Theory Manual](manuals/theory/index.md) for more information). Auxiliary kernels are specified to
calculate the acceleration and velocities using Newmark-beta scheme at the end of each time step
where the displacement is already solved and known. Lastly, stress_xy is defined as an auxiliary
variable. This is achieved by specifying the type of the Auxkernel as RankTwoAux. RankTwoAux means
that the source of the auxiliary variable is a rank two tensor, and the type of rank two tensor is
explicitly defined as stress tensor using the command "rank_two_tensor = stress". Since, the variable
is stress_xy, the location in the stress tensor corresponding to stress_xy needs to be specified
using index_i (row index) and index_j (column index). To request for stress_xy, index_i is set to 0
and index_j is set to 1. The next section explains the boundary conditions that are required to run a
simple, quasi-static and dynamic analyses.
