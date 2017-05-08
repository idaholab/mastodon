# ResponseSpectraCalculator
The `ResponseSpectraCalculator` calculates the acceleration, velocity and displacement response spectra for a given list of acceleration variables that are built by a `ResponseHistoryBuilder` VectorPostprocessor. The response spectra will be calculated as functions of frequency and the outputs will be stored in a csv file.

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called accel_hist, which builds the response histories of the variables, accel_x and accel_y. It is desired that the response spectra for accel_x be calculated. This can be calculated by `ResponseSpectraCalculator` as follows.

!listing tests/vectorpostprocessors/response_spectra_calculator/response_spectra_calculator.i block=VectorPostprocessors

The response spectra calculation is performed only at the end of the simulation and the spectra are output into a csv file as shown below.

!listing tests/vectorpostprocessors/response_spectra_calculator/gold/response_spectra_calculator_accel_csv_accel_spec_0201.csv

!parameters /VectorPostprocessors/ResponseSpectraCalculator

!inputfiles /VectorPostprocessors/ResponseSpectraCalculator

!childobjects /VectorPostprocessors/ResponseSpectraCalculator
