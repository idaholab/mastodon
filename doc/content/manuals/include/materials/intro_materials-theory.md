## Material models

To model the mechanical behavior of a material, three components need to be defined at every point in space and time - strain, elasticity tensor, stress.

1. +Strain+: Strain is a normalized measure of the deformation experienced by a material. In a 1-D
   scenario, say a truss is stretched along its axis, the axial strain is the elongation of the truss
   normalized by the length of the truss. In a 3D scenario, the strain is 3x3 tensor and there are
   three different ways to calculate strains from displacements - small linearized total strain,
   small linearized incremental strain, and finite incremental strain. Details about these methods
   can be found in [modules/tensor_mechanics/index.md].

2. +Elasticity Tensor+: The elasticity tensor is a 4th order tensor with a maximum of 81
   independent constants. For MASTODON applications, the soil and structure are usually assumed to
   behave isotropically, i.e., the material behaves the same in all directions. Under this
   assumption, the number of independent elastic constants reduces from 81 to 2. The two independent
   constants that are usually provided for the soil are the shear modulus and Poissons's ratio, and
   for the structure it is the Young's modulus and Poisson's ratio.

3. +Stress+: The stress at a point in space and time is a 3x3 tensor which is a function of the
   strain at that location.  The function that relates the stress tensor to the strain tensor is the
   constitutive model. Depending on the constitutive model, the material can behave elastically or
   plastically with an increment in strain.
