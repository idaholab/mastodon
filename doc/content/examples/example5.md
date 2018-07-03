# Example 5: Direct shear type test

The example problem demonstrates a full-scale direct shear test of a concrete block sliding over non-linear soil. A non-linear soil block (4ft * 4ft * 4ft in dimension) is created with a concrete block on it's surface, which is linear elastic. A constant normal pressure is applied to the top surface of the concrete block while one of the sides is subjected to a sinusoidal cyclic displacement boundary condition. The non-linear soil domain is confined in all directions by setting displacements at the boundaries in the bottom and lateral directions to zero (since the sand is physically contained in a large tank, 4ft * 4ft *4ft in dimensions). We use the thin layer element formulation [citet:datta2018ncee] to model Coulomb-type frictional contact between the concrete block and non-linear soil. The thin layer elements use the I-Soil material model to simulate contact behavior through an elastic - perfectly plastic backbone curve with some nominal hardening if required. The non-linear soil block is also modeled using the I-Soil material model and possesses the properties of F-55 Ottawa sand. The geometry and meshing of the problem is shown in [fig:Schematic].

!media media/examples/Schematic.jpg
       style=width:50%;margin-left:250px;float:center;
       id=fig:Schematic
       caption=Schematic diagram of the concrete-soil assembly .

The input file to run this analysis is provided below.

!listing examples/ex05/3Dsliding_32psi.i

Following the Mesh block, GlobalParams, Variables, AuxVariables, Kernels, and AuxKernels are defined and detailed information about these blocks can be found in [Getting Started](manuals/user/index.md). The non-linear soil block is fixed in all directions and the gravity is applied along the negative z direction. The function block includes the gravity (32.2 $ft/s^2$), depth and density (90.5 $lb/ft^3$) dependent functions for initial stress to create in-situ soil conditions. In addition to the gravity, a normal pressure is applied on the top surface of the concrete block. The pressure is specified in the [BCs] block and acts in the negative z direction by default. Hence, it's sign is unchanged as opposed to the Gravity kernel. The loading is applied through the PresetDisplacement boundary condition where a loading function is applied. It is advisable to use a periodic function (sinusoidal in this example) with a gradually increasing amplitude in each cycle such that any unexpected response in the analysis displacement or acceleration is avoided. This generally occurs when the input displacement is specified at an interval (0.01 sec for this example) which doesn't match with the analysis velocity or acceleration time step size. If the simulation doesn't converge at the current timestep, the implicit executioner scales down the analysis timestep (say the displacement-time history is provided at an interval of 0.01 sec. If the results dont converge,the timestep size is divided by 2). Hence, the velocity and acceleration is computed for the half timestep leading to erratic output results. The best way to solve this issue is by sampling the input displacement-time history at a lower interval than the analysis time step.

The nodes at the same elevation in the concrete and thin layer blocks are allowed to move freely to simulate direct shear tests without confinement in the lateral boundaries. The initial confining stresses at each layer are expressed in the form of a parsed function, which computes the normal stresses due to the combined action of the normal pressure and gravity load.

!listing examples/ex05/3Dsliding_32psi.i
         start=BCs
         end=Materials

Layer ids are assigned to the individual soil layers including the thin layer element (TLE) block.  The reference pressure of the thin-layer elements is obtained by performing a gravity analysis. The gravity analysis is performed by switching off the inertial effects, namely, the inertia kernel blocks for the first few time steps to ensure that a static analysis is performed. The inertia kernel blocks are switched off using the [Controls] block in the input file. It is similar to specifying an initial step in ABAQUS where the static loads (gravity, normal pressure, etc) are defined, and then the dynamic loads and boundary conditions are implemented in the next time step. A relatively easy way of verifying the proper application of gravity is by obtaining the acceleration response history in the direction of gravity, and ensuring that the accelerations are close to zero. The `p_ref` is calculated as the average of $\sigma_x$, $\sigma_y$ and $\sigma_z$ (hydrostatic pressure) during the gravity analysis. It can be observed that the stresses are different from what would be expected of an isotropic elastic material due to material non-linearity in the thin layer I-Soil elements, as well as the non-linear soil domain beneath the thin layer.

## Modeling the thin layer

The failure criterion of the thin layer is similar to the Coulomb friction model, and is represented by the maximum shear strength $\tau _m$ = $\mu$*$\sigma_n$ where $\mu$ is the coefficient of friction and $\sigma_n$ is the normal stress due to the concrete block. The user-defined input shear stress vs. shear strain curve is defined to be bilinear. A nominal amount of hardening might be added depending on the problem (not added in this example) to avoid numerical instabilities that might arise from having a zero post-yield stiffness in the thin layer. The yield stress of this user-defined backbone curve is set to the critical frictional shear stress as mentioned above, and the corresponding shear strain is obtained by dividing the critical shear stress with the elastic modulus of concrete. The Poisson's ratio and the elastic modulus of the thin layer elements are set to the same as that of the concrete block, in order to prevent a sudden change in the volume of the interface elements once the yield point is reached.

The initial stresses of the concrete block, thin layer and soil domain are computed from the gravity analysis using a parsed function. The reference pressure for the thin layer elements is obtained by computing the hydrostatic stress state from the gravity analysis. The `pressure dependency` of the backbone curve is turned off and the shear strength of the thin layer is computed from the initial stresses. This is achieved by using the values 1, 0, and 0, respectively, for $a_0$, $a_1$, and $a_2$. Setting the `pressure dependency = false` in ISoil ensures that the backbone curve follows the critical shear stress/strain relationship which is manually input from computational analysis, and is therefore analogous to the Coulomb friction formulation.

!alert note
This type of backbone curve can now be automated using the 'soil_type = thin_layer' option in I-soil and
ComputeISoilStress.

!listing examples/ex05/3Dsliding_32psi.i
         start=Materials
         end=Preconditioning

## Modeling the non-linear soil domain

The properties of the user-defined backbone curve for the non-linear soil is obtained from experimental data [citet:stokoe2017recent] and is  modified in accordance with the normal confinement pressure as shown below. Typically, the reference pressure, `p_ref`, for the non-linear soil is set to the confinement pressure at which the backbone curve is obtained from experimental data. This is the hydrostatic stress state at which, the dynamic soil properties are defined. The reduction in shear modulus (see [fig:Modreduction]) with increasing shear strain is used to obtain the backbone curve (shear stress vs. shear strain) for varied confining pressures (see [fig:Backbonecurve1]).


!media media/examples/F-55OttawaSandProperties.jpg
     style=width:50%;margin-left:250px;float:center;
     id=fig:Backbonecurve1
     caption=Backbone curves of F-55 Ottawa sand under varied confining pressures.

!media media/examples/F55OttawaSand_Mod_reduction.jpg
     style=width:50%;margin-left:250px;float:center;
     id=fig:Modreduction
     caption=Modulus reduction curves of F-55 Ottawa sand.

## Results and discussion

The shear stress response is obtained at the interface using the ElementAverageValue postprocessor, which calculates the shear stress (averaged across the quadrature points) in each element. The results are similar to the theoretically computed values (see [fig:ex5_results2], [fig:ex5_results1], [fig:ex5_results3]). The Stress_zz pressure bulb distribution is theoretically computed using Newmarks and Boussinesq’s formulas for vertical stress profile beneath square footings, and the results match with the simulation values. The simulations were performed for confining pressures of 2 psi, 8 psi and 32 psi with normal pressures of the same magnitude (applied on top of the concrete block). The shear stress vs shear strain output of the thin layer for all the three cases are given below.

!media media/examples/resultsliding.jpg
     style=width:80%;margin-left:100px;float:center;
     id=fig:resultsliding
     caption=Simulation results for sliding of concrete block on F-55 Ottawa sand

!media media/examples/ex05_data1.jpg
    style=width:60%;margin-left:200px;float:center;
    id=fig:ex5_results2
    caption=Shear stress vs. shear strain response of interface block for a confining pressure of 2 psi

!media media/examples/ex05_data2.jpg
      style=width:60%;margin-left:200px;float:center;
      id=fig:ex5_results1
      caption=Shear stress vs. shear strain response of interface block for a confining pressure of 8 psi

!media media/examples/ex05_data3.jpg
     style=width:60%;margin-left:200px;float:center;
     id=fig:ex5_results3
     caption=Shear stress vs. shear strain response of interface block for a confining pressure of 32 psi

!bibtex bibliography
