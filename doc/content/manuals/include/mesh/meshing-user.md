## Creating a mesh id=meshing

The first thing necessary to run a finite element analysis is a mesh. Generating a mesh of interest in MASTODON can be achieved using the inbuilt [mesh generator](source/mesh/GeneratedMesh.md) or by importing an existing mesh. Imported meshes should be of the exodus format and can be generated using the meshing software, [Cubit](https://cubit.sandia.gov/). Cubit is freely available for users in the U.S. federal government and national labs. For other users, the commercial version of Cutbit called [Trelis](https://www.csimsoft.com/trelis.jsp) is recommended. Usage of the inbuilt mesh generator for a simple problem is demonstrated below.

For any analysis, the number
of dimensions and the degrees of freedom should be specified in the beginning of the `Mesh` block in the input file. An
example input is provided below:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Mesh
         end=GlobalParams

This input creates a single brick element using the MASTODON mesh generator with 8 nodes, 8 gauss
quadrature points, and unit length in each edge. `dim = 3` specifies that the domain is three-dimensional, and nx, ny, and nz specify the number of elements in X, Y and Z directions, respectively.

[fig:single_element] presents a generic three-dimensional brick element along with node and surface
labels. Generated brick element is automatically assigned a block number (block 0 in this case) and
each side of the brick is automatically assigned a surface number.

!media media/user/single_element.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:single_element
       caption=Single brick element (a) node labels (b) surface labels.

The next step is activating the global parameters. These are input parameters, which have the same
value across the input file. The global parameters are activated providing the following commands:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=GlobalParams
         end=Variables

By providing the above commands, the user specifies that if any object in the input file has a
parameter called “displacements”, that parameter would be set to “disp_x disp_y disp_z”, which are
the global displacement degrees of freedoms at the nodal points in x, y, and z directions,
respectively.

Before or after (the sequence does not matter in the input file) the global parameters, the user has
to specify the solution variables using the following commands:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Variables end=AuxVariables

MASTODON allows user to define auxiliary variables. Auxiliary variables are not solved for by the
system but they are calculated from the solution variables. This is particularly necessary for
dynamic analysis, since the nodal accelerations and velocities are defined in this section. In
addition, the stresses and strains are defined as auxiliary variables to access the data later on to
inspect the results. Following commands are the examples of defining the auxiliary variables:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=AuxVariables
         end=Kernels

Using the above commands, user defines velocity, acceleration, stress, and strain auxiliary variables
to be used later on to ask for an output. order = CONSTANT and family=MONOMIAL calculates the average
stress within the element.

The above command blocks forms the basis for all quasi-static and dynamic analyses that can be
conducted using MASTODON framework. Next section describes the Kernels related to physics involved in
the particular analysis.
