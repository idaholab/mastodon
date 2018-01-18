# HazardCurve

!syntax description /UserObjects/HazardCurve

## Description

The `HazardCurve` UserObject is a part of the Seismic Probabilistic Risk Assessment framework in MASTODON. The `HazardCurve` UserObject reads the hazard curve data from a .csv file and divides the hazard curve into a number of bins provided by the `number_of_bins` parameter. Currently, the UserObject assumes that the hazard curve is provided as the Mean Annual Frequency of Exceedance (MAFE) of the peak ground acceleration (PGA). The hazard curve is binned by linearly interpolating the hazard curve in a log-linear scale (MAFE in the log scale and PGA in the linear scale). Each bin is assigned a PGA value that is the midpoint of the bin. After binning the hazard curve, the `HazardCurve` UserObject also scales the ground motions read by the `GroundMotionReader` for use in risk assessment for each hazard bin. For this purpose, it is assumed that the input ground motions are at a reference PGA provided by the `reference_acceleration` input parameter. The ground motions are then linearly scaled to the PGA of each bin and then used for risk assessment.

The listing below provides a sample application of the `HazardCurve` and `GroundMotionReader` UserObjects. In this application, the ground motions with a specific pattern are read and a hazard curve is read from the file hazard.csv. This hazard curve is split into two bins and the ground motions (assumed to be provided at a reference PGA of 0.4g) are scaled to these bins.

!listing test/tests/transfers/hazard_curve_transfer/hazard_curve_master.i block=UserObjects

!syntax parameters /UserObjects/HazardCurve

!syntax inputs /UserObjects/HazardCurve

!syntax children /UserObjects/HazardCurve
