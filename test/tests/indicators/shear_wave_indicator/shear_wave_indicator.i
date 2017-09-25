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
    [./minimum_element_size]
      type = ShearWaveIndicator
      cutoff_frequency = 25
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

[Materials]
  [./linear]
    type = LinearSoilMaterial
    layer_variable = layer
    layer_ids     = '32     64     128'
    shear_modulus = '1.35e6 4.25e8 5.623e9'
    density       = '1500   1700   2500'
    outputs = exodus
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
