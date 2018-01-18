[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Variables]
  [./u]
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

[Outputs]
  exodus = true
[]
