# Example 12: Experimental validation of fluid-structure interaction simulations

!alert note title=Units of this example
Giga kilograms, Giga Newtons, meters, and seconds were used for the modeling. However, the
post-processed results maybe presented in different units.

This example compares the results of fluid-structure interaction simulations with
experimental results.

## Model description

The tank is cylindrical with height 2 $m$, thickness 7.9 $mm$,
 and radius 0.76 $m$. The fluid inside the tank is water with height 1.62 $m$. Steel has an elastic
modulus of 200 $GPa$ and a Poisson's ratio of 0.27. Water has a density of 1E-6 $Giga~kg/m^3$
 and a speed of sound of 1466.47 $m/s$. This tank with water is subjected to an earthquake
 acceleration in the x-direction and rotational acceleration about the y-direction.
[fig:Tank] presents a schematic of the tank and its dimensions.

!media media/examples/Mir_tank.png
  style=width:40%;float:center;
  id=fig:Tank
  caption=Schematic of the tank and its dimensions.

## Input motion

The bottom of the tank is subjected to an earthquake
acceleration in the x-direction and rotational acceleration about the y-direction.
The peak lateral and rotational accelerations are 0.13 $g$ and 0.28 $rad/s^2$. The length
 of the input motion is about 11 $s$. [fig:input] presents a plot of the input motion.

!row!

!media media/examples/ex12_lat_input.png
 style=width:45%;float:left;padding-top:2.5%;
 id=fig:pressure
 caption=Lateral acceleration input along x-direction.

!media media/examples/ex12_rot_input.png
 style=width:40%;float:right;padding-top:2.5%;
 id=fig:wave
 caption=Rotational acceleration input about y-direction.

!row-end!

## Finite element mesh generation

The three-dimensional finite element mesh is generated using Cubit 15.5b. The following
 Cubit input file presents the script to generate the mesh. [fig:mesh] presents the finite-element mesh used for this example. Note that for applying the rotational acceleration about the y-direction, nodes
 were created along the x-direction. Then, lateral acceleration in the z-direction is applied
 at these nodes to replicate application of rotational accelerations.

!listing /examples/ex12/FSI_ex12.jou

!media media/examples/ex12_mesh.png
 style=width:25%;float:center;
 id=fig:mesh
 caption=Finite-element mesh of the tank.

## Modeling using MOOSE modules

The tank with water is modeled using the capabilities of the [modules/fluid_structure_interaction/index.md] and the [modules/tensor_mechanics/index.md]. In particular,
the tank is modeled using the following kernels in the [modules/tensor_mechanics/index.md]:

- [`InertialForce`](InertialForce.md) kernel for the acceleration component
- [`DynamicTensorMechanics`](DynamicTensorMechanicsAction.md) action for the stiffness component

The fluid is modeled using the following kernels in the [modules/fluid_structure_interaction/index.md]:

- [`AcousticInertia`](AcousticInertia.md) kernel for the double time derivative of the fluid pressure
- [`Diffusion`](Diffusion.md) kernel for the Laplacian of the fluid pressure

Fluid-structure interaction between the tank and the fluid is modeled using the [`FluidStructureInterface_3D`](FluidStructureInterface_3D.md)
 interface kernel. This kernel enforces displacement and stress/pressure continuity
 between the tank and the fluid. The input file for executing the model is presented below.
 As mentioned before, lateral accelerations in the z-direction are applied at several
 nodes along the x-direction to replicate application of rotational accelerations about y-axis.

!listing examples/ex12/FSI_ex12.i

## Results: Pressures on the tank and wave heights over the fluid

Pressures acting on the tank wall as a function of time are recorded at the circular
red dot as illustrated in [fig:Tank]. Wave heights as the function of time are also
recorded at the triangular red dot as illustrated in [fig:Tank]. [fig:pressure] and [fig:wave]
 present the pressures and wave heights, respectively.

!row!

!media media/examples/ex12_pressure.png
  style=width:40%;float:left;padding-top:2.5%;
  id=fig:pressure
  caption=Pressures recorded at the circular dot as illustrated in [fig:Tank].

!media media/examples/ex12_wave.png
  style=width:43%;float:right;padding-top:2.5%;
  id=fig:wave
  caption=Wave heights recorded at the triangular dot as illustrated in [fig:Tank].

!row-end!

## Results: Comparison to experimental results

[fig:comp] presents the peak pressure and peak wave height in comparison to experimental results.
 It is observed that the modeled results match quite well with experiments.

!media media/examples/ex12_comp.png
  style=width:50%;float:center;
  id=fig:comp
  caption=Comparison of the modeled results to experimental results.
