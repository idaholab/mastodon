## Boundary Conditions

Boundary conditions are required by MASTODON finite element analysis framework to be able to run
simple quasi-static and dynamic analysis. Herein, the basic boundary conditions are given and other
constraints that are used to solve for geotechnical earthquake engineering problems are presented
separately (e.g. periodic boundary conditions.) Following input block creates a fully fixed boundary condition at the bottom of a single element.

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=BCs
         end=Periodic

In the above input, `type = PresetBC` sets the variable = disp_”related degree of freedom” with the
value = 0 which provides the fixity by defining a zero displacement at the node. boundary = 0 command
selects the nodes at the bottom surface of the element (labeled as surface 0) and assigns the
boundary conditions.

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
described in single element problem above. Alternatively, the boundary number can be identified using [Cubit](https://cubit.sandia.gov/) or [Trelis](https://www.csimsoft.com/trelis.jsp).  “variable = disp_x” imposes the boundary condition on the x
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

#### Periodic Boundary Conditions

Periodic boundary conditions are used to constrain the nodes to move together in the specified
directions. The following input is an example applied on the single element problem above and should be embedded into the BCs block segment as:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Periodic
         end=top_x
