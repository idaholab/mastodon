### Lead-Rubber Isolator

The Lead-Rubber isolator (LR isolator) material should be used with two-noded beam or link type elements in MASTODON.
For each subdomain (which may contain multiple link elements), the material properties are
defined by creating material blocks, [`ComputeIsolatorDeformation`](source/materials/ComputeIsolatorDeformation.md) and [`ComputeLRIsolatorElasticity`](source/materials/ComputeLRIsolatorElasticity.md),
and the kernel block, [`StressDivergenceIsolator`](source/materials/StressDivergenceIsolator,md).
A sample definition of the kernel and material blocks is shown in the input file below.

!listing test/tests/materials/lr_isolator/lr_isolator_axial_ct.i start=[Kernels] end=[Postprocessors]

A description of each of the input parameters is provided in the syntax description below.
Additionally, the behavior of the element in axial and shear directions is demonstrated
through three examples described below. The analysis results are compared with verified
and validated numerical models implemented in the commercial software code, ABAQUS [citet:abaqus2016].

#### Example 1: Response to axial loading id=axial_example

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

#### Example 2: Response to shear loading id=shear_example

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

#### Example 3: Response to seismic loading id=seismic_example

The following input file simulates the response of the LR isolator under seismic loading. A superstructure
mass of 146890 kg is assumed in this example, and is assigned to the top node of the isolator using a NodalKernel
of type, [`NodalTranslationInertia`](source/nodalkernels/NodalTranslationalInertia.md). The three components of
ground motion are applied at the bottom node as input accelerations, using the
[`PresetAcceleration`](source/bcs/PresetAcceleration.md) BC. It should be noted that a small time step in range of
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
