## Example 1b: Direct shear test using the Darandeli backbone curve formulation (explicit time integration)

[Example 1a](examples/example1a.md) can also be run using an explicit time integration through the `CentralDifference`
scheme. Unlike implicit, Newmark-Beta integration Central Difference integration
does not require nonlinear iterations (i.e., calculating the Jacobian, etc.). However,
explicit schemes are not unconditionally stable. For stability, the timestep
should be less than the so-called critical time step. [`CentralDifference`](source/timeintegrators/CentralDifference.md)
and [`CriticalTimeStep`](source/postprocessors/CriticalTimeStep.md) pages
provide more information on explicit integration and its stability, respectively.

When the using the explicit scheme, while the user can specify the time step for the solver,
it is recommended to use the `CriticalTimeStep` postprocessor which automatically
calculates and applies the critical time step based on the mesh size and material
properties. A factor smaller than unity is recommended to be multiplied to the critical
time step to ensure stability.

The input file is presented below:

!listing examples/ex01b/HYS_darendeli_explicit.i

The plot below compares the hysteretic response between explicit and implicit integration
schemes. These results match closely.

!media media/examples/Exp_Imp_ex01.png
     style=width:70%;float:center;
     id=fig:ad_acc_comp
     caption=Comparison of hysteretic responses generated using explicit and implicit schemes.

!alert note title=Current limitations of explicit integration in MASTODON
Explicit integration using the `CentralDifference` scheme: (1) does not account for
element stiffness damping; (2) cannot be used for element types other than solid
elements; and (3) does not work with input acceleration (input displacement should
be applied). However, the MASTODON team is working to remove these limitations in the near future.
