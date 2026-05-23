[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Problem]
  solve = false
[]

[Functions]
  [function]
    type = PiecewiseLinear
    x = '0 1'
    y = '0 1'
  []
  [bad_function]
    type = ConstantFunction
  []
  [error_test]
    type = BaselineCorrection
    function = function
    disp_fit_order = 10
  []
[]

[Executioner]
  type = Steady
[]
