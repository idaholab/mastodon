### Response history mean

`ResponseHistoryMean` computes the mean history given the response histories of a suite of nodes or a boundary.
The input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called
'disp_nodes' which builds the response histories of the displacement variable 'u'
at nodes 2, 6, and 10. It is desired that the corresponding mean history is calculated.
This can be calculated by `ResponseHistoryMean` using the 'disp_mean' block as follows.

!listing test/tests/vectorpostprocessors/response_history_mean/response_history_mean.i block=VectorPostprocessors

!alert note
`ResponseHistoryBuilder` has the capability to calculate response quantities of different types (i.e., acceleration_x, acceleration_y, velocity_x, displacement_y) at a list of nodes or a boundary. If the `ResponseHistoryBuilder` VectorPostprocessor requesting response histories of different types is input into `ResponseHistoryMean`, then mean across all these types will be calculated by `ResponseHistoryMean`. Of course, this may not be the intent of the user. It is, therefore, the responsibility of the user to ensure that `ResponseHistoryBuilder` has only one response quantity whose histories are recorded at a list of nodes or a boundary. `ResponseHistoryMean` will not perform a check to ensure this.

Because mean response history calculations are performed only at the end of the simulation (last time step) and not at every time step, an 'Outputs' block must be created as shown below. This block (named 'out') ensures that the CSV output is only calculated at the final timestep.

!listing test/tests/vectorpostprocessors/response_history_mean/response_history_mean.i block=Outputs

The name of the Outputs block 'out' should be provided in the mean history VectorPostprocessor block as `outputs = out` as shown above. For this input, a csv file of the mean response history is created, as shown below.

!listing test/tests/vectorpostprocessors/response_history_mean/gold/response_history_mean_out_disp_mean.csv
