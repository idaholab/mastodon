# ResponseHistoryBuilder
The `ResponseHistoryBuilder` creates a time history given a list of variables and a node. Thus, at the specified node the values of the variables are stored at each timestep.

For example, the input file below contains two variables: "u" and "accel_x". It is desired that
the values of these variables be recorded at each timestep at node 2, thus the `ResponseHistoryBuilder` is used as follows.

!listing tests/vectorpostprocessors/response_history_builder/response_history_builder.i block=VectorPostprocessors

For output, it is only necessary to output the file at the end of simulation. Thus, the CSV output
is limited, resulting in the CSV file shown below.

!listing tests/vectorpostprocessors/response_history_builder/response_history_builder.i block=Outputs

!listing tests/vectorpostprocessors/response_history_builder/gold/response_history_builder_out_accel_0006.csv


!parameters /VectorPostprocessors/ResponseHistoryBuilder

!inputfiles /VectorPostprocessors/ResponseHistoryBuilder

!childobjects /VectorPostprocessors/ResponseHistoryBuilder
