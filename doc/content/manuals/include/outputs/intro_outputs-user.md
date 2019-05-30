## Outputs

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
MOOSE or Paraview should be used. `type = ElementVariableValue` returns the average scalar stress value
computed within the specified block. Finally, the results can be dumped into a csv file by using
"Outputs command". Once the analysis is completed, the output csv file will be stored in the same
folder as the input file. The first column contains the Time, and the rest of the columns contain
the values asked in the Postprocessors command. Another form of output is the exodus file which can
be read by Blot. The user also has the option to request results in both csv and exodus formats.
