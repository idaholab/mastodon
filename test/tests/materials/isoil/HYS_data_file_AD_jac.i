# One element test to test the auto-generated GQ/H backbone curve.
# The back surface of the element (z=0) is fixed and the front surface (z=1)
# is moved by applying a cyclic preset displacement.

# The resulting shear stress-strain curve was verified against obtained from DEEPSOIL.

[Mesh]
  type = GeneratedMesh # Can generate simple lines, rectangles and rectangular prisms
  dim = 3 # Dimension of the mesh
  nx = 1 # Number of elements in the x direction
  ny = 1 # Number of elements in the y direction
  nz = 2 # Number of elements in the z direction
  xmin = 0.0
  xmax = 1
  ymin = 0.0
  ymax = 1
  zmin = 0.0
  zmax = 1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  #use_displaced_mesh = false
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[AuxVariables]
  [./stress_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    use_automatic_differentiation = true
    use_displaced_mesh = false
  [../]
[]

[AuxKernels]
  [./layer]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '4.0'
    direction = '0 0 1'
    execute_on = initial
  [../]
  [./stress_zx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zx
    index_i = 0
    index_j = 2
  [../]
  [./strain_zx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zx
    index_i = 0
    index_j = 2
  [../]
[]

[BCs]
  [./x_bot]
    type = PresetBC
    variable = disp_x
    boundary = 'back'
    value = 0.0
  [../]
  [./y_bot]
    type = PresetBC
    variable = disp_y
    boundary = 'left right top bottom'
    value = 0.0
  [../]
  [./z_bot]
    type = PresetBC
    variable = disp_z
    boundary = 'left right top bottom'
    value = 0.0
  [../]
  [./Periodic]
    [./x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = '4'
      secondary = '2'
      translation = '1.0 0.0 0.0'
    [../]
    [./y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = '1'
      secondary = '3'
      translation = '0.0 1.0 0.0'
    [../]
  [../]
 [./top_x]
    type = FunctionPresetBC
    boundary = front
    variable = disp_x
    function = top_disp
  [../]
[]

[Functions]
  [./top_disp]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 8.0'
    y = '0.0 1.0 0.0 -1.0 0.0 0.0'
    scale_factor = 3e-3
  [../]
[]

[Materials]
  [./sample_isoil]
    type = ADComputeISoilStress
    soil_type = 'user_defined'
    layer_variable = layer_id
    layer_ids = '0'
    backbone_curve_files = 'stress_strain_test_1.csv'
    poissons_ratio = '0.3'
  [../]
  [./sample_isoil_strain]
    type = ADComputeIncrementalSmallStrain
    block = '0'
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./sample_isoil_elasticitytensor]
    type = ComputeIsotropicElasticityTensorSoil
    block = '0'
    elastic_modulus = '1.0'
    poissons_ratio = '0.3'
    density = '2'
    wave_speed_calculation = false
    layer_ids = '0'
    layer_variable = layer_id
  [../]
[]

[Preconditioning]
  [./andy]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON

  start_time = 0.0
  end_time = 4.0
  dt = 0.5
  dtmin = 0.5
  timestep_tolerance = 1e-6
  nl_rel_tol = 5e-5
  nl_abs_tol = 1e-7
  #petsc_options = '-snes_test_jacobian_view'
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_6x]
    type = PointValue
    point = '0.0 0.0 1.0'
    variable = disp_x
  [../]
  [./disp_6y]
    type = PointValue
    point = '0.0 0.0 1.0'
    variable = disp_y
  [../]
  [./disp_6z]
    type = PointValue
    point = '0.0 0.0 1.0'
    variable = disp_z
  [../]
  [./strain_zx]
    type = ElementalVariableValue
    variable = strain_zx
    elementid = 1
  [../]
  [./stress_zx]
    type = ElementalVariableValue
    variable = stress_zx
    elementid = 1
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  perf_graph = false
[]
