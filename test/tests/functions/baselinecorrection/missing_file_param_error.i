# This input file should invoke an error, because the `time_name` parameter was
# provided, but the `acceleration_name` parameter was not.

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
    time_name = time
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 1
  []
[]

[Executioner]
  type = Transient
[]
