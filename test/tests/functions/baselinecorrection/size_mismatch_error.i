# This input file should invoke an error, because the lengths of the data
# provided for the `time_values` and `acceleration_values` parameters are not
# equal

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
    time_values = '1 2 3 4'
    acceleration_values = '1 2 3'
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 1
  []
[]

[Executioner]
  type = Transient
[]
