[GlobalParams]
  execute_on = INITIAL
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  solve = false
[]

[Functions]
  [nominal_accel]
    type = PiecewiseLinear
    data_file = sine_wave.csv
  []
  [type_A3]
    type = BaselineCorrection
    function = nominal_accel
  []
  [type_V3]
    type = BaselineCorrection
    function = nominal_accel
    vel_fit_order = 3
    series_output = velocity
  []
  [type_A13V13D11]
    type = BaselineCorrection
    function = nominal_accel
    accel_fit_order = 13
    vel_fit_order = 13
    disp_fit_order = 11
    series_output = displacement
  []
[]

[Executioner]
  type = Steady
[]

[VectorPostprocessors]
  [accel]
    type = PiecewiseFunctionTabulate
    function = type_A3
    contains_complete_history = true
  []
  [vel]
    type = PiecewiseFunctionTabulate
    function = type_V3
    contains_complete_history = true
  []
  [disp]
    type = PiecewiseFunctionTabulate
    function = type_A13V13D11
    contains_complete_history = true
  []
[]

[Outputs]
  csv = true
  file_base = corrected
[]
