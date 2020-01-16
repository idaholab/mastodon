# ResponseSpectraCalculator

The `ResponseSpectraCalculator` calculates the acceleration, velocity and displacement response spectra for a set of response histories calculated by the `ResponseHistoryBuilder` VectorPostprocessor. The response spectra will be calculated as functions of frequency and period, and the outputs will be stored in a csv file.

## Usage

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called 'accel_hist', which builds the response histories of the acceleration variables, accel_x and accel_y at nodes 8 and 9. It is desired that the corresponding response spectra are calculated. This can be calculated by `ResponseSpectraCalculator` using the 'accel_spec' block as follows.

!listing test/tests/vectorpostprocessors/response_spectra_calculator/response_spectra_calculator.i block=VectorPostprocessors

!alert note title=Acceleration histories only
`ResponseSpectraCalculator` assumes that the response histories provided through the `vectorpostprocessor` input parameter (e.g., in the 'accel_hist' block above) are acceleration histories. No checks are performed to ensure that they are accelerations. The user should ensure this by defining the corresponding `ResponseHistoryBuilder` block appropriately.

Special care must be taken to ensure that the response spectra calculations are performed only at the end of the simulation (last time step) and not at every time step. In order to accomplish this, an 'Outputs' block must be created as shown below. This block (named 'out') ensures that the CSV output is only calculated at the final timestep.

!listing test/tests/vectorpostprocessors/response_spectra_calculator/response_spectra_calculator.i block=Outputs

The name of the Outputs block 'out' should be provided in the response spectra VectorPostprocessor block as `outputs = out` as shown above. For this input, a csv file of the response spectra is created, as shown below. Note that the columns in the csv file are organized according to the node numbers and variable names. For example, the spectral accelerations for accel_x at node 8 are under node_8_accel_x_sd, node_8_accel_x_sv, and node_8_accel_x_sa, which are the displacement, velocity and acceleration spectra, respectively. Note that the corresponding frequency and period columns are also included in the output file.

!listing test/tests/vectorpostprocessors/response_spectra_calculator/gold/response_spectra_calculator_out_accel_spec.csv

!syntax parameters /VectorPostprocessors/ResponseSpectraCalculator

!syntax inputs /VectorPostprocessors/ResponseSpectraCalculator

!syntax children /VectorPostprocessors/ResponseSpectraCalculator
