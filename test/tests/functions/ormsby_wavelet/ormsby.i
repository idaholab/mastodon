[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Variables]
  [u]
  []
[]

[Functions]
  [ormsby]
    type = OrmsbyWavelet
    f1 = 0.0
    f2 = 0.2
    f3 = 20.0
    f4 = 25.0
    ts = 2.0
    scale_factor = 0.5
  []
[]

[Postprocessors]
  [func]
    type = FunctionValuePostprocessor
    function = ormsby
    execute_on = "INITIAL TIMESTEP_END"
  []
[]

[Problem]
  type = FEProblem
  solve = false
  kernel_coverage_check = false
[]

[Executioner]
  type = Transient
  num_steps = 400
  dt = 0.01
[]

[Outputs]
  csv = true
[]
