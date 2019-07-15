# ResponseHistoryMean

`ResponseHistoryMean` computes the mean history given the response histories of a suite of nodes. These nodal response histories are computed using the `ResponseHistoryBuilder` class referenced as a vector of pointers. This vector is then passed on to `ResponseHistoryMean` to compute the mean history calculated as a function of time and will be stored in a csv file.

## Usage

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called 'disp_nodes' which builds the response histories of the displacement variable 'u' at nodes 2, 6, and 10. It is desired that the corresponding mean history is calculated. This can be calculated by `ResponseHistoryMean` using the 'disp_mean' block as follows.

!listing test/tests/vectorpostprocessors/response_history_mean/response_history_mean.i block=VectorPostprocessors

!alert note
`ResponseHistoryMean` assumes that the response histories calculated in the vectorpostprocessor, provided in the `vectorpostprocessor` parameter in the input, are quantities of the same type (i.e., accelerations, velocities, or displacements). No checks will be performed to ensure that the input response histories are of the same type. The user should ensure this by defining the corresponding `ResponseHistoryBuilder` appropriately.

Because mean response history calculations are performed only at the end of the simulation (last time step) and not at every time step, an 'Outputs' block must be created as shown below. This block (named 'out') ensures that the CSV output is only calculated at the final timestep.

!listing test/tests/vectorpostprocessors/response_history_mean/response_history_mean.i block=Outputs

The name of the Outputs block 'out' should be provided in the mean history VectorPostprocessor block as `outputs = out` as shown above. For this input, a csv file of the mean response history is created, as shown below.

!listing test/tests/vectorpostprocessors/response_history_mean/gold/response_history_mean_out_disp_mean.csv

!syntax parameters /VectorPostprocessors/ResponseHistoryMean

!syntax inputs /VectorPostprocessors/ResponseHistoryMean

!syntax children /VectorPostprocessors/ResponseHistoryMean
