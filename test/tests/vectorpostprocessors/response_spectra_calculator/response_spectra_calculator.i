#@requirement F8.2
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 4
  xmin = 0.0
  xmax = 1.0
  ymin = 0.0
  ymax = 1.0
  zmin = 0.0
  zmax = 4.0
[]

[Controls]
   [./period0]
      type = TimePeriod
      disable_objects = 'Kernels/inertia_x Kernels/inertia_y Kernels/inertia_z AuxKernels/accel_x AuxKernels/accel_y AuxKernels/accel_z AuxKernels/vel_x AuxKernels/vel_y AuxKernels/vel_z'
      start_time = 0.0
      end_time = 0.005 # same as dt used in the analysis
   [../]
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
  [./vel_x]
  [../]
  [./accel_x]
  [../]
  [./vel_y]
  [../]
  [./accel_y]
  [../]
  [./vel_z]
  [../]
  [./accel_z]
  [../]
  [./stress_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./layer_id]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    stiffness_damping_coefficient = 0.000781
    static_initialization = True
    use_displaced_mesh = True
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    eta = 0.64026
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = True
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    eta = 0.64026
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = True
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    eta = 0.64026
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = True
  [../]
  [./gravity]
    type = Gravity
    variable = disp_z
    value = -9.81
    use_displaced_mesh = True
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
  [./accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = timestep_end
  [../]
  [./vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = timestep_end
  [../]
  [./stress_zx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zx
    index_i = 2
    index_j = 0
  [../]
  [./strain_zx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zx
    index_i = 2
    index_j = 0
  [../]
  [./layer_id]
     type = UniformLayerAuxKernel
     variable = layer_id
     interfaces = '20'
     direction = '0 0 1'
     execute_on = initial
  [../]
[]

[Materials]
   [./elasticity_tensor]
      type = ComputeIsotropicElasticityTensorSoil
      block = 0
      layer_variable = layer_id
      layer_ids = '0'
      poissons_ratio = '0.3'
      density = '2000.0'
      shear_modulus = '325e7'
   [../]
   [./stress1]
     #Computes the stress, using linear elasticity
     type = ComputeLinearElasticStress
     block = 0
    [../]
    [./strain1]
      #Computes the strain, assuming small strains
      type = ComputeSmallStrain
      block = 0
      displacements = 'disp_x disp_y disp_z'
  [../]
[]

[BCs]
  [./bottom_accel_x]
    type = PresetAcceleration
    boundary = back
    function = accel_bottom_x
    variable = disp_x
    beta = 0.25
    acceleration = accel_x
    velocity = vel_x
  [../]
  [./bottom_z]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0.0
  [../]
  [./bottom_accel_y]
    type = PresetAcceleration
    boundary = back
    function = accel_bottom_y
    variable = disp_y
    beta = 0.25
    acceleration = accel_y
    velocity = vel_y
  [../]
  [./Periodic]
    [./x_dir]
      primary = 'left'
      secondary = 'right'
      translation = '1.0 0.0 0.0'
    [../]
    [./y_dir]
      primary = 'bottom'
      secondary = 'top'
      translation = '0.0 1.0 0.0'
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
  [./accel_bottom_y]
    type = PiecewiseLinear
    data_file = 'accel.csv'
    format = columns
    scale_factor = 2.0
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  l_tol = 1e-8
  start_time = 32
  end_time = 33
  dt = 0.005 # should be 0.005
  timestep_tolerance = 1e-6
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  line_search = 'none'
[]

[Outputs]
  [./out]
    type = CSV
    execute_on = 'final'
  [../]
[]

[VectorPostprocessors]
  [./accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x accel_y'
    nodes = '8 9'
  [../]
  [./accel_spec]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist
    regularize_dt = 0.005
    outputs = out
  [../]
[]
