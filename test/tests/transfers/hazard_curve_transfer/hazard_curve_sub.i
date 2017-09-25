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
    x = '0 1'
    y = '200 2000'
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
  dt = 0.01
[]

[Postprocessors]
  [./top]
    type = FunctionValuePostprocessor
    function = func
    point = '0.5 0.85 0' # This is not used but is a required input. Output is being interpolated at time.
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
