# HousnerSpectrumIntensity
The `HousnerSpectrumIntensity` calculates the Housner Spectrum Intensity (HSI) for the given list of acceleration variables that are built by the `ResponseHistoryBuilder` VectorPostprocessor. Housner Spectrum Intensity is defined as the area under the velocity spectrum curve, when plotted with period on the X axis (Housner, 1952). Housner (1952) recommends that the area be calculated between the periods 0.25sec and 2.5sec, which are the default values for the `start_period` and `end_period` parameters in MASTODON (see parameter definitions below). The HSI values will be output into a csv file in the same order as the acceleration variables are entered in the input file.

For example, the input file below contains a `ResponseHistoryBuilder` VectorPostprocessor called accel_hist, which builds the response histories of the variables, accel_x and accel_y. It is desired that the HSIs for accel_x and accel_y be calculated. These can be calculated by the `HousnerSpectrumIntensity` VectorPostprocessor called accel_hsi as shown in the input file below.

!listing tests/vectorpostprocessors/housner_spectrum_intensity/housner_spectrum_intensity.i block=VectorPostprocessors

The HSI calculation is performed only at the end of the simulation and the spectra are output into a csv file as shown below. The order of the HSIs is the same as the order in which, the variables are entered in the input file. In this case, the HSI for accel_x is 0.8149 and the HSI for accel_y is 3.5484e-19.

!listing tests/vectorpostprocessors/housner_spectrum_intensity/gold/housner_spectrum_intensity_accel_csv_accel_hsi_0201.csv

!parameters /VectorPostprocessors/HousnerSpectrumIntensity

!inputfiles /VectorPostprocessors/HousnerSpectrumIntensity

!childobjects /VectorPostprocessors/HousnerSpectrumIntensity
