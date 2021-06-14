# Example 12: Soil-structure interaction analysis of nuclear power plant structure with fluid structure interaction

!alert note title=Units of this example
GigaPascals, meters, and seconds.

## Model description

This example demonstrates a soil-structure interaction (SSI) analysis of a representative nuclear power-plant structure complete with an internal reactor vessel and generic fluid material where fluid structure interaction (FSI) effects are modeled. Details regarding the model can be seen in [fig:ssi_model] below.

!media media/examples/ex12/SSI_model.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:ssi_model
       caption=Top Left: View of inner structure. Bottom Left: View of reactor vessel and supporting components. Right: Full SSI/FSI model.

The model shown in the figure above contains a rectangular soil domain 32m deep and 450m x 300m in the XY plane. The structure sits on top of a 1m thick basemat. The building is surrounded by external buttresses that also span the top of the structure beneath the roof slab. The cylindrical reactor vessel has a spherical bottom and is suspended from a mid level slab, which is supported by internal walls as well as a cylindrical citadel. A more detailed look at the reactor vessel is shown in [fig:rv_section] below.

!media media/examples/ex12/RV_section.png
       style=width:50%;margin-right:0px;float:center;
       id=fig:rv_section
       caption=Section view of the support citadel, reactor vessel and internal fluid.

The reactor vessel shown in [fig:rv_section] is 2cm thick, the inner diameter of the vessel is 5 m and the vessel is approximately 6m tall from the bottom of the spherical end to the top of the vessel.

## Finite element mesh generation

The three-dimensional finite element mesh is generated using Cubit 15.6. The procedure for generating this mesh is not trivial, so the journal script used to create the model is provided in the example 12 directory along with other necessary input files. In order to adequately capture bending of the structure walls, a minimum of 2 linear elements through the thickness of the walls is required. Alternatively, 1 or 2 second order elements could be used, but this will result in an increase in computational costs.

The soil domain has an unstructured mesh using approximately 2m cubic elements which capture frequencies less than about 50 Hz. While this results in a somewhat coarser mesh, the computational burden will still be manageable. The foundation and soil share a continuous mesh so the foundation-soil interface is tied, and does not allow any separation. This model is presented in [fig:Mesh].

!media media/examples/ex12/mesh.png
       style=width:70%;margin-right:0px;float:center;
       id=fig:Mesh
       caption=Cubit generated mesh for SSI analysis.

## Modeling in MASTODON

This example consists of three individual analysis efforts for: (i) site response analysis (no structure); (ii) fixed-base structure (no soil); (iii) full soil structure interaction model

### Site response analysis

A single stack of 3D volume elements are generated within the mastodon input file in order to define the site response model domain.  Nonlinear soil is modeled using the Isoil material block, where the backbone curve is supplied as a csv file. Periodic boundary conditions are applied to the two opposite faces of the soil domain, which are perpendicular to the Z-axis to ensure that they move together. This is achieved using the [`Periodic`](source/actions/AddPeriodicBCAction.md) option in the boundary conditions. The translational motion is fully defined in X Y and Z directions as shown in [fig:ground_motion] and is applied at the bottom of the soil block. The acceleration response is measured at the top using [`ResponseHistoryBuilder`](source/vectorpostprocessors/ResponseHistoryBuilder.md). This represents the free field soil response and is used as the input motion for the fixed base model. For more information on how to set up a site response model in MASTODON, see [`Example 3a`](examples/example3a.md)

!media media/examples/ex12/ground_motion.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:ground_motion
       caption=Ormsby wavelet input ground motion used for the analysis.

The input file for simulating the free field soil response is presented below.

!listing examples/ex12/site_response/sr_input.i

### Fixed-base structure analysis

Once the free field soil response has been collected from the site response model, that acceleration history can be used as an acceleration input for the fixed base structure model. The bottom surface is fixed to the accelerations collected from the site response model using the [`PresetAcceleration`](source/bcs/PresetAcceleration.md) boundary condition.

!media media/examples/ex12/fixed_base_mesh.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Structure
       caption=Meshed fixed base model used in this analysis.

!listing examples/ex12/fixed_base/fixed_base.i

The response history builder vectorpostprocessor was used to capture the response at various of points of interest in this model, specifcally around the reactor vessel. [fig:rv_response] shows the acceleration response spectrum at a point on the slab directly above the center of the reactor vessel. A nodeset was created during the meshing proccess to gather struture response information like this at various points around the model, additionally node ids can be supplied to the vectorpostprocessor.

!media media/examples/ex12/fixed_base_result.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:rv_response
       caption=plot of the spectral acceleration at a point on the slab directly above the center of the reactor vessel.

### Soil-structure interaction analysis

The SSI model is essentially a combination of the two previously described input files and is shown below. The full model can be seen in [fig:ssi_model] The ground motion shown in [fig:ground_motion] is applied at the bottom of the soil domain, and just as with the site response model, periodic boundary conditions are applied on the sides of the soil domain. The structure and basemat are merged with the soil so gapping/rocking/sliding behavior is not considered in this simulation.

!listing examples/ex12/ssi_input.i

!bibtex bibliography
