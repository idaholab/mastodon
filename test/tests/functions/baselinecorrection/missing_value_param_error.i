# This input file should invoke an error, because the `time_values` parameter was
# provided, but the `acceleration_values` parameter was not.

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
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 1
  []
[]

[Executioner]
  type = Transient
[]
