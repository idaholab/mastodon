# This input file should invoke an error, because no value was provided for any
# of the parameters: `accel_fit_order`, `vel_fit_order`, or `disp_fit_order`.

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
    gamma = 0.5
    beta = 0.25
  []
[]

[Executioner]
  type = Transient
[]
