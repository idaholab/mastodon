# Example 9: Soil-structure interaction analysis of an undamped rigid structure on undamped linear soil

!alert note title=Units of this example
Tons, Kilo Newtons, meters, and seconds.

## Model description

This example demonstrates a fully-coupled soil-structure interaction (SSI) analysis of an undamped simplified beam element structure that is representative of a nuclear power plant. The soil domain is uniform throughout with plan dimensions 75 $m$ X 75 $m$ and a depth of 29.47 $m$. The soil behavior is assumed to be linear with a shear modulus 46282.5 $KN/m^2$, density 1.7 $Tons/m^3$, and Poisson's ratio 0.3. The foundation is assumed to be rigid with plan dimensions 13.8 $m$ X 13.8 $m$ and a thickness of 1.4 $m$. The foundation is embedded in the soil domain such that its top surface is flush with the top of the soil. The rigid structure is 10 $m$ tall with a lumped mass of 166 $Tons$ at the top. Rigidity of the foundation and the structure is simulated by using a large value for the stiffness and the moment of inertia (if applicable). Both the foundation and the structure have a density 2.7 $Tons/m^3$. More details can be found in [!citet](Bolisetti2018).

## Input ground motion

The bottom surface of the soil domain is subjected to an Ormsby wavelet as an acceleration input. An Ormsby wavelet is defined by four parameters: low cutoff frequency $(f_1)$, low frequency $(f_2)$, high frequency $(f_3)$, and high cutoff frequency $(f_4)$. The Ormsby wavelet has a constant Fourier amplitude between $f_2$-$f_3$, and it tapers to zero amplitudes at $f_1$ and $f_4$. In the time domain, the Wavelet inherently has an amplitude of unity, but it can be scaled to any desired value. In this example, $f_1, f_2, f_3,$ and $f_4$ are 0, 0.2, 20, and 25 $Hz$, respectively. The Wavelet has an amplitude of 0.5$g$ in the time domain and is centered at 1 $sec$. [fig:Ormsby] presents the acceleration time history of the Ormsby wavelet used. More information on this Wavelet is provided in Appendix B of [!citet](BolisettiThesis).

!media media/examples/ex09_Ormsby.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Ormsby
       caption=Ormsby wavelet input ground motion used for the analysis.

## Finite element mesh generation

The three-dimensional finite element mesh is generated using Cubit 15.5b. The procedure for generating the mesh is briefly described as follows:

- The soil domain is first generated using the `Create volumes` option.
- The foundation is also created using the same option and it is positioned such that its top surface is flush with the soil surface. This can be achieved using the `move` command.
- A groove is made in the soil domain to accommodate the foundation using the `Webcut > Tool` option. Then, the common surfaces between the soil and the foundation are merged using the `merge all` command. Now, the foundation and the soil are in continuum.
- The vertical structure is connected to the foundation through rigid horizontal beams so that rotations of this vertical structure are now constrained by the rigidity of the horizontal beams.
- The vertical structure is represented by longitudinally connected beams whose total length is equal to required length of the structure.

The soil domain is meshed using 2m cubic elements which capture frequencies less than about 50 Hz. While this results in a somewhat coarser mesh, the computational burden will still be manageable. The foundation and soil share a continuous mesh and the foundation-soil interface is therefore tied, and does not allow any sliding or separation. Rigid foundation meshing should be less of a concern in relation to the flexible soil meshing. While each horizontal connector beam is meshed into a single element, the vertical beam is meshed into five elements. In total, the model has 21,685 elements with 24,358 nodes. This model is presented in [fig:Mesh].

!media media/examples/ex09_Mesh.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Mesh
       caption=Cubit model for SSI analysis.

## Modeling in MASTODON

This example consists of three individual analysis efforts for: (i) fixed-base structure without soil domain; (ii) site response analysis in the absence of the structure; (iii) soil structure interaction

### Fixed-base structure analysis

The input file for simulating the individual structural response is presented below. The vertical beam representing the structure has a mass lumped at the top, in addition to having a density along its length. The horizontal beams connected to the rigid foundation are massless and they simulate a rigid connection with the foundation. The rigidity of both the vertical and the horizontal beams is achieved by setting properties such as area, moment of inertia, and Young's modulus to very large values. The rigidity of the foundation is achieved by setting its Young's modulus to a large value. The Ormsby wavelet is applied at the bottom of the rigid foundation and the response at the top of the structure is recorded. [fig:Structure] presents the 1%-damped acceleration response spectra at the top of the structure. It is observed that, since the structure is rigid, its response spectra exactly matches with that of the input Ormsby wavelet.

!listing examples/ex09/Structure_Response.i

!media media/examples/ex09_Structure.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Structure
       caption=Acceleration response spectra of the fixed base structure subjected to the Ormsby wavelet.

### Site response analysis

The input file for simulating the individual soil response is presented below. The uniform soil domain has a shear wave velocity of $165$ $m/s$. Although the soil is uniform throughout, the [`UniformLayerAuxKernel`](source/auxkernels/UniformLayerAuxKernel.md) is used to define the soil layers. This is necessary when using the [`ComputeIsotropicElasticityTensorSoil`](source/materials/ComputeIsotropicElasticityTensorSoil.md) material. Alternatively, if the [`ComputeIsotropicElasticityTensor`](source/materials/ComputeIsotropicElasticityTensor.md) is used for a uniform soil, the `UniformLayerAuxKernel` is not necessary. In addition, periodic boundary conditions are applied to the two opposite faces of the soil domain, which are perpendicular to the Z-axis to ensure that they move together. This is achieved using the [`Periodic`](source/actions/AddPeriodicBCAction.md) option in the boundary conditions. The Ormsby Wavelet is applied at the bottom of the soil block and the response is recorded at the top. The 1% damped acceleration response spectrum at the top of the soil is presented below. The periods at which the response spectrum peaks are the fundamental periods of the soil domain. The periods inferred from this figure are very similar to those predicted by the theoretical solution $4H/((2n-1)~Vs)$, where $H$ is the depth of soil and $n$ is the index of the period of interest.

!listing examples/ex09/Soil_Response.i

!media media/examples/ex09_Soil.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Soil
       caption=Acceleration response spectra of the soil subjected to the Ormsby wavelet.

### Soil-structure interaction analysis

The input file for simulating the SSI response is presented below. The amplification ratio of the structural response spectra at 10m height from the free-field soil is computed. Free-field soil is the soil region unaffected by the structural response, which, in this case, is a corner node on the surface of the soil domain. [fig:SSI_Amp] presents the amplification ratio as a function of the period.

!listing examples/ex09/SSI_Response.i

!media media/examples/ex09_SSI.png
       style=width:80%;margin-right:0px;float:center;
       id=fig:SSI_Amp
       caption=Amplification ratio of the structural response spectra from the free-field soil in SSI analysis. This is computed using MASTODON.

[fig:SSI_FreeField] presents the free-field soil acceleration response (1% damped) from the SSI analysis, compared to that obtained from the site response analysis. It is seen that these to spectra are very similar. The subtle differences are likely due to the coarse mesh size and small domain extent for the soil-structure interaction problem.

!media media/examples/ex09_SSI_FreeField.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:SSI_FreeField
       caption=Free-field response spectra from the SSI analysis and the individual soil analysis.

!bibtex bibliography
