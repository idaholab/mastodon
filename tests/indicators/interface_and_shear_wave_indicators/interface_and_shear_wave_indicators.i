[Mesh]
  type = GeneratedMesh
  dim = 2
  xmax = 12000
  ymax = 200
  nx = 120 # don't make this smaller, this was the smallest I could used to reproduce the MPI error
  ny = 20  # ditto
[]

[Adaptivity]
  initial_marker = marker
  initial_steps = 2
  max_h_level = 2
  [./Indicators]
    [./minimum_element_size]
      type = ShearWaveIndicator
      cutoff_frequency = 2
    [../]
    [./layer_interface]
       type = LayerInterfaceIndicator
       variable = layer
    [../]
  [../]
  [./Markers]
    [./minimum_element_marker]
      type = MinimumElementSizeMarker
      indicator = minimum_element_size
      factor = 0.1
    [../]
    [./interface_marker]
      type = ErrorToleranceMarker
      indicator = layer_interface
      refine = 0.5
    [../]
    [./marker]
      type = ComboMarker
      markers = 'minimum_element_marker interface_marker'
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
    boundary = 1
    variable = u
    value = 1
  [../]
  [./right]
    type = DirichletBC
    boundary = 2
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
    file = input.png
    component = 0
  [../]
[]

[ICs]
  [./id_ic]
    type = FunctionIC
    function = image
    variable = layer
  [../]
[]

[Materials]
  [./linear]
    type = LinearSoilMaterial
    layer_variable = layer
    layer_ids = '204 179 153 128 102 77 51 26 0'
    shear_modulus = '8.585e7 4.924e8 2.689e8 1.471e8 7.266e8 4.245e8 8.3911e8 1.26e9 1.668e9'
    density = '1600.0 1600.0 1600.0 1600.0 1700.0 1600.0 1700.0 1700.0 1700.0'
    outputs = exodus
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre    boomeramg'
[]

[Outputs]
  hide = 'u'
  exodus = true
[]
