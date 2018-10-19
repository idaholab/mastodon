### Lead-Rubber Isolator id=lrisolator

The lead-rubber (LR) isolator material should be used with two-noded beam or link type elements in MASTODON.
For each subdomain (which may contain multiple link elements), the material properties are
defined by creating material blocks, [`ComputeIsolatorDeformation`](source/materials/ComputeIsolatorDeformation.md) and [`ComputeLRIsolatorElasticity`](source/materials/ComputeLRIsolatorElasticity.md),
and the kernel block, [`StressDivergenceIsolator`](source/kernels/StressDivergenceIsolator.md).
A sample definition of the kernel and material block is shown in the input file below.

!listing test/tests/materials/lr_isolator/lr_isolator_axial_ct.i start=[Kernels] end=[Postprocessors]

A description of each of the input parameters is provided in the syntax description below.
Additionally, the behavior of the LR isolator element in the axial and shear directions is demonstrated
using the three examples described below. Analysis results are compared with verified
and validated numerical models implemented in the commercial software code, ABAQUS [citet:abaqus2016].

#### Example 1: Response to axial loading id=axial_example

The input file below demonstrates the response of the LR isolator element in the axial direction. A
cyclic displacement is applied at the top node of the isolator in the axial (local X) direction while the
bottom node is restrained in all six degrees of freedom. To simulate the coupled (shear and axial) response, a ramped displacement is applied at the top node in the shear (local Y) direction.

!listing test/tests/materials/lr_isolator/lr_isolator_axial_ct.i

The response of the LR isolator to cyclic loading in the axial direction is shown in [fig:axialresponse].

!media media/materials/lr_isolator/axialresponse.png
       style=width:100%;float:left;
       id=fig:axialresponse
       caption=Axial response of the LR isolator to the prescribed cyclic loading.

#### Example 2: Response to shear loading id=shear_example

The input file below simulates the response of the LR isolator element in the shear direction. A sinusoidal displacement is applied at the top node of the isolator in the shear (local Y) direction. The isolator element
has the same set of constraints as in Example 1. A constant axial load is applied to the isolator in the axial direction to simulate the weight of the superstructure.

!listing test/tests/materials/lr_isolator/lr_isolator_shear.i start=[BCs] end=[Preconditioning]

The response of the LR isolator to cyclic loading in the shear direction is shown in [fig:shearresponse].

!media media/materials/lr_isolator/shearresponse.png
      style=width:100%;float:left;
      id=fig:shearresponse
      caption=Shear response of the LR isolator to the prescribed cyclic loading.

#### Example 3: Response to seismic loading id=seismic_example

The input file below simulates the response of the LR isolator to seismic loading. A superstructure
mass of 146890 kg is assumed in this example, and is assigned to the top node of the isolator using a NodalKernel
of type, [`NodalTranslationInertia`](source/nodalkernels/NodalTranslationalInertia.md). Three components of
ground motion are applied at the bottom node as input accelerations, using the [`PresetAcceleration`](source/bcs/PresetAcceleration.md) BC. A small time step in the range of 0.0005-0.0001 sec is required for seismic analysis, especially for accuracy in the axial direction. It is
recommended to iteratively reduce the time step till the change in the results is within a desired
tolerance.

!listing test/tests/materials/lr_isolator/lr_isolator_seismic.i start=[BCs] end=[Preconditioning]

The figures below show the response of the LR isolator to the prescribed seismic loading.

!media media/materials/lr_isolator/SeismicAxial.png
       style=width:100%;float:left;
       id=fig:SeismicAxial
       caption=Axial response of the LR isolator to the prescribed seismic loading.

!media media/materials/lr_isolator/SeismicShearY.png
       style=width:100%;float:left;
       id=fig:SeismicShearY
       caption=Response of the LR isolator in the shear (local y) direction to the prescribed seismic loading.

!media media/materials/lr_isolator/SeismicShearZ.png
       style=width:100%;float:left
       id=fig:SeismicShearZ
       caption=Response of the LR isolator in the shear (local z) direction to the prescribed seismic loading.
