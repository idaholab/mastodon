[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
  xmax = 100
[]

[AuxVariables]
  [./layer_id]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./layers]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '10 20 50 1000'
    layer_ids = '6 24 19 80'
    execute_on = initial
  [../]
[]

[Executioner]
  type = Steady
[]

[Problem]
  solve = false
[]

[Outputs]
  exodus = true
[]
