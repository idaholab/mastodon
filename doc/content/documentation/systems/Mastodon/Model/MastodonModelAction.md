# Mastodon Model Action

!syntax description /Mastodon/Model/MastodonModelAction

## Description

The Mastodon Model action creates the required displacement Variables, velocity and acceleration
AuxVariables and AuxKernels, inertia Kernels and the DynamicTensorMechanicsAction. These objects are
created based on the dimension of the mesh of the problem and if static or dynamic analysis is
performed. The Mastodon Model action is created by the Model sub block of the Mastodon block as shown
in the example input file below.

## Constructed MooseObjects

[model-objects] below lists all the objects created by the input parameters in the Mastodon/Model
block.

!table id=model-objects caption=List of objects created by the Mastodon/Model block
| Functionality     | Replaced Classes   | Associated Parameters   |
|-------------------|--------------------|-------------------------|
| Displacement variables        | `Variable`     | Created automatically when the Mastodon/Model block is created |
| DynamicTensorMechanicsAction  | `DynamicTensorMechanicsAction` | Created automatically when the Mastodon/Model block is created |
| Inertia kernels               | `InertialForce`     | Created when `dynamic_analysis` = `true`. Also uses the `use_displaced_mesh`, `beta` and `gamma` parameters |
| Velocity AuxVariables         | `AuxVariable`     | Created when `dynamic_analysis` = `true` |
| Acceleration AuxVariables     | `AuxVariable`     | Created when `dynamic_analysis` = `true` |
| Velocity AuxKernels           | `NewmarkVelAux`     | Created when `dynamic_analysis` = `true` |
| Acceleration AuxKernels       | `NewmarkAccelAux`     | Created when `dynamic_analysis` = `true` |

## Example Input File Syntax

!listing test/tests/mastodonblock/model/model_dynamic.i block=Mastodon

!syntax parameters /Mastodon/Model/MastodonModelAction
