# Example 9: Soil Structure Interaction analysis of a rigid structure on linear soil without considering damping

!alert note title=Units of this example
Tons, Newtons, meters, and seconds.

## Model Description

This example demonstrates a fully coupled Soil Structure Interaction (SSI) analysis of a rigid nuclear structure without considering damping. The soil domain is uniform throughout with plan dimensions 75 $m$ X 75 $m$ and a depth of 29.47 $m$. The soil behavior is assumed to be linear with a shear modulus 46282.5 $KN/m^2$, density 1.7 $Tons/m^3$, and Poisson's ratio 0.3. The foundation is assumed to be rigid with plan dimensions 13.8 $m$ X 13.8 $m$ and a thickness of 1.4 $m$. The foundation is embedded in the soil domain such that its top surface is flush with the top of the soil. The rigid structure is 10 $m$ tall with a lumped mass of 166 $Tons$ at the top. Both the foundation and the structure have a density 2.7 $Tons/m^3$. More details can be found in [!citet](Bolisetti2018).

## Input ground motion

The bottom surface of the soil domain is subjected to an Ormsby Wavelet as an acceleration input. An Ormsby Wavelet is defined by four parameters: low cutoff frequency $(f_1)$, low frequency $(f_2)$, high frequency $(f_3)$, and high cutoff frequency $(f_4)$. The Ormsby Wavelet has a constant Fourier amplitude between $f_2$-$f_3$, and it tapers to zero amplitudes at $f_1$ and $f_4$. In the time domain, the Wavelet inherently has an amplitude of unity, but it can be scaled to any desired value. In this example, $f_1, f_2, f_3,$ and $f_4$ are 0, 0.2, 20, and 25 $Hz$, respectively. The Wavelet has an amplitude 0.5$g$ in the time domain and is centered at 1 $sec$. The figure below shows the acceleration time history of the Ormsby Wavelet used. More information on this Wavelet is provided in Appendix B of [!citet](BolisettiThesis).

!media media/examples/ex09_Ormsby.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Ormsby
       caption=Ormsby wavelet input ground motion used for the analysis.

## Finite element mesh generation

The three dimensional finite element mesh is generated using Cubit 15.5b. The procedure followed for generating the mesh is briefly described:

- The soil domain is first generated using the `Create volumes` option.
- The foundation is also created using the same option and it is positioned such that its top surface is flush with the soil surface. This can be achieved using the `move` command.
- A groove is made in the soil domain to accommodate the foundation using the `Webcut > Tool` option. Then, the common surfaces between the soil and the foundation are merged using the `merge all` command. Now, the foundation and the soil are in continuum.
- The vertical structure is connected to the foundation through rigid horizontal beams so that rotations of this vertical structure are now constrained by the rigidity of the horizontal beams.
- The vertical structure is represented by a beam of the required length.

The soil domain is meshed such that each element is approximately a cube of side 2 $m$. While this results in a somewhat coarser mesh, the computational burden will still be manageable. The foundation is meshed such that it compatible with the soil meshing. Rigid foundation meshing should be less of a concern in relation to the flexible soil meshing. While each horizontal connector beam is meshed into a single element, the vertical beam is meshed into five elements. In total, the model has 21,685 elements with 24,358 nodes. This model is presented in the figure below.

!media media/examples/ex09_Mesh.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Mesh
       caption=Cubit model for SSI analysis.

## Modeling in MASTODON

This example consists of three individual analysis efforts: (i) fixed-base structure without soil domain; (ii) site response analysis in the absence of the structure; (iii) soil structure interaction

### Fixed base structure analysis

The input file for simulating the individual structural response is presented below. The vertical beam representing the structure has a mass lumped at the top in addition to having a density along its length. The horizontal beams connected to the rigid foundation are massless and they facilitate a rotational degree of freedom for the vertical beam at the node where it is intersecting with the foundation. The rigidity of both the vertical and the horizontal beams is achieved by setting properties such as area, moment of inertia, and Young's modulus to high values. The rigidity of the foundation is achieved by setting its Young's modulus to a high value. The Ormsby Wavelet was applied at the bottom of the rigid foundation and the response at the top of the structure was recorded. [fig:Structure] presents the 1% damped acceleration response spectra at the structure top and it is observed that the fundament period of about 0.23 $sec$. This value shows a good agreement with that of the LS-Dyna model by [!citet](BolisettiThesis).

!listing examples/ex09/Structure_Response.i

!media media/examples/ex09_Structure.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Structure
       caption=Response spectra of the fixed base structural response under the Ormsby Wavelet.

### Site response analysis

The input file for simulating the individual soil response is presented below. The uniform soil domain has a shear wave velocity of $165$ $m/s$. Although the soil is uniform throughout, the `UniformLayerAuxKernel` is used to specify the top surface of the soil. This is necessary when using the `ComputeIsotropicElasticityTensorSoil` material. Alternatively, if the `ComputeIsotropicElasticityTensor` is used for a uniform soil, the `UniformLayerAuxKernel` is not necessary. In addition, periodic boundary conditions are applied to the two opposite faces of the soil domain which are perpendicular to the Z axis to ensure that they move in unison. This achieved using the `Periodic` option in `BCs`. The Ormsby Wavelet is applied at the bottom of the soil block and the response is recorded at the top. The 1% damped acceleration response spectrum at the top of the soil is presented below. The periods at which the response spectrum peaks are the fundamental periods of the soil domain. The periods inferred from this figure are very similar to those predicted by the theoretical solution $4H/((2n-1)~Vs)$, where $H$ is the depth of soil and $n$ is the index of the period of interest.

!listing examples/ex09/Soil_Response.i

!media media/examples/ex09_Soil.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Soil
       caption=Response spectra of the individual soil response under the Ormsby Wavelet.

### Soil Structure Interaction analysis

The input file for simulating the SSI response is presented below. The amplification ratio of the structural response spectra from the free-field soil is computed. Free-field soil is the soil region located far away from the structure, which in this case is one of the top end most nodes of the domain. [fig:SSI_Amp] presents the amplification ratio as a function of the period. [fig:SSI_Amp_Valid] presents the amplification ratio obtained by [!citet](BolisettiThesis) using LS-Dyna. It is seen that the variation of the amplification ratios with period is similar between MASTODON and LS-Dyna.

!listing examples/ex09/SSI_Response.i

!media media/examples/ex09_SSI.png
       style=width:80%;margin-right:0px;float:center;
       id=fig:SSI_Amp
       caption=Amplification ratio of the structural response spectra from the free-field soil in SSI analysis. This is computed using MASTODON.

!media media/examples/ex09_SSI_Valid.png
      style=width:80%;margin-right:0px;float:center;
      id=fig:SSI_Amp_Valid
      caption=Amplification ratio computed using LS-Dyna by [!citet](BolisettiThesis).

[fig:SSI_FreeField] presents the free-field soil acceleration response (1% damped) from the SSI analysis, compared to that obtained from the site response analysis. It is seen that these to spectra are very similar.

!media media/examples/ex09_SSI_FreeField.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:SSI_FreeField
       caption=Free-field response spectra from the SSI analysis and the individual soil analysis.

## Note on the computational effort required

When a $dt$ of 0.002$s$ is used, for a 3$s$ Ormsby Wavelet, this problem has 1500 time steps. It is not recommended to run this problem on your local machine and it is better to use a supercomputer. For details on how to use an INL supercomputer, please see [FAQ](help/faq.md). On FALCON machine with 1 full node, this problem takes about 3-3.5 hours to run.

!bibtex bibliography
