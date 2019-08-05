# Example 6a: Displacement-controlled contact problem with elastic soil

## Model Description

This example demonstrates frictional contact between a solid block and elastic soil in MASTODON. A 6 inch cubic block is centered on top of a 4 foot cubic block of soil. The top surface of the soil is free and the remaining surfaces are fixed in all directions. A uniform normal pressure of 5 psi is applied to the top of the smaller solid block and it is given a prescribed displacement in a direction parallel to the contact surface. The resulting normal and frictional forces at the interface of the two materials are then obtained and compared with analytical results.

## Modeling in MASTODON

!listing examples/ex06a/psf_grav.i

The mesh used in this model was generated externally using the mesh generation software, [Cubit](https://cubit.sandia.gov/). MASTODON imports the mesh shown in [fig:ex06input] using the `FileMesh` type specified in the `Mesh` block. Details regarding supported mesh file types can be found at [GettingStarted](manuals/user/index.md).

!media media/examples/ex06/ex06_input.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex06input
       caption=Input model in MASTODON.

The displacement variables are defined in the `Variables` block. The accelerations, velocities, normal and tangential (frictional) forces are defined as auxiliary variables in the `AuxVariables` block.

[TensorMechanics](syntax/index.md) and [InertialForce](syntax/index.md) kernels are used to model the dynamics of the 3D solid elements. The `Gravity` block is specified in `Kernels` in order to apply gravitational force to the entire model. The [Newmark time integration](manuals/theory/index.md) parameters used in this problem correspond to the Newmark's average acceleration method, i.e. `beta = 0.25` and `gamma = 0.5` and damping is ignored.

The [ComputeIsotropicElasticityTensorBeam](syntax/index.md) block is used to create the elasticity tensor of the solid block and the elastic soil, using Young's Modulus and Poisson's ratio. The stresses and strain are calculated using [ComputeFiniteStrainElasticStress](syntax/index.md) and [ComputeFiniteStrain](syntax/index.md). The densities are assigned to the solid block and the soil using [GenericConstantMaterial](syntax/index.md).

In the soil block, the top surface is free and all other surfaces are fixed in every direction. A uniform vertical pressure of 5 psi is applied to the top surface of the solid block and it is given the prescribed displacement history shown in [input_motion].

!plot scatter filename=examples/ex06a/input_motion.csv
              data=[{'x':'time', 'y':'displacement'}]
              layout={'xaxis':{'title':'Time (secs)'},'yaxis':{'title':'Displacement (in)'}, 'title':'Displacement of the block'}
              caption=Input displacement of the block
              id=input_motion

[Contact](manuals/user/index.md) is defined in MASTODON using the `Contact` block. This model is simulating Coulomb friction, with a coefficient of friction value equal to 0.7. The formulation implemented and demonstrated in this example uses a master/slave concept in which the nodes of the slave boundary are restricted by the nodes of the master surface boundary. Penetration is enforced using a penalty factor which is inversely related to the amount of penetration allowed (large penalty factor results in small amount of penetration and vice versa). The factor used in this example is 1e5.

!listing examples/ex06a/psf_grav.i start=Contact end=Preconditioning

Static analysis is performed by turning off inertial terms for a certain time period in the `Controls` block.
It stabilizes the system under the gravitational loading and pressure to perform dynamic analysis. The `Executioner` and the `Postprocessors` blocks are provided to solve the nonlinear system and extract the required data.

## Results

The graph for the frictional force as a function of displacement obtained from MASTODON is shown in [frictional_force].

!plot scatter filename=examples/ex06a/psf_grav_out.csv
              data=[{'x':'dispx', 'y':'tang_forc_x'}]
              layout={'xaxis':{'title':'Displacement (in)'},'yaxis':{'title':'Frictional Force (lbf)'}}
              caption=Graph showing the frictional force as a function of displacement for Columb Friction Model
              id=frictional_force

The graph for the normal force as a function of time obtained from MASTODON is shown in [normal_force].

!plot scatter filename=examples/ex06a/psf_grav_out.csv
              data=[{'x':'time', 'y':'nor_forc'}]
              layout={'xaxis':{'title':'Time (s)'},'yaxis':{'title':'Normal Force (lbf)'}}
              caption=Graph showing the normal force as a function of time for Columb Friction Model
              id=normal_force

## Theoretical Solution

Normal Force = density x volume x gravity + pressure x area = 198.7473 lbf

Frictional Force = coefficient of friction x Normal Force = 139.123 lbf

The results from MASTODON are in agreement with the theoretical calculations.
