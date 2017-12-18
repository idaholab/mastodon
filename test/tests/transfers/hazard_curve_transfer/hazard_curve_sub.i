[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[]

[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./rank]
  [../]
[]

[Functions]
  [./func]
    type = PiecewiseLinear
    x = '100 101'
    y = '200 201'
  [../]
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
  solve = false
[]

[Executioner]
  type = Transient
  num_steps = 20
  dt = 0.1
[]

[Postprocessors]
  [./top]
    type = FunctionValuePostprocessor
    function = func
    point = '0.5 0.85 0'
    execute_on = 'timestep_end'
  [../]
  [./bottom]
    type = FunctionValuePostprocessor
    function = func
    point = '0.5 0.15 0'
    execute_on = 'timestep_end'
  [../]
[]

[AuxKernels]
  [./rank]
    type = ProcessorIDAux
    variable = rank
    execute_on = initial
  [../]
[]

[Outputs]
  csv = true
[]
