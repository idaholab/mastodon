# ResponseHistoryBuilder

The `ResponseHistoryBuilder` creates a response history given a list of variables and a list of nodes or boundaries. Either a list of nodes or boundaries should be specified, but not both. At the specified node(s) the values of the variables are stored at each time step. Each `ResponseHistoryBuilder` vectorpostprocessor creates a csv file with a time column and the values of variables at the nodes specified in the list of nodes or in the boundaries. For a variable 'accel_x' at node 2, the column header is 'node_2_accel_x'. The header for the time column is 'time'.

## Usage

The input file below contains two `ResponseHistoryBuilder` vectorpostprocessor blocks. The first block requests output at two nodes, 2 and 10, and the second block requests output at a boundary, 'bnd', which also contains the nodes, 2 and 10. Both blocks request histories for the variables, 'u' and 'accel_x'.

!listing test/tests/vectorpostprocessors/response_history_builder/response_history_builder.i block=VectorPostprocessors

As shown in the output csv files below, the response histories for the variables, 'u' and 'accel_x' are output at the requested nodes. The first output file below corresponds to the accel_nodes block and the second output file corresponds to the accel_bnd block. Note that both the output files are exactly the same, since the same variables are requested and the boundary, 'bnd' has the same nodes, 2 and 10.

!listing test/tests/vectorpostprocessors/response_history_builder/gold/response_history_builder_out_accel_nodes.csv

!listing test/tests/vectorpostprocessors/response_history_builder/gold/response_history_builder_out_accel_bnd.csv

!syntax parameters /VectorPostprocessors/ResponseHistoryBuilder

!syntax inputs /VectorPostprocessors/ResponseHistoryBuilder

!syntax children /VectorPostprocessors/ResponseHistoryBuilder
