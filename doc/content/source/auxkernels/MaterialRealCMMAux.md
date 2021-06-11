# MaterialRealCMMAux

!syntax description /AuxKernels/MaterialRealCMMAux

## Description

`MaterialRealCMMAux` can be used to populate an `AuxVariable` with an element of
a material property that is of type, ColumnMajorMatrix (CMM). In MASTODON, a
common use case is accessing the deformation or force CMMs from link elements
when the [`LinearSpring`](source/materials/LinearSpring.md) material or any of
the seismic isolator materials (e.g., [`ComputeFPIsolatorElasticity`](source/materials/ComputeFPIsolatorElasticity.md)) is used. This `AuxKernel` and the corresponding
`AuxVariable` should be block-restricted to blocks where the material property
given by the `property` input parameter below is defined.

For example, the following input file syntax will create the AuxVariable field
called `global_force_x` with the X direction force in the spring element modeled
using the LinearSpring material. The AuxKernel is defined as follows:

!listing test/tests/auxkernels/materialrealcmm/spring_static.i block=AuxKernels

In this input, the AuxKernel takes the element (0, 0) from the CMM material
property named, "ls_global_forces". This material property name refers to the
global forces in the linear spring material. (See LinearSpring.C for other
material properties.) In this example, the linear spring material is only defined
in block '0' and therefore this AuxKernel and AuxVariable are also restricted to
this block.

!syntax parameters /AuxKernels/MaterialRealCMMAux

!syntax inputs /AuxKernels/MaterialRealCMMAux

!syntax children /AuxKernels/MaterialRealCMMAux
