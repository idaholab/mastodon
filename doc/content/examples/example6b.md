# Example 6b: Displacement-controlled frictional contact problem with  I-soil and using restart option for static initialization.

## Model Description

This example demonstrates frictional contact between a solid block and I-soil in MASTODON. A 6 inch cubic block is centered on top of a 4 foot cubic block of soil. The top surface of the soil is free and the remaining surfaces are fixed in all directions. A uniform normal pressure of 5 psi is applied to the top of the smaller solid block and it is given a prescribed displacement in a direction parallel to the contact surface. The resulting normal and frictional forces at the interface of the two materials are then obtained and compared with analytical results.

## Modeling in MASTODON

The simulation is carried out in two steps.

## Step 1: Stabilizing the system under the gravity and pressure forces

!listing examples/ex06b/stabilize_isoil.i

 A static analysis is carried out in the first step to stabilize the system under the action of constant gravitational force and pressure force. The mesh shown in [fig:ex06_input] used in this model was generated externally using the mesh generation software, [Cubit](https://cubit.sandia.gov/). MASTODON imports the mesh using the `FileMesh` type specified in the `Mesh` block.

The displacement variables are defined in the `Variables` block. The accelerations, velocities, normal and tangential (frictional) forces are defined as auxiliary variables in the `AuxVariables` block.

!media media/examples/ex06/ex06_input.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex06_input
       caption=Input model in MASTODON


[TensorMechanics](syntax/index.md) kernel is used to set up the stress divergence kernels and [Gravity](syntax/index.md) kernel is used to apply the gravitational force. The [ComputeIsotropicElasticityTensorBeam](syntax/index.md) block is used to create the elasticity tensor of the elastic solid block, using Young's Modulus and Poisson's ratio. The stresses and strain are calculated using [ComputeFiniteStrainElasticStress](syntax/index.md) and [ComputeFiniteStrain](syntax/index.md). The densities are assigned to the solid block and the soil using [GenericConstantMaterial](syntax/index.md).

The I-soil block is used to create a nonlinear hysteretic soil model. The detailed explanation for this model can be found at [I-soil](syntax/Materials/I_Soil/index.md).

In the soil block, the top surface is free and all other surfaces are fixed in every direction. A uniform vertical pressure of 5 psi is applied to the top surface of the solid block.

[Contact](manuals/user/index.md) is defined in MASTODON using the `Contact` block. This model is simulating Coulomb friction, with a coefficient of friction value equal to 0.2. The formulation implemented and demonstrated in this example uses a master/slave concept in which the nodes of the slave boundary are restricted by the nodes of the master surface boundary. Penetration is enforced using a penalty factor which is inversely related to the amount of penetration allowed (large penalty factor results in small amount of penetration and vice versa). The factor used in this example is 1e5.

!listing examples/ex06b/stabilize_isoil.i start=Contact end=Preconditioning

The use of restart in the MOOSE framework requires to specify `checkpoints` in the `Outputs` block. The `checkpoint` creates a set of files that be used as an initial condition for the next simulation.

!listing examples/ex06b/stabilize_isoil.i start=Outputs

## Step 2: Performing the dynamic simulation using the stabilized system

!listing examples/ex06b/actual_simulation_isoil.i

 The final state of the previous simulation is used as an input file for the dynamic simulation using the `Mesh`. `Problem` block is used to define the advanced restart option as shown below.

!listing examples/ex06b/actual_simulation_isoil.i start=Mesh end=GlobalParams

The remaining input file is similar to the input file used in the previous step. The aux variables and the kernels required for the dynamic analysis are added and the input motion is applied to the block as shown in [input_motion].

!plot scatter filename=examples/ex06a/input_motion.csv
              data=[{'x':'time', 'y':'displacement'}]
              layout={'xaxis':{'title':'Time (secs)'},'yaxis':{'title':'Displacement (in)'}, 'title':'Displacement of the block'}
              caption=Input displacement of the block
              id=input_motion


## Results

The graph for the frictional force as a function of displacement obtained from MASTODON is shown in [frictional_force].

!plot scatter filename=examples/ex06b/finalresult.csv
              data=[{'x':'dispx', 'y':'tang_forc_x'}]
              layout={'xaxis':{'title':'Displacement (in)'},'yaxis':{'title':'Frictional Force (lbf)'}}
              caption=Frictional force as a function of displacement for Coulomb friction model
              id=frictional_force


## Theoretical Solution

Normal Force = density x volume x gravity + pressure x area = 198.7473 lbf

Frictional Force = coefficient of friction x Normal Force = 39.749 lbf

The results from MASTODON are in agreement with the theoretical calculations.
