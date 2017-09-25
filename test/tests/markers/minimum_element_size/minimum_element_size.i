[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  xmax = 20
  ymax = 20
[]

[Adaptivity]
  initial_marker = marker
  initial_steps = 5
  [./Indicators]
    [./minimum_element_size]
      type = ShearWaveIndicator
      cutoff_frequency = 25
    [../]
  [../]
  [./Markers]
    [./marker]
      type = MinimumElementSizeMarker
      indicator = minimum_element_size
    [../]
  []
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
  [./hmin]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./hmin]
    type = ElementLengthAux
    variable = hmin
    method = min
    execute_on = 'initial timestep_end'
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
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  hide = 'u'
  exodus = true
[]
