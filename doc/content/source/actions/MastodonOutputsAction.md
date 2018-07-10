# Mastodon Outputs Action

!syntax description /Mastodon/Outputs/MastodonOutputsAction

## Description

The Mastodon Outputs action creates the required objects for creating MASTODON simulation
outputs. Currently this action provides the option to create the stress and strain AuxVariables and
AuxKernels automatically if the user needs stress and strain outputs from the simulation. The
Mastodon Outputs action is created by the Outputs sub block of the Mastodon block as shown in the
example input file below.

!alert note
The Mastodon Outputs action is under development and more functionality will be added in the future.

## Constructed MooseObjects

[mastodon-outputs] below lists all the objects created by the input parameters in the
Mastodon/Outputs block.

!table id=mastodon-outputs caption=List of objects created by the Mastodon/Outputs block
| Functionality     | Replaced Classes   | Associated Parameters   |
|-------------------|--------------------|-------------------------|
| Stress AuxVariables     | `AuxVariable`     | Created when `stress_strain_output` = `true` |
| Strain AuxVariables     | `AuxVariable`     | Created when `stress_strain_output` = `true` |
| Stress AuxKernels           | `RankTwoAux`     | Created when `stress_strain_output` = `true` |
| Strain AuxKernels       | `RankTwoAux`     | Created when `stress_strain_output` = `true` |

## Example Input File Syntax

!listing test/tests/mastodonblock/outputs/stress_strain_output.i
         block=Mastodon

!syntax parameters /Mastodon/Outputs/MastodonOutputsAction
