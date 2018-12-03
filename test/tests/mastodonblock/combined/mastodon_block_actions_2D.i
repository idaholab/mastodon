# One element model to test Mastodon Block Actions. Periodic boundary conditions are
# applied and an acceleration is prescribed at the bottom surface.

# UPDATE THIS FILE WITH MASTODON BLOCK ACTIONS AS THEY ARE IMPLEMENTED

[Mesh]
  type = GeneratedMesh # Can generate simple lines, rectangles and rectangular prisms
  dim = 2 # Dimension of the mesh
  nx = 1 # Number of elements in the x direction
  ny = 1 # Number of elements in the y direction
  xmin = 0.0
  xmax = 1
  ymin = 0.0
  ymax = 1
[]

[Mastodon]
  [./Model]
    dynamic_analysis = true
    dim = 2
    eta = 0.64026
    zeta = 0.000781
  [../]
  [./Outputs]
    stress_strain_output = true
  [../]
[]

[AuxVariables]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./layer]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '2.0'
    direction = '0 1 0'
    execute_on = initial
  [../]
[]

[BCs]
  [./x_bot]
    type = PresetAcceleration
    boundary = bottom
    function = accel_bottom_x
    variable = disp_x
    beta = 0.25
    acceleration = accel_x
    velocity = vel_x
  [../]
  [./y_bot]
    type = PresetBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./Periodic]
    [./x_dir]
      variable = 'disp_x disp_y'
      primary = left
      secondary = right
      translation = '1.0 0.0 0.0'
    [../]
  [../]
[]

[Functions]
  [./accel_bottom_x]
    type = PiecewiseLinear
    data_file = 'accel.csv'
    format = columns
    scale_factor = 1.0
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensorSoil
    block = 0
    layer_variable = layer_id
    layer_ids = '0'
    shear_modulus = '1.0e+2'
    poissons_ratio = '0.2'
    density = '1.0'
  [../]
  [./strain_1]
    type = ComputeSmallStrain
    block = 0
    displacements = 'disp_x disp_y'
  [../]
  [./stress_1]
    type = ComputeLinearElasticStress
    block = 0
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
  solve_type = PJFNK
  nl_abs_tol = 1e-11
  nl_rel_tol = 1e-11
  start_time = 32
  end_time = 33
  dt = 0.01
  timestep_tolerance = 1e-6
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  line_search = 'none'
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_2x]
    type = NodalVariableValue
    nodeid = 2
    variable = disp_x
  [../]
  [./disp_2y]
    type = NodalVariableValue
    nodeid = 2
    variable = disp_y
  [../]
  [./vel_2x]
    type = NodalVariableValue
    nodeid = 2
    variable = vel_x
  [../]
  [./vel_2y]
    type = NodalVariableValue
    nodeid = 2
    variable = vel_y
  [../]
  [./accel_2x]
    type = NodalVariableValue
    nodeid = 2
    variable = accel_x
  [../]
  [./accel_2y]
    type = NodalVariableValue
    nodeid = 2
    variable = accel_y
  [../]
  [./stress_xy_el]
    type = ElementalVariableValue
    variable = stress_xy
    elementid = 0
  [../]
  [./strain_xy_el]
    type = ElementalVariableValue
    variable = strain_xy
    elementid = 0
  [../]
  [./stress_xx_el]
    type = ElementalVariableValue
    variable = stress_xx
    elementid = 0
  [../]
  [./stress_yy_el]
    type = ElementalVariableValue
    variable = stress_yy
    elementid = 0
  [../]
  [./strain_xx_el]
    type = ElementalVariableValue
    variable = strain_xx
    elementid = 0
  [../]
  [./strain_yy_el]
    type = ElementalVariableValue
    variable = strain_yy
    elementid = 0
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  perf_graph = false
[]
