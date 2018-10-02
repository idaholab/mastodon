### Linear elastic soil

A linear elastic soil can be defined in MASTODON by defining a elasticity tensor using
ComputeIsotropicElasticityTensor, stress calculator using ComputeLinearElasticStress, and a small
strain calculator using ComputeSmallStrain. Density can be defined using the
GenericConstantMaterial. An example of these input blocks is shown below:

!listing test/tests/dirackernels/seismic_source/one_seismic_source.i
         start=Materials
         end=Executioner
