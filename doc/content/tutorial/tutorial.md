# Tutorial

This tutorial is a breakdown of a full MASTODON input file. It will begin as a very simple site response analysis and will gain complexity as the tutorial proceeds. We will step through the problem in the same sort of way that an advanced user would. Each part of the input file will be briefly explained and links to useful areas in the documentation site will be provided.

## Linear Site Response

The first step is to begin with something very simple. Linear site response. No gravity, linear soil, everything in this step should be fairly straightforward.

In general, users do not just start writing a text input file from scratch, instead the user should find a test or example input file that is generally related to the problem being set up and modify that input to better fit his or her model. With that stated, feel free to use this input file as a skeleton for your future site response analysis models. This tutorial will step through each block of text that comprises this input file.

!alert note
The order in which you define these blocks within the input file does not matter! There is somewhat of a natural progression when it comes to defining the various blocks, but the order is not significant.

### The Model

The model we will be stepping through in this tutorial is a 1D site response. The model is a 100 m soil column containing 100 elements, each measuring 1 m x 1 m x 1 m. The base of the soil column is prescribed the acceleration history shown in [fig:tutorial_site_response]. Note that this is just a contrived example problem, there is no significance or meaning associated with the input motion. The outputs include an exodus file that can be used for visualizing results, and csv files containing accelerations and response spectra calculations at the top of the column. These outputs will be discussed in detail further into the tutorial. The input file is 'site_reponse_1.i' It can be executed in MASTODON, after compiling the code, by pointing to the executable and the input file as shown below:

```
~/projects/mastodon/mastodon-opt -i site_response_1.i
```

!media media/tutorial/acc_hist.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:tutorial_site_response
       caption=Acceleration history prescribed at the base of the soil column.

At this point, we will start going through the input file. Each block of text in the input file will be briefly explained and useful links will be provided for further investigation.

!listing tutorial/site_response_1.i
         start=GlobalParams
         end=Mesh

The first block shown in this input file is the Global Params. This is an optional block of text in the MASTODON input file, which is useful for defining parameters that are required in multiple blocks of the input file. Instead of defining these parameters in each of the blocks that require it, you can define it only once in the GlobalParams block. In general this will help the user shorten his or her input file, but is not at all a requirement. In this instance, we have defined the displacement variables, which are required to be defined in the dynamic tensor mechanics kernel and the strain calculator block. Instead of defining them in those two places, we can define them only once in GlobalParams. The redundant lines are commented in the input file to show where they would be, if not set in the GlobalParams block.

!listing tutorial/site_response_1.i
         start=Mesh
         end=Variables

The next block in the input file is the Mesh block. This is a required block of text in the input file where the user defines the mesh to be used in the analysis. For simple geometry, the mesh can be created using the internal mesh generator which is the method demonstrated in this input file. The model is a 100 m soil column containing 100 elements, each measuring 1 m x 1 m x 1 m. If the geometry is more complex than rectangle/cube shapes, it is advised that the user create the mesh using an external mesh generation software. Mesh files can be imported in this block by changing the type to [FileMesh](source/mesh/FileMesh.md). Users and developers at INL utilize Cubit for most of our mesh generation needs, which is commercially available as Trelis. The preferred file type is an exodus (.e) file, although MASTODON can support other mesh file types, such as Abaqus (.inp) files (some limitations apply). See [Mesh](syntax/Mesh/index.md) for more information on the mesh system.

!listing tutorial/site_response_1.i
         start=Variables
         end=AuxVariables

The next block of text is the Variables block. This is where the user defines the non-linear variables being solved for, such as displacements, temperature, etc. See [Variables](syntax/Variables/index.md) for more information on the variables system.

!listing tutorial/site_response_1.i
         start=AuxVariables
         end=Kernels

Following Variables, the user must then define the AuxVariables. Auxiliary variables are values which can be derived from the primary variables or explicitly defined, this includes velocity, acceleration, stress etc. See [AuxVariables](syntax/AuxVariables/index.md) for more information on the AuxVariables system.

!listing tutorial/site_response_1.i
         start=Kernels
         end=AuxKernels

The Kernels block is where the physics of interest is defined. Because MASTODON is built on the MOOSE Framework, it can accomodate many types of problems, and therefore does not expect dynamics problems only. Because this is a dynamics simulation, the user must define the [DynamicTensorMechanics](syntax/Kernels/DynamicTensorMechanics/index.md) block, along with the inertia blocks for the three directions. There is a large library of options for this block in the input file. See [Kernels](syntax/Kernels/index.md) for more information on the Kernels system.

!listing tutorial/site_response_1.i
         start=AuxKernels
         end=BCs

The AuxKernels block is defined next. AuxKernels act on variables using explicit or know functions in order to determine the AuxVariable values. Here is where we define all accelerations and velocities, which are determined using the displacement variables in the [Newmark Method](source/auxkernels/NewmarkAccelAux.md). This block is also where stress or other field variables would be setup. See [AuxKernels](syntax/AuxKernels/index.md) for more information on the AuxKernels system.

!listing tutorial/site_response_1.i
         start=BCs
         end=Functions

Now that all of the variables have been defined and set up properly, we can begin to formulate the problem for the analysis. The BCs block is where problem boundary conditions are applied. We use the DirichletBC type to fix the bottom surface of the soil column in the Y and Z directions, then prescribe an acceleration history to the bottom surface using the PresetAcceleration type. This is defined in the X direction only, for this problem. In order to constrain the soil column so that it only moves in shear, a periodic boundary condition is applied along the side surfaces of the soil column using the Periodic block. The side surfaces are input as boundaries. The boundaries called 'back', 'front', 'bottom', and 'top' are automatically created by the GeneratedMesh above. Usually, these boundaries will have to be defined in Cubit, Trelis, or any other meshing software being used. The Periodic BC requires the surfaces (input as boundaries) to be perfectly parallel and the normal vector of translation between the surfaces is input as the 'translation' parameter. See [BCs](syntax/BCs/index.md) for more information on the boundary conditions system including the Periodic BC.

!listing tutorial/site_response_1.i
         start=Functions
         end=Materials

The next block in this input file is the Functions block. This is another optional block that is not required for all simulations. Recall from the boundary conditions that for this model we are prescribing the base acceleration. There are a number of ways to do this within MASTODON, one of which is to supply a csv file that contains the acceleration history to be used. We input this file using a PiecewiseLinear function so that if the timesteps in the simulation do not exactly match those of the acceleration history, the acceleration values will be linearly interpolated. There are a wide variety of function types that can be defined within this block. See [Functions](syntax/Functions/index.md) for more information.

!listing tutorial/site_response_1.i
         start=Materials
         end=Executioner

The materials block is a required block in the MASTODON input file. This is where the user will define the material models to be used and the property values necessary for those calculations. For this example we have one block of material, which is linear soil. We define the material type as [ComputeIsotropicElasticityTensorSoil](source/materials/ComputeIsotropicElasticityTensorSoil.md) and define the necessary properties for each soil layer. Additionally we define the stress and strain formulations. There are a great deal of material options available to MASTODON users, see [Materials](syntax/Materials/index.md) for more information on the materials system.

!listing tutorial/site_response_1.i
         start=Executioner
         end=Postprocessors

The Executioner is a required block in the input file. It defines important solver information, start and end times, convergence criteria and timestep size. See [Executioner](syntax/Executioner/index.md) for more information on the executioner system.

!listing tutorial/site_response_1.i
         start=Postprocessors
         end=VectorPostprocessors

The Postprocessors block is another optional block in the input file. It is useful for obtaining various quantities of interest. For instance, if the user would like to get the acceleration history at the top of the soil column, there are a number of ways to do that. One way would be to run the analysis without any postprocessors, then probe the field values in an exodus viewer such as paraview. Another method, is to use a postprocessor in MASTODON. In this input file, we have two postprocessors defined, but both do the same thing - obtain the acceleration history at the top of the soil column. The data collected is output in a csv file. There are several postprocessors available for use in MASTODON, for more information see [Postprocessors](syntax/Postprocessors/index.md).

!listing tutorial/site_response_1.i
         start=VectorPostprocessors
         end=Outputs

Similar to Postprocessors, the user can also define vectorpostprocessors. These can be thought of as postprocessors that define multiple values at each step in the simulation. In this input we have setup the [ResponseHistoryBuilder](source/vectorpostprocessors/ResponseHistoryBuilder.md) in order to obtain the acceleation histories at the top of the soil column, and the [ResponseSpectraCalculator](source/vectorpostprocessors/ResponseSpectraCalculator.md) in order to obtain the response spectra for each of those acceleration histories. Like postprocessors, there are a wide variety of VectorPostprocessors available for use in MASTODON. See [VectorPostprocessors](syntax/VectorPostprocessors/index.md) for more information.

!listing tutorial/site_response_1.i
         start=Outputs

The final block of text in this input file is the Outputs block. It is not required, and some information is automatically output by default. This block is useful for controlling the amount of information that is output to the screen during your simulation, as well as other useful information such as performance metrics. This is also where the user can specify the types of files to be output (csv, exodus, etc.). See [Outputs](syntax/Outputs/index.md) for more information on the outputs system.

In general, an exodus file is the output file that will be useful for visualization of the results. For this, any exodus file viewing software will work. [Peacock](application_usage/peacock.md) is the MOOSE Graphical User Interface currently being developed and used within the MOOSE developer and user community. Another option that is widely used is [Paraview](https://www.paraview.org/).
