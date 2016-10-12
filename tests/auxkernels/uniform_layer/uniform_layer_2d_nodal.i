[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmax = 100
  ymax = 100
[]

[AuxVariables]
  [./layer_id]
  [../]
[]

[AuxKernels]
  [./layers]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '20 40 78 1000'
    layer_ids = '6 24 19 80'
    direction = '1 1 0'
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
