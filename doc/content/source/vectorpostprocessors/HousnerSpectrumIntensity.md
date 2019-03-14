# HousnerSpectrumIntensity

The `HousnerSpectrumIntensity` calculates the Housner Spectrum Intensity (HSI) for the given list of acceleration variables that are built by the `ResponseHistoryBuilder` VectorPostprocessor. Housner Spectrum Intensity is defined as the area under the velocity spectrum curve, when plotted with period on the X axis [citep:housner1952hsi]. [citet:housner1952hsi] recommends that the area be calculated between the periods 0.25sec and 2.5sec, which are the default values for the `start_period` and `end_period` parameters in MASTODON (see parameter definitions below). The HSI values will be output into a csv file in the same order as the acceleration variables are entered in the input file.

## Usage

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called accel_hist, which builds the response histories of the variables, accel_x and accel_y. It is desired that the HSIs for accel_x and accel_y be calculated. These can be calculated by the `HousnerSpectrumIntensity` VectorPostprocessor called accel_hsi as shown in the input file below.

!listing test/tests/vectorpostprocessors/housner_spectrum_intensity/housner_spectrum_intensity.i block=VectorPostprocessors

Special care must be taken to ensure that the HSI calculations are performed only at the end of the simulation (last time step) and not at every time step. In order to accomplish this, an 'Outputs' block must be created as shown below. This block (named 'out') ensures that the CSV output is only calculated at the final timestep.

!listing test/tests/vectorpostprocessors/housner_spectrum_intensity/housner_spectrum_intensity.i block=Outputs

For such an input, the HSI calculation is performed only at the end of the simulation and the spectra are output into a csv file as shown below. The order of the HSIs is the same as the order in which, the variables are entered in the input file. In this case, the HSI for accel_x is 0.81488283655869 and for accel_y is 1.629765673084.

!listing test/tests/vectorpostprocessors/housner_spectrum_intensity/gold/housner_spectrum_intensity_out_accel_hsi.csv

!syntax parameters /VectorPostprocessors/HousnerSpectrumIntensity

!syntax inputs /VectorPostprocessors/HousnerSpectrumIntensity

!syntax children /VectorPostprocessors/HousnerSpectrumIntensity

!bibtex bibliography
