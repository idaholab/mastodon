### Lead-rubber (LR) isolator

The LR isolator material simulates an LR bearing using the formulation developed
by [citet:manishkumarmceer2015]. This material model should be used with
two-noded link elements with three degrees of freedom at each node. A detailed
description of the formulation and the various behaviors included in the
formulation is provided in the [theory manual](manuals/theory/index.md).
The following input blocks should be created in order to model the LR isolator
material in MASTODON. Input parameters and the syntax for these blocks can be
found in their corresponding syntax pages in the links below.

- [`ComputeIsolatorDeformation`](source/materials/ComputeIsolatorDeformation.md)
- [`ComputeLRIsolatorElasticity`](source/materials/ComputeLRIsolatorElasticity.md)
- [`StressDivergenceIsolator`](source/kernels/StressDivergenceIsolator.md)

A sample input for these blocks is below.

!listing test/tests/materials/lr_isolator/lr_isolator_axial_ct.i start=[Kernels] end=[Postprocessors]

#### Limitations

- Currently, this formulation is limited to small rigid deformations in the isolator. This is because
  the isolator deformations are transformed from the global to local coordinate systems using a
  transformation matrix that is calculated from the initial position of the isolator. However,
  seismic isolators are typically placed between two large concrete slabs (which are almost
  rigid) and therefore the rigid body rotations in the isolators are usually negligible.
- The post-buckling behavior of the isolator is modeled using a very small axial stiffness ($1/1000^{th}$) of
  initial stiffness, in order to avoid numerical convergence problems.
- The mass of the LR bearing is not modeled in this material model. The user can model the mass of
  the isolators using NodalKernels, and following a procedure similar to that shown in modeling the
  superstructure mass in the examples above.

#### Example 1: Response to axial loading

The input file below demonstrates the response of the LR isolator element in the axial direction. A
cyclic displacement is applied at top node of the isolator in axial (local X) direction while the
bottom node is constrained in all six degrees of freedom. Additionally, the rotational degrees of freedom at
the top node are also constrained. In order to simulate the coupled (shear and axial) response,
a ramp displacement is applied in shear (local Y) direction at top node.

!listing test/tests/materials/lr_isolator/lr_isolator_axial_ct.i

The axial loading and response of the LR isolator for cyclic loading in axial direction is shown in [fig:axialresponse] below.

!media media/materials/lr_isolator/axialresponse.png
       style=width:100%;float:left;
       id=fig:axialresponse
       caption=Axial response of the LR isolator under cyclic loading.

#### Example 2: Response to shear loading

The input file below demonstrates the response of the LR isolator in shear. A sinusoidal displacement
is applied at top node of the isolator in shear (local Y direction). The isolator element
has the same set of constraints as in Example 1. In order to simulate the coupling between
the shear and axial responses, a constant axial load is applied on the isolator in
local X direction to simulate the weight of the superstructure.

!listing test/tests/materials/lr_isolator/lr_isolator_shear.i start=[BCs] end=[Preconditioning]

The loading profile and response of the LR isolator for cyclic loading in shear is shown in
[fig:shearresponse] below.

!media media/materials/lr_isolator/shearresponse.png
      style=width:100%;float:left;
      id=fig:shearresponse
      caption=Shear response of the LR isolator under cyclic loading.

#### Example 3: Response to seismic loading

The following input file simulates the response of the LR isolator under seismic loading. A superstructure
mass of 146890 kg is assumed in this example, and is assigned to the top node of the isolator using a NodalKernel
of type, [`NodalTranslationInertia`](source/nodalkernels/NodalTranslationalInertia.md).
The three components of ground motion are applied at the bottom node as
input accelerations, using the [`PresetAcceleration`](source/bcs/PresetAcceleration.md)
BC. It should be noted that a small time step in range of
0.0005-0.0001 sec is required for seismic analysis, especially for accuracy in the axial direction. It is
recommended to iteratively reduce the time step until the change in the results is within a desired
tolerance.

!listing test/tests/materials/lr_isolator/lr_isolator_seismic.i start=[BCs] end=[Preconditioning]

The following figures shows the response of LR isolator under seismic loading.

!media media/materials/lr_isolator/SeismicAxial.png
       style=width:100%;float:left;
       id=fig:SeismicAxial
       caption=Axial response of lead rubber bearing under seismic loading.

!media media/materials/lr_isolator/SeismicShearY.png
       style=width:100%;float:left;
       id=fig:SeismicShearY
       caption=Response of lead rubber bearing in shear (local y) direction under seismic loading.

!media media/materials/lr_isolator/SeismicShearZ.png
       style=width:100%;float:left
       id=fig:SeismicShearZ
       caption=Response of lead rubber bearing in shear (local z) direction under seismic loading.
