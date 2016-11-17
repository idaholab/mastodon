# UniformLayerAuxKernel
!description /AuxKernels/UniformLayerAuxKernel

## Description
!image media/uniform_layer_3d.png padding-left=20px width=60% float=right caption=Example of oriented "layers" in 3D.

The `UniformLayerAuxKernel` is designed to create an arbitrary number of planar layers to be used
as layer identifiers. The layers created can be oriented in any direction.

For example, the following input file syntax will create the AuxVariable field shown in the adjacent figure.

!input tests/auxkernels/uniform_layer/uniform_layer_3d.i block=AuxKernels

The "interfaces" parameters is a list of layer interface locations in the direction of the "direction" vector
parameter. Optionally, the "ids" of each of the layers that will be created may be provided in the "layer_ids"
parameter, if this list is omitted then the "ids" are assigned in ascending order beginning at 0.


!parameters /AuxKernels/UniformLayerAuxKernel

!inputfiles /AuxKernels/UniformLayerAuxKernel

!childobjects /AuxKernels/UniformLayerAuxKernel
