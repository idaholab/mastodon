# Example 11a: Fluid-structure interaction analysis of a tank subjected to uni-directional harmonic acceleration

!alert note title=Units of this example
Giga kilograms, Giga Newtons, meters, and seconds were used for the modeling. However, the
post-processed results maybe presented in different units.

## Model description

This example demonstrates the fluid-structure interaction between a steel tank and
the fluid inside it. The tank is cylindrical with height 2 $m$, thickness 7.9 $mm$,
 and radius 0.79 $m$. The fluid inside the tank is water with height 1.8 $m$. Steel has an elastic
modulus of 200 $GPa$ and a Poisson's ratio of 0.27. Water has a density of 1E-6 $Giga~kg/m^3$
 and a speed of sound of 1466.47 $m/s$. This tank with water is subjected to a harmonic
 acceleration in the x-direction. [fig:Tank] presents a schematic of the tank and its dimensions.

!media media/examples/SC_solns_tank.png
  style=width:40%;float:center;
  id=fig:Tank
  caption=Schematic of the tank and its dimensions.

## Input motion

The bottom of the tank is subjected to two cycles of sinusoidal acceleration with
peak amplitude of 0.025 $g$. The length of the input motion is about 4 $s$. [fig:input] presents
 a plot of the input motion.

!media media/examples/ex11a_input.png
 style=width:50%;float:center;
 id=fig:input
 caption=Input harmonic motion to the tank.

## Finite element mesh generation

The three-dimensional finite element mesh is generated using Cubit 15.5b. The following
 Cubit input file presents the script to generate the mesh. [fig:mesh] presents the finite-element mesh used for this example.

!listing /examples/ex11a/Model_1.jou

!media media/examples/ex11a_mesh.png
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

!listing examples/ex11a/FSI_ex11a.i

## Results: Pressures on the tank and wave heights over the fluid

Pressures acting on the tank wall as a function of time are recorded at the circular
red dot as illustrated in [fig:Tank]. Wave heights as the function of time are also
recorded at the triangular red dot as illustrated in [fig:Tank]. [fig:pressure] and [fig:wave]
 present the pressures and wave heights, respectively.

!row!

!media media/examples/ex11a_pressure.png
  style=width:40%;float:left;padding-top:2.5%;
  id=fig:pressure
  caption=Pressures recorded at the circular dot as illustrated in [fig:Tank].

!media media/examples/ex11a_wave.png
  style=width:40%;float:right;padding-top:2.5%;
  id=fig:wave
  caption=Wave heights recorded at the triangular dot as illustrated in [fig:Tank].

!row-end!

## Results: Comparison to a reference solution

[fig:comp] presents the peak pressure and peak wave height in comparison to a reference solution.
 It is observed that the modeled results match quite well with the reference solution.

!media media/examples/ex11a_comp.png
  style=width:50%;float:center;
  id=fig:comp
  caption=Comparison of the modeled results to a reference solution.
