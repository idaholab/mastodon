### Friction Pendulum<sup>TM</sup> Isolator

`ComputeFPIsolatorElasticity` material simulates the behavior of a single concave Friction Pendulum<sup>TM</sup> bearing in MASTODON, and is used with a two-noded link type element. For each subdomain (that may contain multiple link elements), the material properties are defined by creating material blocks: [`ComputeIsolatorDeformation`](source/materials/ComputeIsolatorDeformation.md) and [`ComputeFPIsolatorElasticity`](source/materials/ComputeFPIsolatorElasticity.md),
and the kernel block, [`StressDivergenceIsolator`](source/materials/StressDivergenceIsolator,md).
A sample definition of the kernel and the material block is shown in the input file below.

!listing test/tests/materials/fp_isolator/fp_isolator_shear_PTV.i start=[Kernels] end=[Postprocessors]

A description of each of the input parameters is provided [here](source/materials/ComputeFPIsolatorElasticity.md). To model the co-efficient of sliding friction as a function of the axial pressure, sliding velocity, and the instantaneous temperature, the following switch parameters are used:

`pressure_dependent = true` <br/>
`velocity_dependent = true` <br/>
`temperature_change = true` <br/>

The axial and rotational terms of the basic stiffness matrix of the bearing element can be specified using the optional parameters: `k_x, k_xx, k_yy, k_zz`. A default value `10e13` is assumed when SI units are used. In the pressure factor computation `kp`, the instantaneous pressure and reference pressure values are in MPa. The user is required to provide information on the current unit system used in the material parameter `unit`, so that corresponding conversion factors are computed. The behavior of the FP bearing element in the shear direction is demonstrated using the examples described below. Analysis results are compared with verified and validated numerical models implemented in the open-source finite element package OpenSees [citep:opensees2009].

#### Example: Response to the cyclic loading in shear

The input file below demonstrates the response of the FP bearing subjected to cyclic loading in the shear direction. Three test cases: where co-efficient of friction as a function of the axial pressure only, co-efficient of friction as a function of the sliding velocity only, and coefficient of friction as a function of the instantaneous temperature only, are defined. A sinusoidal displacement history of [fig:FP_Staticload] is applied to the top node and the bottom node is restrained in the six degrees-of-freedom for all the three test cases. A constant axial load of 6285710 N is applied to the top node to simulate the weight of the superstructure.

!listing test/tests/materials/fp_isolator/fp_isolator_shear_PTV.i

!media media/materials/fp_isolator/fp_staticload.png
       style=width:45%;margin-left:250px;float:center;
       id=fig:FP_Staticload
       caption=Displacement loading history along the shear direction of the bearing.

The response of the FP bearing to cyclic loading in the shear direction is shown in [fig:FP_Shearresponse]

!media media/materials/fp_isolator/fp_staticresponse.png
       style=width:75%;margin-left:115px;float:center;
       id=fig:FP_Shearresponse
       caption=Response of the FP bearing to the prescribed cyclic loading in shear.

#### Example: Response to the seismic loading

The input file below describes the response of the FP bearing to seismic excitation. A superstructure
mass of 640745 kg is assumed for the purpose of this example, and is assigned to the top node of the bearing using a NodalKernel of type, [`NodalTranslationInertia`](source/nodalkernels/NodalTranslationalInertia.md). Three components of ground motion are applied at the bottom node as input accelerations, using the [`PresetAcceleration`](source/bcs/PresetAcceleration.md) BC. An axial load is applied on the bearing to account for the weight of superstructure. For the initial few time steps, where gravity load is being applied, inertia objects are disabled using `control` block to avoid any dynamic effects.

!listing test/tests/materials/fp_isolator/fp_isolator_seismic_PTV.i

The figures below show the response of the FP bearing to the prescribed seismic loading. The local co-ordinate system for the FP bearing can be identified [here](manuals/include/materials/fp_isolator-theory.md#physical_model).

!media media/materials/fp_isolator/fp_seismicshearY.png
       style=width:100%;float:left;
       id=fig:fp_SeismicShearY
       caption=Response of the FP bearing in the shear (local y) direction to the prescribed seismic loading ($\mu = f(P,V,T)$).

!media media/materials/fp_isolator/fp_seismicshearZ.png
       style=width:100%;float:left
       id=fig:fp_SeismicShearZ
       caption=Response of the FP bearing in the shear (local z) direction to the prescribed seismic loading ($\mu = f(P,V,T)$).
