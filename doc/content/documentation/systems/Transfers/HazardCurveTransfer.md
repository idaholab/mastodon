# HazardCurveTransfer

!syntax description /Transfers/HazardCurveTransfer

## Description

The `HazardCurveTransfer` is a part of the Seismic Probabilistic Risk Assessment (SPRA) framework in MASTODON. The `HazardCurveTransfer` transfers the scaled ground motion data created by the `HazardCurve` UserObject to the sub-input file. Listed below is a sample master input file that uses the `HazardCurveTransfer`. In the input file below, the 'GroundMotionReader' UserObject reads the ground motions files, and the 'HazardCurve' UserObject reads the hazard curve file, splits the hazard curve into a number of intensity bins and scales the ground motions to the intensity of each bin. The `HazardCurveMultiApp` then creates sub files corresponding to each scaled ground motion. The `HazardCurveTransfer` transfers the scaled ground motions into the sub files for performing multiple simulations.

In the listing below, a set of ground motions and a hazard curve are read by the `GroundMotionReader` and `HazardCurve` UserObjects. The ground motions are then scaled for each bin and transferred as functions in the sub-files by `HazardCurveTransfer`. This function should be defined in the sub file (listed below in hazard_curve_sub.i) and the name of this function ('func', in this case) should be provided in the `function` input parameter in the `HazardCurveTransfer`. Also the component (X, Y or Z) of the ground motion that is to be used should be provided to the `component` parameter. In order to use more than one component, multiple `HazardCurveTransfer` blocks with multiple functions should be used.

The risk assessment procedure outlined above is called time-based assessment \citep{ynhuangmceer2008}. When the risk assessment is performed for only one intensity bin of the hazard curve, it is referred to as an intensity-based assessment. Further description of the intensity-based and time-based assessment procedures can be found in \citet{ynhuangmceer2008}.

!listing test/tests/transfers/hazard_curve_transfer/hazard_curve_master.i start=[UserObjects] end=[Problem]

!listing test/tests/transfers/hazard_curve_transfer/hazard_curve_sub.i block=Functions

!syntax parameters /Transfers/HazardCurveTransfer

!syntax inputs /Transfers/HazardCurveTransfer

!syntax children /Transfers/HazardCurveTransfer

## References
\bibliographystyle{unsrt}
\bibliography{doc/bib/mastodon.bib}
