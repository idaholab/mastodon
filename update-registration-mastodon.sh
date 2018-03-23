#!/bin/bash
# run this script with one or more arguments that are the directories that hold the code files for
# the objects you want to update.

dirs=$@

appname='MastodonApp'

objs='
NonReflectingBC
SeismicForce
UniformLayerAuxKernel
FunctionPointForce
SeismicSource
ShearWaveIndicator
LayerInterfaceIndicator
LinearSoilMaterial
ComputeIsotropicElasticityTensorSoil
ComputeISoilStress
MinimumElementSizeMarker
ResponseHistoryBuilder
ResponseSpectraCalculator
HousnerSpectrumIntensity
Fragility
GroundMotionReader
HazardCurve
HazardCurveMultiApp
HazardCurveTransfer
PiecewiseFunctionTransfer
'

actions='
SeismicForceAction add_bc
SeismicDisplacementAction add_bc
NonReflectingBCAction add_bc
SeismicSourceAction add_dirac_kernel
ISoilAction add_material
MastodonModelAction meta_action
MastodonModelAction add_variable
MastodonModelAction add_aux_variable
MastodonModelAction add_kernel
MastodonModelAction add_aux_kernel
MastodonOutputsAction add_aux_variable
MastodonOutputsAction add_aux_kernel
'

for obj in $objs; do
    find $dirs | grep "/${obj}\.C$" | xargs perl -0777 -i'' -pe 's/(template ?<>\nInputParameters\n? ?validParams<(\w*)>\(\))/registerMooseObject\("'"$appname"'", \2\);\n\n\1/igs'
done

while read action task; do
    find $dirs | grep "/${action}\.C$" | xargs perl -0777 -i'' -pe 's/(template ?<>\nInputParameters\n? ?validParams<(\w*)>\(\))/registerMooseAction\("'"$appname"'", \2, "'"$task"'"\);\n\n\1/igs'
done <<< "$actions"
