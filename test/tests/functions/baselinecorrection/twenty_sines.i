# A comprehensive test of the BaselineCorrection function output. The input
# acceleration time history is twenty cycles of `-250 \pi^{2} \sin(50 \pi t)`

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
    acceleration_name = acceleration
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 9
    vel_fit_order = 9
    disp_fit_order = 7
  []
[]

[Executioner]
  type = Transient
  num_steps = 800
  dt = 1e-03
[]

[Postprocessors]
  [check_func_value]
    type = FunctionValuePostprocessor
    function = corrected_accel_func
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Outputs]
  csv = true
[]
