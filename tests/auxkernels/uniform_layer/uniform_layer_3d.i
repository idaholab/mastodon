#@requirement F3.3
#@requirement DI1.3
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 20
  ny = 20
  nz = 20
  xmax = 100
  ymax = 100
  zmax = 100
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
    interfaces = '20 40 78 1000'
    layer_ids = '6 24 19 80'
    direction = '1 2 3'
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
