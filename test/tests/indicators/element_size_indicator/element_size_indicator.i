[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 5
  ny = 5
[]

[Adaptivity]
  initial_marker = marker
  initial_steps = 5
  [./Indicators]
    [./minimum_element_size]
      type = ElementSizeIndicator
      element_size = ele_size
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

[Materials]
  [./ele_size]
    type = GenericConstantMaterial
    prop_names = ele_size
    prop_values = 0.05
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
