# This input file should invoke an error, because no data was provided for
# either the `time_values` or `acceleration_values` parameters

[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  solve = false
[]

[Functions]
  [corrected_accel_func]
    type = BaselineCorrection
    time_values = ''
    acceleration_values = ''
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 1
  []
[]

[Executioner]
  type = Transient
[]
