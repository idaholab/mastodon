# UniformLayerAuxKernel
!syntax description /AuxKernels/UniformLayerAuxKernel

## Description

The `UniformLayerAuxKernel` is designed to create an arbitrary number of planar layers to be used
as layer identifiers. The layers created can be oriented in any direction.

For example, the following input file syntax will create the AuxVariable field shown in \ref{uniform_layer_3d}.

!listing tests/auxkernels/uniform_layer/uniform_layer_3d.i block=AuxKernels

The "interfaces" parameters is a list of layer interface locations in the direction of the "direction" vector
parameter. Optionally, the "ids" of each of the layers that will be created may be provided in the "layer_ids"
parameter, if this list is omitted then the "ids" are assigned in ascending order beginning at 0.

!media media/uniform_layer_3d.png style=width:50%;margin-left:auto;margin-right:auto; caption=Example of oriented "layers" in 3D. id=uniform_layer_3d

!syntax parameters /AuxKernels/UniformLayerAuxKernel

!syntax inputs /AuxKernels/UniformLayerAuxKernel

!syntax children /AuxKernels/UniformLayerAuxKernel
