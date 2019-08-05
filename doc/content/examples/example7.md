# Example 7: Demonstration of domain reduction method

## Model Description

This example demonstrates the use of the domain reduction method (DRM) in MASTODON. DRM was developed by [!citet](bielak2003domain) to improve the computational efficiency in source-to-site wave propagation problems. Problems involving fault rupture scenarios require large computational domains spanning many kilometers. If a structure of a width in the order of 40 m is to be designed to withstand the seismic waves generated from this fault rupture, it would be inefficient to simulate the entire fault rupture scenario for every small change in the structure's properties. To overcome this, DRM splits the problem into parts: (i) free-field response of the soil to the fault rupture is computed first, (ii) seismic forces along the DRM element layer is then transferred from the large model is part (i) to a much smaller computational domain containing just the structure and a part of the soil around it.  

In this example, rupture of a horizontal earthquake fault is modeled in a 1000 m x 500 m homogenous linear elastic soil domain [fig:DRMmodel] (a). The region within the gray DRM element layer is the smaller soil domain, which for this demonstration is assumed to have the exact same material properties as the larger soil domain [fig:DRMmodel] (d). This method requires the forces along the inner and outer boundary of the DRM element layer to be transferred from (a) to (d). To compute these forces, two intermediate models are required which are shown in [fig:DRMmodel] (b) and [fig:DRMmodel] (c). In the next few paragraphs, details of the input files corresponding to each model are described.

!media media/examples/DRM_model_description.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:DRMmodel
       caption=Illustration showing the larger model (a), the two intermediate models (b and c) and the smaller model (d) required for the domain reduction method. The white line in (a) is the horizontal earthquake fault and the gray region is the DRM element layer.

## Modeling in MASTODON

The input file corresponding to [fig:DRMmodel] (a) is shown below. Here, the earthquake fault rupture is modeled using the [SeismicSource](SeismicSource.md) block within DiracKernels. The fault, which extends between 300 m and 700 m is divided into 501 point sources and the locations of these point sources are provided through the `position_functions`. To model wave propagation in an infinite half-space, absorbing boundaries are placed at the left, right and bottom boundaries using the [NonReflectingBC](NonReflectingBC.md) block within the BCs block. Please refer to the [GettingStarted](getting_started-user.md) for details on the other objects used in this input file that are required for simulating wave propagation. The resulting displacements in the soil are stored in the exodus output file and this is used as input for the two intermediate models.

!listing examples/ex07/big_model.i

The input file corresponding to [fig:DRMmodel] (b) is shown below. To generate the mesh corresponding to this model within MASTODON, first a rectangular mesh is created using the [GeneratedMesh](source/mesh/GeneratedMesh.md) object. Then, a new subdomain/block, which includes everything within the inner boundary of the DRM layer, is created using the [SubdomainBoundingBox](source/meshmodifiers/SubdomainBoundingBox.md) object within the MeshModifiers block. This block of elements is then deleted using the [BlockDeleter](source/meshmodifiers/BlockDeleter.md) object that is also located with the MeshModifiers block, leaving only the mesh corresponding to the DRM element layer. New nodesets containing the nodes on the inner and outer boundaries of the DRM layer are created using the [AddExtraNodeset](source/meshmodifiers/AddExtraNodeset.md) object located within the MeshModifiers block. Both the inner and outer boundaries are split into 3 sections - left, bottom and right, for simplicity.

!listing examples/ex07/fixed_outer.i

!alert note
The three new boundaries corresponding to the inner/outer boundaries of the DRM layer should not have any overlapping nodes.

In [fig:DRMmodel] (b), the aim is to compute the seismic forces at the outer boundary of the DRM layer. To accomplish this task, the outer boundary is fixed using the [DirichletBC](source/bcs/DirichletBC.md) object and the displacement from the larger model is transferred to the inner boundary. The resulting reaction forces at the outer boundary are the seismic forces that need to be transferred to same boundary in the smaller model ([fig:DRMmodel] (d)). The transfer of displacements from model (a) to model (b) is achieved using the [SolutionUserObject](source/userobject/SolutionUserObject.md) and the [SolutionFunction](source/functions/SolutionFunction.md), which extract the displacement variables from the exodus file corresponding to the larger model and transfers it into a function of space and time, which is then prescribed at the inner boundaries using the [FunctionDirichletBC](source/bcs/FunctionDirichletBC) object. The resid_x and resid_y AuxVariables in this input file are used to store the residuals from the kernels and these residuals are essentially the reaction forces at the outer boundaries. These variables are stored in the output exodus file and will be used as input to the smaller model.  

The other intermediate model ([fig:DRMmodel] (c)) is very similar to the one described above but with the inner boundaries being fixed and the displacement from the larger model being transferred to the outer boundaries. One difference here is that the displacements from larger model has to be scaled by a factor of -1.0 (refer to the SolutionFunction object within the Functions block) before being applied to the outer boundaries. The reaction forces at the inner boundary are again stored in the resid_x and resid_y variables and these are transferred to the smaller model as seismic forces along the inner boundary. The input file for this model is shown below.

!listing examples/ex07/fixed_outer.i

The smaller model shown in [fig:DRMmodel] (d) has three distinct sections, the region within the DRM layer, the DRM layer, and the region outside it. The region within the DRM layer is the region of interest. The region outside the DRM layer is added to remove any additional waves generated due to addition of new features (e.g., structure, local topography) to the smaller soil domain, which are absent in the larger soil domain. As mentioned above, for this demonstration, the region within the DRM layer is modeled to be exactly same as that in the larger soil domain. So, no additional waves would be generated in this demonstration. Therefore, the response of the region outside the DRM layer would be identically zero throughout the simulation. But absorbing boundary conditions are placed at the left, right and bottom boundaries of the smaller soil domain to absorb any waves that may be radiating out of this domain in a more general scenario. As in the two intermediate models discussed above, the SolutionUserObject and SolutionFunction are used to extract the reaction forces from the exodus files corresponding to the intermediate models, and these forces are applied at the corresponding DRM layer boundaries using the [UserForcingFunctionNodalKernel](source/NodalKernels/UserForcingFunctionNodalKernel) object within the NodalKernels block. The input file for the smaller model is shown below.

!listing examples/ex07/small_model.i

## Results

As expected for a horizontal fault rupture, the waves resulting from the central portion of the fault form a vertically propagating wavefront. The shear wavefront generated in this scenario is presented in the left panel of [fig:DRMresults]. This results in the region directly above the fault having nearly zero vertical velocity.

Next, to confirm that DRM is working as expected, the horizontal velocity time histories at (X = 500 m, Y = 500 m) from both the large and smaller model are plotted in the right panel of [fig:DRMresults]. Since the region within the DRM layer has the exact same properties in both the larger and smaller soil domains, the velocities at any point within this region have to match exactly between the large the small models. As seen from the right panel of [fig:DRMresults], the velocities from both models are identical as expected.

!media media/examples/DRM_results.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:DRMresults
       caption= The left panel presents a snapshot showing the vertical propagation of shear wavefront in the larger model. The contours in this figure represent the magnitude of velocity and the white line represents the earthquake fault. The right panel presents a comparison between velocity time histories at a point from the large and small models.

Examples of problems that have been solved using DRM in MASTODON can be found in [!citet](veeraraghavan2017embedded) and [!citet](veeraraghavan2019nlssi).

!bibtex bibliography
