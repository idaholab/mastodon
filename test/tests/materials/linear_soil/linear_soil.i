cd [Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 2
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
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[ICs]
  [./layer_ic]
    type = FunctionIC
    function = 'if(x<0.5,1980,1949)'
    variable = layer
  [../]
[]

[Materials]
  [./linear]
    type = LinearSoilMaterial
    layer_variable = layer
    layer_ids     = '1980   1949'
    shear_modulus = '1.35e6 4.25e8'
    density       = '1500   1700'
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
