[Mesh]
  type = GeneratedMesh
  dim = 3
[]

[Variables]
  [./u]
  [../]
[]

[DiracKernels]
  [./src]
    type = FunctionPointForce
    function = 'x'
    variable = u
  [../]
[]

[Problem]
  kernel_coverage_check = false
[]

[Executioner]
  type = Transient
  num_steps = 1
[]

[Outputs]
[]
