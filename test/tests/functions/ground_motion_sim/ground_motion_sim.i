[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Variables]
  [u]
  []
[]

[Functions]
  [groundmotion]
    type = GroundMotionSim
    M = 7.1
    R = 10
    Vs30 = 300
    n = 0
  []
[]

[Postprocessors]
  [func]
    type = FunctionValuePostprocessor
    function = groundmotion
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
  num_steps = 5000
  dt = 0.005
[]

[Outputs]
  csv = true
[]
