[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 17
  xmin = -0.05
  ymin = -0.05
  zmin = 0.0
  xmax = 0.05
  ymax = 0.05
  zmax = 32.0
  dim = 3
[]

[Variables]
  [disp_x]
    order = FIRST
    family = LAGRANGE
  []
  [disp_y]
    order = FIRST
    family = LAGRANGE
  []
  [disp_z]
    order = FIRST
    family = LAGRANGE
  []
[]

[AuxVariables]
  [vel_x]
  []
  [accel_x]
  []
  [vel_y]
  []
  [accel_y]
  []
  [vel_z]
  []
  [accel_z]
  []
  [layer_id]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Kernels]
  [DynamicTensorMechanics]
    stiffness_damping_coefficient = 0.0017490 #1.5% 1-20hz
    displacements = 'disp_x disp_y disp_z'
    static_initialization = true
  []
  [inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    eta = 0.0607069 #1.5% 1-20hz
  []
  [inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    eta = 0.0607069 #1.5% 1-20hz
  []
  [inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    eta = 0.0607069 #1.5% 1-20hz
  []
  [gravity]
    type = Gravity
    variable = disp_z
    value = -9.81
  []
[]

[AuxKernels]
  [accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = timestep_end
  []
  [accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = timestep_end
  []
  [accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = timestep_end
  []
  [layer_id]
     type = UniformLayerAuxKernel
     variable = layer_id
     interfaces = '32'
     direction = '0.0 0.0 1.0'
     execute_on = initial
  []
[]

[Materials]
  [I_Soil]
    [soil_all]
      block = 0
      layer_variable = layer_id
      layer_ids = '0'
      soil_type = 'user_defined'
      backbone_curve_files = '../backbone.csv'
      displacements = 'disp_x disp_y disp_z'
      poissons_ratio = '0.3'
      initial_shear_modulus = '0.523325'
      density = '1.730e-6'
      a0 = 1.0
      a1 = 0.0
      a2 = 0.0
      b_exp = 0.0
    []
  []
[]

[Controls]
  [inertia_switch]
    type = TimePeriod
    start_time = 0.0
    end_time = 0.01
    disable_objects = '*/inertia_x */inertia_y */inertia_z */vel_x */vel_y */vel_z  */accel_x */accel_y */accel_z'
    set_sync_times = true
    execute_on = 'timestep_begin timestep_end'
  []
[]

[Functions]
  [input_motion_x]
    type = PiecewiseLinear
    data_file = '../ground_motion.csv'
    format = columns
    xy_in_file_only = false
    y_index_in_file = 1
  []
  [input_motion_y]
    type = PiecewiseLinear
    data_file = '../ground_motion.csv'
    format = columns
    xy_in_file_only = false
    y_index_in_file = 2
  []
  [input_motion_z]
    type = PiecewiseLinear
    data_file = '../ground_motion.csv'
    format = columns
    xy_in_file_only = false
    y_index_in_file = 3
  []
[]

[BCs]
  [x_motion]
    type = PresetAcceleration
    acceleration = accel_x
    velocity = vel_x
    variable = disp_x
    beta = 0.25
    boundary = 'back'
    function = 'input_motion_x'
  []
  [y_motion]
    type = PresetAcceleration
    acceleration = accel_y
    velocity = vel_y
    variable = disp_y
    beta = 0.25
    boundary = 'back'
    function = 'input_motion_y'
  []
  [z_motion]
    type = PresetAcceleration
    acceleration = accel_z
    velocity = vel_z
    variable = disp_z
    beta = 0.25
    boundary = 'back'
    function = 'input_motion_z'
  []
  [Periodic]
    [y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'bottom'
      secondary = 'top'
      translation = '0.0 0.1 0.0'
    []
    [x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'left'
      secondary = 'right'
      translation = '0.1 0.0 0.0'
    []
  []
[]

[VectorPostprocessors]
  [accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x accel_y accel_z'
    nodes = '0 71'
  []
  [accel_spec]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist
    regularize_dt = 0.002
    damping_ratio = 0.05
    start_frequency = 0.1
    end_frequency = 1000
    outputs = out1
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  petsc_options = '-ksp_snes_ew'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  solve_type = 'NEWTON'
  nl_rel_tol = 1e-7
  nl_abs_tol = 1e-12
  # l_tol = 1e-2
  dt = 0.01
  end_time = 28.0
  timestep_tolerance = 1e-6
  [TimeIntegrator]
    type = NewmarkBeta
    beta = 0.25
    gamma = 0.5
  []
[]

[Outputs]
  exodus = true
  perf_graph = true
  csv = true
  [out1]
    type = CSV
    execute_on = 'final'
  []
[]
