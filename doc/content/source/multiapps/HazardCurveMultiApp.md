# HazardCurveMultiApp

!syntax description /MultiApps/HazardCurveMultiApp

## Description

The `HazardCurveMultiApp` is a part of the Seismic Probabilistic Risk Assessment (SPRA) framework in MASTODON. The `HazardCurveMultiApp` creates sub-input files for each ground motion created by the `HazardCurve` UserObject. Listed below is a sample master input file that uses the HazardCurveMultiApp. In the input file below, the `GroundMotionReader` UserObject reads the ground motions files, and the `HazardCurve` UserObject reads the hazard curve file, splits the hazard curve into a number of intensity bins and scales the ground motions to the intensity of each bin. The `HazardCurveMultiApp` then creates sub files corresponding to each scaled ground motion. The `HazardCurveTransfer` transfers the scaled ground motions into the sub files for performing multiple simulations.

The risk assessment procedure outlined above is called time-based assessment [citep:ynhuangmceer2008]. When the risk assessment is performed for only one intensity bin of the hazard curve, it is referred to as an intensity-based assessment. Further description of the intensity-based and time-based assessment procedures can be found in [citet:ynhuangmceer2008].

!listing test/tests/transfers/hazard_curve_transfer/hazard_curve_master.i start=[UserObjects] end=[Problem]

!syntax parameters /MultiApps/HazardCurveMultiApp

!syntax inputs /MultiApps/HazardCurveMultiApp

!syntax children /MultiApps/HazardCurveMultiApp



!bibtex bibliography
