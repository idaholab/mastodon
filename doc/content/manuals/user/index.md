# User Manual

## Getting Started

This section provides step-by-step instructions to define the basic components of a simple numerical
model for a finite element analysis using MASTODON framework focusing on the geotechnical earthquake
engineering applications.

### Defining a simple domain

The first thing necessary to run a finite element analysis is a meshed domain. Meshing the domain of
interest for MASTODON finite element analysis can be achieved using the inbuilt mesh generator or
using a preprocessor <a href= " https://cubit.sandia.gov/"> Cubit </a>. For any analysis, the number
of dimensions and the degrees of freedom should be specified in the beginning of the input file. An
example commands are provided below:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Mesh
         end=GlobalParams

This command generates a single brick element using the MASTODON mesh generator with 8 nodes, 8 gauss
quadrature points, and unit length in each edge. dim = 3 command specifies that the domain in three
dimensions, and nx, ny, and nz specify the number of elements in the corresponding directions.

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

### Kernels

Kernels are related to the physics involved in the particular analysis. This document focusses on
kernel commands related to the geotechnical earthquake engineering applications. Further information
can be found in [/Kernels/index.md].

The main kernel that is used for quasi-static and dynamic analyses is
[modules/tensor_mechanics/index.md]. This kernel is used to solve the equation of motion without the
inertial effects. It requires information about the unknowns that are solved for. The following chunk
of commands can be used to activate dynamic tensor mechanics kernel along with Newmark-beta
integration scheme, inertial effects, and two mode Rayleigh viscous damping (both stiffness and mass
proportional damping):

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Kernels
         end=BCs

displacements = 'disp_x disp_y disp_z' line is not necessary if the displacements are already defined
as global parameters but are provided here for the sake of completeness. Beta and gamma are the
parameters of Newmark-beta integration scheme. Zeta and eta are the stiffness and mass matrix
coefficients of Rayleigh damping formulation (see [modules/tensor_mechanics/index.md] and
[Theory Manual](manuals/theory/index.md) for more information). Auxiliary kernels are specified to
calculate the acceleration and velocities using Newmark-beta scheme at the end of each time step
where the displacement is already solved and known. Lastly, stress_xy is defined as an auxiliary
variable. This is achieved by specifying the type of the Auxkernel as RankTwoAux. RankTwoAux means
that the source of the auxiliary variable is a rank two tensor, and the type of rank two tensor is
explicitly defined as stress tensor using the command “rank_two_tensor = stress”. Since, the variable
is stress_xy, the location in the stress tensor corresponding to stress_xy needs to be specified
using index_i (row index) and index_j (column index). To request for stress_xy, index_i is set to 0
and index_j is set to 1. The next section explains the boundary conditions that are required to run a
simple, quasi-static and dynamic analyses.

### Boundary Conditions

Boundary conditions are required by MASTODON finite element analysis framework to be able to run
simple quasi-static and dynamic analysis. Herein, the basic boundary conditions are given and other
constraints that are used to solve for geotechnical earthquake engineering problems are presented
separately (e.g. periodic boundary conditions.) Following commands are related to the single element
presented in [fig:single_element] and a chunk of command is provided below to demonstrate how to fix
the bottom nodes in all directions.

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=BCs
         end=Periodic

In the above code, type = PresetBC sets the variable = disp_”related degree of freedom” with the
value = 0 which provides the fixity by defining a zero displacement at the node. boundary = 0 command
selects the nodes at the bottom surface of the element (labeled as surface 0) and assigns the
boundary conditions.

### Analysis Executioner

In order to unfold the analysis, executioner commands are required. There are two main types of
executioner in MASTODON, which are steady and transient. Herein, an example is provided to run
transient analysis that will be used later on examples related to geotechnical earthquake engineering
applications.

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Executioner
         end=Postprocessors

The type = Transient command is self-explanatory and sets the executioner type to
transient. solve_type = PJFNK is the default option (preconditioned Jacobian Free Newton-Krylov) used
to solve the transient problems in MASTODON without constructing full stiffness matrix. The command
dt = 0.01 (is set to 0.01 sec. here for an example purpose) determines the initial time step and user
is allowed to set a minimum time step “dtmin” for the analysis. start_time and end_time specifies
where the transient analysis will start and where it will end. Further information on available
options can be found in [/Executioner/index.md].

### Outputs

The analysis results can be dumped to a csv file using postprocessor and output commands. The first
command should define the type of results that will be dumped into the csv file. The following
command is an example to specify the results to be outputted:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Postprocessors

Since, the stresses, accelerations, and velocities are defined as an auxiliary variable previously
([Kernels](manuals/user/index.md)), they can be directly called in this section. elementid = 0
specifies the element number from which stress values will be pulled out. Note that MASTODON assigns
numbers to elements and nodes different than those of the input file provided by the user (for
user-defined mesh). To obtain the correct element and nodes numbers, the software Blot installed with
MOOSE or Paraview should be used. type = ElementVariableValue returns the average scalar stress value
computed within the specified block. Finally, the results can be dumped into a csv file by using
"Outputs command". Once the analysis is completed, the output csv file will be stored in the same
folder as the input file. The first column contains the “Time”, and the rest of the columns contain
the values asked in the "Postprocessors" command. Another form of output is the exodus file which can
be read by Blot. The user also has the option to request results in both csv and exodus formats.

### Gravity Loading

Gravity loading can be applied on the domain to achieve the stress state due to gravity. MOOSE
includes built-in commands to apply the gravity as a body force throughout the transient
analysis. The following command can be used to apply the gravitational field on the modeled domain:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=gravity
         end=AuxKernels

The above command should be embedded inside the [Kernels](manuals/user/index.md) command
block. variable = disp_z activates the gravity in z direction and value = -9.81 is assigned to
specify the magnitude and direction of the gravitational field.

Gravity command applies an external body force along the specified direction on the domain of
interest. In order to bring the system to equilibrium, a transient analysis with some viscous damping
is necessary. This approach causes fluctuations on the stress and strains at the beginning of the
analysis because of the elements being initially at zero stress state. Viscous damping removes the
fluctuations and brings the system to equilibrium. Once the system equilibrates, the gravity stresses
are obtained along with the displacements due to the gravity loading. “Initial Stress” command is
available in MASTODON framework to eliminate the need for a separate transient analysis. In addition,
no displacements, or strains due to gravity result.

### Loads

There are two types of loads that can be applied to the domain of interest. These are prescribed
displacement and prescribed loads. Both types can be defined as a time history.

#### Prescribed Displacement

The preset displacement boundary condition can be used to apply a displacement time history to a
boundary (at the nodes). The displacement boundary condition first converts the user defined
displacement time history to an acceleration time history using Backward Euler finite difference
scheme. This acceleration is then integrated to get displacement using Newmark-beta method.  The
resulting displacement is then applied as a kinematic displacement boundary condition. The following
command can be used to apply the preset displacement boundary condition:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=top_x
         end=Functions

The above command should be embedded inside the BCs command block. “boundary = 5” assigns the preset
displacement to boundary 5 which, in this case, is a predefined boundary of a single element as
described in [fig:single_element]. Alternatively, the boundary number can be identified using <a
href= "www.csimsoft.com/"> Trelis </a>.  “variable = disp_x” imposes the boundary condition on the x
direction. “beta” is the Newmark-beta integration parameter. The “function = top_disp” specifies the
function that defines the loading time history. It is defined in the “Functions” block as follows:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Functions
         end=Materials

Displacement2.csv is the file, located in the same directory of the input file, containing the
displacement time history. The first column of this file should contain the time vector starting at
0.0. The second column should contain the displacement values. “type = PiecewiseLinear” defines the
type of the function which is in this case piecewise-linear. “format” specifies the format of the
data file, i.e. whether the data is in columns or rows.

#### Prescribed Acceleration

The preset acceleration boundary condition can be used to apply an acceleration time history to a
boundary. The preset acceleration boundary condition integrates the given acceleration time history
to get the displacement using Newmark-beta method. This displacement is then applied as a kinematic
displacement boundary condition. Syntax is the same as prescribing a displacement boundary condition
but with type = PresetAcceleration and the function describing time vs acceleration data instead of
time vs displacement.

### Periodic Boundary Conditions

Periodic boundary conditions are used to constrain the nodes to move together in the specified
directions. The following command is an example applied on the element presented in
[fig:single_element] and should be embedded into the BCs block segment as:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Periodic
         end=top_x

### Initial Stresses

Initial stress command is available to apply the prescribed initial stresses to the finite element
domain. If the user applies initial stress state that is equal to that of gravity loadings, upon
transient analysis, no vibration occurs and the domain reaches equilibrium with zero strains. This
eliminates the necessity of conducting separate analysis for free-field problems and allows the user
to start directly with a dynamic analysis such as base shaking etc. The following command can be used
within the material definition to activate the initial stresses in the material block:

```
initial_stress = '-4.204286 0 0  0 -4.204286 0  0 0 -9.810'
```

The nine numerical values fill the stress tensor in the following order - xx, xy, xz, yz, yy, yz, zx,
zy, and zz. The above example command activates -4.2025 units of stress along both xx and yy
direction, and -9.81 units of stress along zz direction.  These stresses are the gravity stresses at
the mid-depth of the element in [fig:single_element] with density = 2. Therefore, the element will
immediately start with gravity stress conditions without any elastic deformations or stress
fluctuations.

### Materials

#### Linear elastic soil

A linear elastic soil can be defined in MASTODON by defining a elasticity tensor using
ComputeIsotropicElasticityTensor, stress calculator using ComputeLinearElasticStress, and a small
strain calculator using ComputeSmallStrain. Density can be defined using the
GenericConstantMaterial. An example of these input blocks is shown below:

!listing test/tests/dirackernels/seismic_source/one_seismic_source.i
         start=Materials
         end=Executioner

#### I-soil material model

The nonlinear soil material model usage is demonstrated in the [Example 1](examples/index.md)
