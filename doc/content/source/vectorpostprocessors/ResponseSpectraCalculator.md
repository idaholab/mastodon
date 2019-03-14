# ResponseSpectraCalculator

The `ResponseSpectraCalculator` calculates the acceleration, velocity and displacement response spectra for a given list of acceleration variables that are built by a `ResponseHistoryBuilder` VectorPostprocessor. The response spectra will be calculated as functions of frequency and the outputs will be stored in a csv file.

## Usage

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called accel_hist, which builds the response histories of the variables, accel_x and accel_y. It is desired that the response spectra for accel_x be calculated. This can be calculated by `ResponseSpectraCalculator` as follows.

!listing test/tests/vectorpostprocessors/response_spectra_calculator/response_spectra_calculator.i block=VectorPostprocessors/accel_spec

Special care must be taken to ensure that the response spectra calculations are performed only at the end of the simulation (last time step) and not at every time step. In order to accomplish this, an 'Outputs' block must be created as shown below. This block (named 'out') ensures that the CSV output is only calculated at the final timestep.

!listing test/tests/vectorpostprocessors/response_spectra_calculator/response_spectra_calculator.i block=Outputs

The name of the Outputs block 'out' should be provided in the response spectra VectorPostprocessor block as `outputs = out` as shown above. For this input, a csv file of the response spectra is created, as shown below.

!listing test/tests/vectorpostprocessors/response_spectra_calculator/gold/response_spectra_calculator_out_accel_spec.csv

!syntax parameters /VectorPostprocessors/ResponseSpectraCalculator

!syntax inputs /VectorPostprocessors/ResponseSpectraCalculator

!syntax children /VectorPostprocessors/ResponseSpectraCalculator
