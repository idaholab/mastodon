### Nonlinear Fluid Viscous Damper

[`ComputeFVDamperElasticity`](source/materials/ComputeFVDamperElasticity.md) material is used to simulate the hysteretic response of a nonlinear fluid viscous damper (FVD) and  is used with a two-noded link type elements in MASTODON. For each subdomain (that may contain multiple link elements), the material properties are defined by creating the material block,  [`ComputeFVDamperElasticity`](source/materials/ComputeFVDamperElasticity.md), and the kernel block, [`StressDivergenceDamper`](source/kernels/StressDivergenceDamper). Sample information is shown in the input file below.

!listing test/tests/materials/fv_damper/fv_damper_axial.i start=[Kernels] end=[Postprocessors]

A description of each of the input parameters is provided in the syntax description. Additionally, the response of the nonlinear FVD to the static and seismic loading is demonstrated using the examples described below. Analysis results are compared with the verified and validated numerical models implemented in the open source finite element package, OpenSees ([citet:opensees2009]).

#### Example: Response to the static loading

The input file below describes the analysis of a nonlinear FVD to a prescribed cyclic loading along its axis. A sinusoidal displacement as shown in [fig:fv_staticload], with increasing amplitude of 12, 24 and 36 mm, at a frequency of 0.5Hz is applied to the top node of the damper in the axial direction, and the bottom node is restrained in all the three degrees-of-freedom.

!listing test/tests/materials/fv_damper/fv_damper_axial.i

!media media/materials/fv_damper/fv_staticload.png
       style=width:50%;margin-left:25%;float:center;
       id=fig:fv_staticload
       caption=Displacement loading history along the axial direction of the damper.

Four test cases are run for varying $C_d$ and $\alpha$, and the axial stiffness ($k$) of the damper is held constant.The values presented below are in SI units. The response of the nonlinear FVD element to this prescribed cyclic loading in the axial direction is shown in [fig:fv_staticresponse]

!media media/materials/fv_damper/fv_staticresponse.png
       style=width:100%;float:left;
       id=fig:fv_staticresponse
       caption=Axial response of the nonlinear FVD to the prescribed cyclic loading for varying $C_d$ and $\alpha$.

#### Example: Response to the seismic loading

The input file below, demonstrates the seismic response a single bay portal frame installed with a diagonal nonlinear FVD element. The beam-columns are modeled using Euler-Bernoulli beam formulation and are assumed to behave linear elastic. A total mass of 50 tons is lumped at the floor level nodes to simulate the reactive mass. [LineElementMaster](/LineElementMaster/index.md) is used to simplify the input file syntax for creating beam and column elements. The mesh adopted for beam and column elements is identified in [fig:fv_seismicframe].

!listing test/tests/materials/fv_damper/fv_damper_seismic_frame.i

!media media/materials/fv_damper/fv_seismicframe.png
       style=width:60%;margin-left:25%;float:center;
       id=fig:fv_seismicframe
       caption=Mesh geometry for the portal frame installed with a diagonal nonlinear FVD element.

The ground acceleration is applied at the bottom nodes of the frame using [`PresetAcceleration`](source/bcs/PresetAcceleration.md) boundary condition. The hysteretic response of the nonlinear FVD to the prescribed seismic loading is shown in [fig:fv_seismicframeresponse]. The response is benchmarked using the results obtained from OpenSees.

!media media/materials/fv_damper/fv_seismicframeresponse.png
       style=width:60%;margin-left:25%;float:center;
       id=fig:fv_seismicframeresponse
       caption=Nonlinear hysteretic response of the nonlinear FVD element to the prescribed seismic loading.
