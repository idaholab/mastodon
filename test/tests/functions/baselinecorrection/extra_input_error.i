# This input file should invoke an error, because both the `data_file` parameter
# and the `time_values` and `acceleration_values` parameters were specified.

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
    data_file = twenty_sines_accel.csv
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
