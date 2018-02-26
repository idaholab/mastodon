[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Variables]
  [./u]
  [../]
[]

[Functions]
  [./func]
    type = PiecewiseLinear
    x = '0 1'
    y = '2 4'
  [../]
[]

[Postprocessors]
  [./func]
    type = FunctionValuePostprocessor
    function = func
    execute_on = "INITIAL TIMESTEP_END"
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
  kernel_coverage_check = false
[]

[Executioner]
  type = Transient
  num_steps = 2
  dt = 0.5
[]

[Outputs]
  csv = true
[]
