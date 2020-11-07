# A test of the BaselineCorrection function output scaled by an input factor.
# The input acceleration time history is a half cycle of `9.87 \sin(\pi t)`

[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  solve = false
[]

[Functions]
  [corrected_disp_func]
    type = BaselineCorrection
    series_type = displacement
    time_values = '0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0'
    acceleration_values = '0.000 3.050 5.801 7.985 9.387 9.870 9.387 7.985 5.801 3.050 0.000'
    gamma = 0.5
    beta = 0.25
    accel_fit_order = 1
    scale_factor = 4.886
  []
[]

[Executioner]
  type = Transient
  num_steps = 10
  dt = 0.1
[]

[Postprocessors]
  [check_func_value]
    type = FunctionValuePostprocessor
    function = corrected_disp_func
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Outputs]
  csv = true
[]
