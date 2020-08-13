# Example 10a: Cantilever example demonstrating central difference time integration

## Model Description

This example demonstrates the Central Difference Time Integration in MASTODON. A 2 $ft$ x 2 $ft$ x 20 $ft$ cantilever is modeled with the bottom surface fixed along x, y & z directions. A uniform normal pressure of 1 $kip/ft^2$ is applied to the top surface along the axis of the model. The resulting stress time-history and displacement time-history are then obtained at locations (0, 0, 10) and (0, 0, 20) respectively and compared with results from Implicit Newmark-Beta time integration.

[fig:ex10a_input] demonstrates the overall dimensions and boundary conditions of the model.

!media media/examples/MASTODON_ex10_model.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex10a_input
       caption=Input model in MASTODON

## Modeling in MASTODON

!listing examples/ex10a/MASTODON_Explicit_HEX8.i

The automeshing option provided by MASTODON is used to create the geometry and specify the element type (see documentation of [GeneratedMesh](mesh/index.md) in Syntax for detailed information).

!listing examples/ex10a/MASTODON_Explicit_HEX8.i
         start=Mesh
         end=Variables

The displacement variables are defined in the `Variables` block. The accelerations, velocities and stresses are defined as auxiliary variables in the `AuxVariables` block.

`TensorMechanics` and `InertialForce` kernels are used to model the dynamics of the 3D solid elements. `Gravity` and damping are not modeled. The AuxKernel, `RankTwoAux` is used to compute the stress-zz.

!listing examples/ex10a/MASTODON_Explicit_HEX8.i
         start=AuxKernels
         end=Materials

The `ComputeIsotropicElasticityTensorBeam` block is used to create the elasticity tensor of the model, using Young's Modulus and Poisson's ratio. The stresses and strains are calculated using `ComputeLinearElasticStress` and `ComputeSmallStrain`. The densities are assigned to the solid block and the soil using `GenericConstantMaterial`.

The model is fixed along X, Y & Z directions at the bottom surface and an uniform vertical pressure of 1 $kip/ft^2$ is applied to the top surface (see [fig:ex10a_input]).

[CentralDifference](timeintegrators/index.md) does not perform a non-linear solve and hence has a simplified `Executioner` block in comparison to implicit time integration.

!listing examples/ex10a/MASTODON_Explicit_HEX8.i
         start=Executioner
         end=Postprocessors

Time integrators deriving from this class have three solve options, provided via the `solve_type` parameter:
1. `consistent`: (the default) A full mass matrix is built and used in a linear solve for the update.
2. `lumped`: A lumped-mass matrix is built, inverted, and applied to the RHS, which is faster but can possibly be less accurate.
3. `lump_preconditioned`: The inversion of the lumped-mass matrix is used to precondition the consistent solve.

The [CriticalTimeStep](postprocessors/CriticalTimeStep.md) postprocessor object can be used to automatically enforce the critical time step when using explicit time integration. A factor smaller than unity is recommended to be multiplied to the critical time step to ensure stability. `solve_type = consistent` is opted since Implicit Newmark Beta uses a full mass matrix.

## Results

The stress time-history and displacement time-history are obtained at locations (0, 0, 10) and (0, 0, 20) respectively and compared with results from Implicit Newmark-Beta time integration (as shown in [fig:ex10a_stresszz_0_0_10] and [fig:ex10a_Dispz_0_0_20]). These figures show that the implicit and explicit results are almost identical.


!media media/examples/Example10a_Stresszz_0_0_10.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex10a_stresszz_0_0_10
       caption=Stress-zz Time-History at (0, 0, 10)

!media media/examples/Example10a_Dispz_0_0_20.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex10a_Dispz_0_0_20
       caption=Disp-z Time-History at (0, 0, 20)
