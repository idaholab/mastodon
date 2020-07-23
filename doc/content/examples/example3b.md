## Example 3b: Nonlinear site-response analysis (explicit time integration)

[Example 3a](examples/example3a.md) can also be run using an explicit time integration through the `CentralDifference`
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
time step to ensure stability. Also, displacement input has to be applied to the
soil column instead of an acceleration input.

The input file is presented below:

!listing examples/ex03b/shear_beam_Isoil_free_field_explicit.i

The plot below compares the accelerations recored at the top of the soil column between
explicit and implicit integration schemes and also their 5 percent damped response spectra.
 These results match closely.

!row!

!col! small=12 medium=6 large=6
!media media/examples/Exp_Imp_ex03.png
     style=width:100%;float:center;
     id=fig:acc_comp
      caption=Comparison of accelerations at the soil surface generated using explicit and implicit schemes.
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/Exp_Imp_ex03_Sa.png
     style=width:95%;float:center;
     id=fig:acc_comp
      caption=Comparison of response spectra at the soil surface generated using explicit and implicit schemes.
!col-end!

!row-end!

!alert note title=Current limitations of explicit integration in MASTODON
Explicit integration using the `CentralDifference` scheme: (1) does not account for
element stiffness damping; (2) cannot be used for element types other than solid
elements; and (3) does not work with input acceleration (input displacement should
be applied). However, the MASTODON team is working to remove these limitations in the near future.
