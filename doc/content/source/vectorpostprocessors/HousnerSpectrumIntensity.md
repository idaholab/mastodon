# HousnerSpectrumIntensity

The `HousnerSpectrumIntensity` calculates the Housner Spectrum Intensity (HSI) for a set of response histories calculated by the `ResponseHistoryBuilder` VectorPostprocessor. Housner Spectrum Intensity is defined as the area under the velocity spectrum curve, when plotted with period on the X axis [!citep](housner1952hsi). [!citet](housner1952hsi) recommends that the area be calculated between the periods 0.25sec and 2.5sec, which are the default values for the `start_period` and `end_period` parameters in MASTODON (see parameter definitions below). The HSI values will be output into a csv file in the same order as the acceleration variables are entered in the input file.

## Usage

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called 'accel_hist', which builds the response histories of the acceleration variables, accel_x and accel_y at nodes 8 and 9. It is desired that the HSIs for accel_x and accel_y at these nodes be calculated. These can be calculated by the `HousnerSpectrumIntensity` vectorpostprocessor called 'accel_hsi', as shown in the input file below.

!listing test/tests/vectorpostprocessors/housner_spectrum_intensity/housner_spectrum_intensity.i block=VectorPostprocessors

!alert note
`HousnerSpectrumIntensity` assumes that the response histories calculated in the vectorpostprocessor, provided in the `vectorpostprocessor` parameter in the input are acceleration histories, and will not perform any checks to ensure that they are accelerations. The user should ensure this by defining the corresponding `ResponseHistoryBuilder` appropriately.

Special care must be taken to ensure that the HSI calculations are performed only at the end of the simulation (last time step) and not at every time step. In order to accomplish this, an 'Outputs' block must be created as shown below. This block (named 'out') ensures that the CSV output is only calculated at the final timestep.

!listing test/tests/vectorpostprocessors/housner_spectrum_intensity/housner_spectrum_intensity.i block=Outputs

For such an input, the HSI calculation is performed only at the end of the simulation and the HSIs are output into a csv file as shown below. Note that the columns in the csv file are organized according to the node numbers and variable names. For example, the HSIs for accel_x at node 8 are under node_8_accel_x_hsi. In this case, the HSIs for nodes 8 and 9 for the variables, accel_x and accel_y are reported.

!listing test/tests/vectorpostprocessors/housner_spectrum_intensity/gold/housner_spectrum_intensity_out_accel_hsi.csv

!syntax parameters /VectorPostprocessors/HousnerSpectrumIntensity

!syntax inputs /VectorPostprocessors/HousnerSpectrumIntensity

!syntax children /VectorPostprocessors/HousnerSpectrumIntensity

!bibtex bibliography
