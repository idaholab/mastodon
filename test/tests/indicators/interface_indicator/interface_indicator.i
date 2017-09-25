[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 30
  ny = 30
  xmax = 300
  ymax = 300
[]


[Adaptivity]
  [./Indicators]
    [./interface_error]
      type = LayerInterfaceIndicator
      variable = layer
    [../]
  [../]
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    boundary = left
    variable = u
    value = 1
  [../]
  [./right]
    type = DirichletBC
    boundary = right
    variable = u
    value = 0
  [../]
[]

[AuxVariables]
  [./layer]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Functions]
  [./image]
    type = ImageFunction
    file = ../../images/three_layer.png
    component = 0
  [../]
[]

[ICs]
  [./layer_ic]
    type = FunctionIC
    function = image
    variable = layer
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1
[]

[Outputs]
  execute_on = 'TIMESTEP_END'
  hide = 'u'
  exodus = true
[]
