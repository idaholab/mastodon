# Example 3

This example is designated to conduct nonlinear site response analysis using I-soil. A 20 m soil column is created and meshed to have 20 elements each having 1 m x 1 m x 1 m dimensions. Shear wave velocity (so that the small strain shear modulus) is depth dependent and has a value of 100 m/sec at top and 250 m/sec at bottom. \ref{fig:1dsoilcolumn} demonstrates the geometry of the problem.

!media media/examples/1dsoilcolumn.png width=60% margin-left=150px float=center id=fig:1dsoilcolumn caption=Schematic of the non-uniform shear beam model (reprinted from Baltaji et al. 2017).

Below is the full input file provided to run this analysis.

!listing examples/ex03/shear_beam_Isoil_free_field.i

First step is to create the problem geometry. For simpler geometries, MASTODON provides automeshing option and has the following block to generate the mesh described in \ref{fig:1dsoilcolumn} :

!listing examples/ex03/shear_beam_Isoil_free_field.i start=Mesh end=GlobalParams

Following the Mesh block, GlobalParams, Variables, AuxVariables, Kernels, and AuxKernels are defined and detailed information about these blocks can be found at [Getting Started](manuals/user/index.md).

The shear beam model is fixed at gravity direction (z), the nodes at the same elevation are constrained to move equally via periodic boundary condition and acceleration time history is applied at the bottom of the column in x direction using following commands:

!listing examples/ex03/shear_beam_Isoil_free_field.i start=BCs end=Functions

In above block of commands, PresetAcceleration type boundary condition is used to define the bease excitation. Function of "function=accel_bottom" is used to input the acceleration time history and this function is defined as:

!listing examples/ex03/shear_beam_Isoil_free_field.i start=Functions end=Materials

The function block also includes the gravity (9.81 m/s$^2$), depth and density (2000 ton/m$^3$) dependent functions for initial stress to create in-situ soil conditions. Lateral stresses are defined using K0 conditions (lateral earth pressure coefficient at rest) and for this example it is calculated as K0 = Poisson's Ratio / (1-Poisson's ratio) where Poisson's ratio is chosen to be 0.3. Once the base excitation and initial stresses are defined, the material definition block is used to create the soil properties for each layer as:

!listing examples/ex03/shear_beam_Isoil_free_field.i start=Materials end=Preconditioning

The above command uses strength controlled backbone curve (using GQ/H model) with 100 shear stress - shear strain points along with small strain (i.e., initial) shear modulus $G_0$ that are compatible with shear wave velocity $(V_s)$ profile presented in \ref{fig:1dsoilcolumn}. The initial shear modulus - shear wave velocity compatibility is ensured by calculating $G_0$ as $\rho$ * $(V_s)^2$. The initial bulk modulus ($B_0$) is calculated using elastic relationship between $B_0$ and $G_0$ via Poisson's ratio.
The reference pressure $ p_{ref} $ is calculated at each layer and corresponds to mean effective confining pressure at which the dynamic soil properties are defined.

!media media/examples/MRBB.png width=60% margin-left=150px float=center id=fig:MRBB caption=Modulus reduction and backbone curves (reprinted from Baltaji et al. 2017).

For this example, since the model does not exhibit significant effective mean stress change throughout the shaking, the pressure dependency parameters set to be $ a_0 $ = 1, $ a_1 $ = 0, $ a_2 $ = 0, and $b$ = 0 so that the model is pressure independent.

Finally, the execution and output commands are provided to run the analysis and extract the data using following chain of blocks (see [Getting Started](manuals/user/index.md)):

!listing examples/ex03/shear_beam_Isoil_free_field.i start=Preconditioning

Accelerations obtained at the bottom, mid-depth and top of the soil columns are used to calculate the spectral response for 5 percent damping, the results are compared to analogous model developed in DEEPSOIL and presented in  

!media media/examples/shearbeam.png width=60% margin-left=150px float=center id=fig:shearbeam caption=Comparison of spectral accelerations from MASTODON and DEEPSOIL (modified from Baltaji et al. 2017).

Further details regarding to application of MASTODON to simulate seismic site response and soil structure interaction analysis can be found in \citet{baltaji2017nonlinear}.

\bibliographystyle{unsrt}
\bibliography{doc/bib/mastodon.bib}
