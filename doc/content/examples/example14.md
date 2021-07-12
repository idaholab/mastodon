# Example 14: Seismic analysis of a base-isolated nuclear power plant building

!alert note title=Units of this example
GN, GPa, m, and sec

This example demonstrates the seismic analysis of a nuclear power plant (NPP) building that houses a reactor vessel and four steam generators. This NPP building is identical to that of [Example 13](LINK) and is a hypothetical molten-chloride fast reactor (NPP). Details of the NPP building are provided in [Example 13](LINK) as well as [!citet](inl-ext-20-59608). This example is presented in two parts: (1) seismic analysis of just the foundation basemat on seismic isolators, and (2) seismic analysis of the full building on seismic isolators. The foundation basemat and the isolation system in both cases is identical. In each case, the modeling is briefly described and the results are presented.

## Part 1: Foundation basemat analysis

### Modeling

Before performing the seismic analysis of the full NPP building, the base-isolated basemat is first analyzed. The isolation system of the building comprises 70 Friction Pendulum^TM^ (FP) isolators arranged in a grid shown in [fig:iso_plan] below. Note that the figure shows a view of the foundation from the bottom, and therefore the +Z axis is pointing inwards into the basemat. The basemat is 98.75m x 68.75m in plan and is 1m thick.

!media media/examples/ex14/isolation_plan.png
       style=width:50%;margin-right:0px;float:center;
       id=fig:iso_plan
       caption=Isolators under the basemat of the NPP building

The basemat is modeled to be almost rigid, with an elastic modulus of almost 99.2 GPa, which is four times stiffer than concrete. Each of the isolators is independently attached to the basemat with rigid beam elements, thereby simulating a rigid connection between them. The isolators themselves are 0.3m long and have a friction coefficient, `mu_ref=0.06`, radius of curvature, and `r_eff=1.0`. These properties result in a sliding period of 2 sec, which is reasonable for the NPP building. Two methods of modeling the isolation system are demonstrated here. In method 1, the friction coefficients of the isolators are assumed to be independent of pressure, temperature, and velocity and the `mu_ref` value is used throughout the simulations. (See the [theory manual](manuals/theory/index.md) for a detailed description of pressure, temperature, velocity dependency of the friction coefficient of FP isolators). Additionally, a unidirectional ground motion is applied in the X direction. In method 2, the pressure, temperature, and velocity dependencies are switched on and ground motions are applied in all three directions. The Materials blocks for the isolators for the two mwethods are listed below, and the acceleration histories of the input motions are presented in [fig:inp_motion_xyz] below.

!listing examples/ex14/basemat_with_isolators_new.i block=Materials/elasticity

!listing examples/ex14/basemat_with_isolators_new_3D.i block=Materials/elasticity

!media media/examples/ex14/inp_motion_xyz.png
       style=width:50%;margin-right:0px;float:center;
       id=fig:inp_motion_xyz
       caption=Acceleration histories of the input ground motions used in this example.

!alert note title=Gravity analysis
Gravity analysis is an integral part of seismic analysis of base-isolated structures, especially with FP bearings. The shear strength of the FP bearings depends on the normal force on the surface, which comes from gravity. In this example, gravity loads are initiated through a static initialization performed in the first 3 time steps. Static analysis is achieve by (a) switching off the velocity and acceleration calculations in the time integrator for the first couple of timesteps in the `NewmarkBeta` TimeIntegrator block, and (b) switching off the inertia kernels using the controls block. Static initialization also requires that the stiffness damping is turned off in the stress divergence kernels, typically by setting `static_initialization=true` when available. This option is currently only available for continuum elements through the `DynamicTensorMechanics` block and the isolators through the `StressDivergenceIsolator` block. Static initialization is turned on for both of these kernels in this example. Note from the input file, that time derivative calculations are turned off for the first two timesteps and the inertia kernels are switched on in the 3rd time step. It is highly recommended that users go through these parts of the input file that enable gravity simulation.

### Outputs

An important output in seismic analysis of isolated systems is the force-displacement relationship from the isolators. Both the acceleration responses of the basemat and the force-displacement relationship of one of the isolators is presented in this section.

In [fig:bm_fb_xz] and [fig:bm_fb_yz], the orange and blue lines correspond to methods 1 and 2, respectively. Note that when the pressure, temperature, and velocity dependencies are switched on (method 2, blue) the hysteresis is a lot more complicated and the stiffness varies significantly during the earthquake. Note that the results for method 1 do not appear in [fig:bm_fb_yz] because they are almost zero, given that the ground motion for method 1 is unidirectional. The results presented here are for an isolator at the center of the basemat plan.

!row!

!col! small=12 medium=6 large=6
!media media/examples/ex14/basemat_fb_XZ.png
       style=width:100% id=fig:bm_fb_xz
       caption=Shear force-displacement history in the XZ direction. Orange is method 1 and blue is method 2.
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/ex14/basemat_fb_YZ.png
      style=width:100% id=fig:bm_fb_yz
      caption=Shear force-displacement history in the YZ direction. Orange is method 1 and blue is method 2.
!col-end!

!row-end!

The acceleration response spectra calculated at the center of the basemat are presented in the figures below. The top row presents the input acceleration spectra and the bottom row presents the output spectra at the center of the basemat in X, Y, and Z directions. Again, in these figures, blue plots present method 1 and orange plots present method 2. The figures show that the accelerations at the top of the basemat reduce drastically due to seismic isolation, except in the Z direction, in which, the response almost stays the same, demonstrating a typical isolation response with FP isolation systems.

!row!

!col! small=12 medium=4 large=4
!media media/examples/ex14/inp_sa_x.png
       style=width:100% id=fig:inp_sa_x
       caption=5% damped acceleration response spectra in X direction (input)

!media media/examples/ex14/basemat_sa_x.png
       style=width:100% id=fig:bm_sa_x
       caption=5% damped acceleration response spectra in X direction (basemat center)
!col-end!

!col! small=12 medium=4 large=4
!media media/examples/ex14/inp_sa_y.png
       style=width:100% id=fig:inp_sa_y
       caption=5% damped acceleration response spectra in Y direction (input)

!media media/examples/ex14/basemat_sa_y.png
       style=width:100% id=fig:bm_sa_y
       caption=5% damped acceleration response spectra in Y direction (basemat center)
!col-end!

!col! small=12 medium=4 large=4
!media media/examples/ex14/inp_sa_z.png
       style=width:100% id=fig:inp_sa_z
       caption=5% damped acceleration response spectra in Z direction (input)

!media media/examples/ex14/basemat_sa_z.png
       style=width:100% id=fig:bm_sa_z
       caption=5% damped acceleration response spectra in Z direction (basemat center)
!col-end!

!row-end!


## Part 2: NPP building analysis

Now that the seismic response with just the basemat is shown to be reasonable, the response of the seismically isolated building is presented in this part.

### Modeling



!bibtex bibliography
