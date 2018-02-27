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
    y = '777 888'
  [../]
[]

[MultiApps]
  [./sub]
    type = TransientMultiApp
    positions = '0 0 0'
    input_files = 'sub.i'
    execute_on = 'TIMESTEP_BEGIN'
  [../]
[]

[Transfers]
  [./transfer]
    type = PiecewiseFunctionTransfer
    multi_app = sub
    direction = from_multiapp
    to_function = func
    from_function = func
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
