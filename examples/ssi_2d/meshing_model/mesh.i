[Mesh]
  file = 2D_lanl_ssi.e
[]

[AuxVariables]
  [./layers]
    family = MONOMIAL
    order = CONSTANT
  [../]
  [./hmin]
     family = MONOMIAL
     order = CONSTANT
  [../]
[]

[AuxKernels]
  [./layer_id]
     type = UniformLayerAuxKernel
     variable = layers
     interfaces = '28 64 163 192 245 264 284 284.8333 324.8333'
     layer_ids =  ' 0  1   2   3   4   5  6   7        8'
     direction = '0.0 1.0 0.0'
     execute_on = initial
  [../]
  [./hmin]
    type = ElementLengthAux
    variable = hmin
    method = max
    execute_on = 'initial timestep_end'
  [../]
[]

[Adaptivity]
  marker = marker
  steps = 5
  max_h_level = 5

  [./Indicators]
    [./minimum_element_size]
      type = ShearWaveIndicator
      cutoff_frequency = 50
    [../]
#    [./layer_interface]
#      type = LayerInterfaceIndicator
#      variable = layers
#    [../]
  [../]
  [./Markers]
    [./marker]
      type = MinimumElementSizeMarker
      indicator = minimum_element_size
      scale = 0.1
    [../]
#    [./interface_marker]
#      type = ErrorToleranceMarker
#      indicator = layer_interface
#      refine = 0.5
#    [../]
#    [./marker]
#       type = ComboMarker
#       markers = 'minimum_element_marker interface_marker'
#    [../]
  [../]
[]

[Materials]
  [./linear]
    type = LinearSoilMaterial
    layer_variable = layers
    layer_ids      = '0 1 2 3 4 5 6 7 8'
    shear_modulus = '23868.598 9399.222 18371.573 3166.9132 6635.72168 8972.0412 2745.578 5741.43725 1.04e6'
    density = '0.0031045 0.0031045 0.0032985 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045'
    outputs = exodus
    block = '1 2 3 4'
  [../]
[]

[Postprocessors]
  [./n_elems]
    type = NumElems
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 8
[]

[Problem]
  solve = false
[]

[Outputs]
  exodus = true
[]
