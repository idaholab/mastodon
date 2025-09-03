[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 15
  xmin = 0
  ymin = 0
  zmin = 0.0
  xmax = 1
  ymax = 1
  zmax = 29.47
  dim = 3
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
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
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
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
  [./layer_id]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '29.8'
    direction = '0.0 0.0 1.0'
    execute_on = initial
  [../]
[]

[BCs]
  [./bottom_z]
    type = DirichletBC
    variable = disp_z
    boundary = 'back'
    value = 0.0
  [../]
  [./bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'back'
    value = 0.0
  [../]
  [./bottom_accel]
    type = PresetAcceleration
    boundary = 'back'
    function = accel_bottom
    variable = 'disp_x'
    beta = 0.25
    acceleration = 'accel_x'
    velocity = 'vel_x'
  [../]
  [./Periodic]
    [./y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'bottom'
      secondary = 'top'
      translation = '0.0 1.0 0.0'
    [../]
    [./x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'left'
      secondary = 'right'
      translation = '1.0 0.0 0.0'
    [../]
  [../]
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Ormsby_USE1.csv
    scale_factor = 1.0
    format = 'columns'
  [../]
  [./initial_zz]
    type = ParsedFunction
    value = '-1.7 * 9.81 * (29.47 - z)'
  [../]
  [./initial_xx]
    type = ParsedFunction
    value = '-1.7 * 9.81 * (29.47 - z) * 0.3/0.7'
  [../]
[]

[Materials]
  [./elastic_soil]
    type = ComputeIsotropicElasticityTensorSoil
    layer_variable = layer_id
    layer_ids = '0'
    poissons_ratio = '0.3'
    density = '1.7'
    shear_modulus = '46282.5'
  [../]
  [./stress_beam2]
    type = ComputeFiniteStrainElasticStress
  [../]
  [./strain_15]
    type = ComputeFiniteStrain
    displacements = 'disp_x disp_y disp_z'
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
  solve_type = 'NEWTON'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  start_time = 0.0
  end_time = 4.0
  dt = 0.001
  dtmin = 0.0001
  nl_abs_tol = 1e-4
  nl_rel_tol = 1e-5
  l_tol = 1e-5
  l_max_its = 25
  timestep_tolerance = 1e-8
[]

[VectorPostprocessors]
  [./accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x'
    nodes = '3 63'
  [../]
  [./accel_spec]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist
    regularize_dt = 0.001
    damping_ratio = 0.01
    start_frequency = 0.1
    end_frequency = 1000
    outputs = out
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = true
  [./out]
    execute_on = 'FINAL'
    type = CSV
    file_base = Soil_Final_NoDamp
  [../]
  [./out2]
    type = Exodus
    file_base = Exod_Soil_Final_NoDamp
    time_step_interval = 10
  [../]
[]
