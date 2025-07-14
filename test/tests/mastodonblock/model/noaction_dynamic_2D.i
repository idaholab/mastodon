# One element 2D model to test Mastodon Actions. Periodic boundary conditions are
# applied and an acceleration is prescribed at the bottom surface.

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
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[AuxVariables]
  [./vel_x]
  [../]
  [./accel_x]
  [../]
  [./vel_y]
  [../]
  [./accel_y]
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yy]
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
    block = 0
    displacements = 'disp_x disp_y'
    stiffness_damping_coefficient = 0.000781
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    eta = 0.64026
    use_displaced_mesh = false
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    eta = 0.64026
    use_displaced_mesh = false
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = timestep_end
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = timestep_end
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = timestep_end
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = timestep_end
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 1
    index_j = 0
  [../]
  [./strain_xy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = stress_xy
    index_i = 1
    index_j = 0
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
  [../]
  [./strain_yy]
    type = RankTwoAux
    rank_two_tensor =total_strain
    variable = strain_yy
    index_i = 1
    index_j = 1
  [../]
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
    type = DirichletBC
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
