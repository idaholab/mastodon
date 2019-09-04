# Example 3: Nonlinear Site-Response Analysis

This example demonstrates a nonlinear site-response analysis using I-soil. A 20 m soil column is
created and meshed to have 20 elements, each measuring 1 m x 1 m x 1 m. Shear wave velocity (and the
small-strain shear modulus) is depth dependent and has a value of 100 m/sec at top and 250 m/sec at
the bottom. [fig:1dsoilcolumn] demonstrates the geometry of the problem.

!media media/examples/1dsoilcolumn.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:1dsoilcolumn
       caption=Schematic of the non-uniform shear beam model (reprinted from
               [!cite](baltaji2017nonlinear)).

Below is the full input file provided to run this analysis.

!listing examples/ex03/shear_beam_Isoil_free_field.i

The first step is to create the problem geometry. For simpler geometries, MASTODON provides an
automeshing option and has the following block to generate the mesh described in [fig:1dsoilcolumn] :

!listing examples/ex03/shear_beam_Isoil_free_field.i
         start=Mesh
         end=GlobalParams

Following the Mesh block, GlobalParams, Variables, AuxVariables, Kernels, and AuxKernels are defined
and detailed information about these blocks can be found at [Getting Started](manuals/user/index.md).

The shear beam model is fixed at gravity direction (z), the nodes at the same elevation are
constrained to move equally via periodic boundary condition and acceleration time history is applied
at the bottom of the column in x direction using following inputs:

!listing examples/ex03/shear_beam_Isoil_free_field.i
         start=BCs
         end=Functions

In the above input block, PresetAcceleration type boundary condition is used to define the base
excitation. Function of "function=accel_bottom" is used to input the acceleration time history and
this function is defined as:

!listing examples/ex03/shear_beam_Isoil_free_field.i
         start=Functions
         end=Materials

The function block also includes the gravity (9.81 m/s$^2$), depth and density (2000 ton/m$^3$)
dependent functions for initial stress to create in-situ soil conditions. Lateral stresses are
defined using K0 conditions (lateral earth pressure coefficient at rest) and for this example it is
calculated as K0 = Poisson's Ratio / (1-Poisson's ratio) where Poisson's ratio is chosen to be
0.3. Once the base excitation and initial stresses are defined, the material definition block is used
to create the soil properties for each layer as:

!listing examples/ex03/shear_beam_Isoil_free_field.i
         start=Materials
         end=Preconditioning

The above input uses a strength-controlled backbone curve (using GQ/H model) with 100 shear
stress-strain points and a small-strain (i.e., initial) shear modulus $G_0$ compatible with the shear
wave velocity $(V_s)$ profile presented in [fig:1dsoilcolumn]. The initial shear modulus - shear wave
velocity compatibility is ensured by calculating $G_0$ as $\rho$ * $(V_s)^2$. The reference pressure
$ p_{ref} $ is calculated at each layer and corresponds to mean effective confining pressure at which
the dynamic soil properties are defined.

!media media/examples/MRBB.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:MRBB
       caption=Modulus reduction and backbone curves (reprinted from Baltaji et al. 2017).

In this example, since the model does not exhibit a significant change in the mean effective stress
during shaking, the pressure dependency parameters are set to be $ a_0 $ = 1, $ a_1 $ = 0, $ a_2 $ =
0, and $b$ = 0 so that the soil behavior is pressure independent.

Finally, the execution and output blocks are provided to run the analysis and extract the data using
following chain of blocks (see [Getting Started](manuals/user/index.md)):

!listing examples/ex03/shear_beam_Isoil_free_field.i
         start=Preconditioning

Accelerations obtained at the bottom, mid-depth and top of the soil columns are used to calculate the
spectral response for 5 percent damping, the results are compared to analogous model developed in
DEEPSOIL and presented in

!media media/examples/shearbeam.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:shearbeam
       caption=Comparison of spectral accelerations from MASTODON and DEEPSOIL (modified from
               [!cite](baltaji2017nonlinear)).

Further details regarding to application of MASTODON to simulate seismic site response and soil
structure interaction analysis can be found in [!citet](baltaji2017nonlinear).

## Implementation using Automatic Differentiation

This example can also be run using Automatic Differentiation (AD). Automatic Differentiation
enables the computation of a numerical Jacobian that is almost identical to the theoretical
Jacobian of a kernel. This results in much quicker convergence, even for highly nonlinear
problems. Therefore, `solve_type=NEWTON` can be used in the executioner, which uses
the full Jacobian unlike `solve_type=PJFNK`, which is recommended when only an approximate
estimate of the Jacobian is available. In MASTODON, the iSoil material model has been
converted to AD and the usage of the AD version of iSoil is demonstrated below.  
More information can be found in [ISoil material](source/materials/ADComputeISoilStress.md).

The following two modifications to the input file should be made to run this example using AD.
These modifications ensure the AD versions of the stress divergence kernels and the
material kernels are being used during the solve. Add 'use_automatic_differentiation = true'
in 'Kernels/DynamicTensorMechanics':

!listing examples/ex03/shear_beam_Isoil_free_field_AD.i
         start=Kernels
         end=AuxKernels

Also, add 'use_automatic_differentiation = true' in 'Materials/I_Soil/soil_all':

!listing examples/ex03/shear_beam_Isoil_free_field_AD.i
         start=Materials
         end=Preconditioning

The plots below present the accelerations and response spectra at the top soil computed
using AD and no-AD. The AD and the no-AD results quite closely match.

!media media/examples/AD_noAD_Comp.png
       style=width:90%;float:center;
       id=fig:ad_acc_comp
       caption=Comparison of accelerations at the soil surface generated using AD and no-AD.

!media media/examples/AD_noAD_Comp_RS.png
       style=width:90%;float:center;
       id=fig:ad_rs_comp
       caption=Comparison of response spectra at the soil surface generated using AD and no-AD.

!bibtex bibliography
