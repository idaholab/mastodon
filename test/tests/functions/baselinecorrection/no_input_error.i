# This input file should invoke an error, because neither the `data_file` nor
# the `time_values` and `acceleration_values` parameters are provided.

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
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 1
  []
[]

[Executioner]
  type = Transient
[]
