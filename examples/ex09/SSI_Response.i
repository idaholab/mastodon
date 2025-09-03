[Mesh]
  type = FileMesh
  file = Full_3D_Str_red.e
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
  [./rot_x]
    block = '3 4 6'
  [../]
  [./rot_y]
    block = '3 4 6'
  [../]
  [./rot_z]
    block = '3 4 6'
  [../]
[]

[AuxVariables]
  [./stress_x]
  [../]
  [./stress_y]
  [../]
  [./stress_z]
  [../]
  [./vel_x]
  [../]
  [./vel_y]
  [../]
  [./vel_z]
  [../]
  [./accel_x]
  [../]
  [./accel_y]
  [../]
  [./accel_z]
  [../]
  [./rot_vel_x]
    block = '3 4 6'
  [../]
  [./rot_vel_y]
    block = '3 4 6'
  [../]
  [./rot_vel_z]
    block = '3 4 6'
  [../]
  [./rot_accel_x]
    block = '3 4 6'
  [../]
  [./rot_accel_y]
    block = '3 4 6'
  [../]
  [./rot_accel_z]
    block = '3 4 6'
  [../]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Physics/SolidMechanics/LineElement/QuasiStatic]
  displacements = 'disp_x disp_y disp_z'
  rotations = 'rot_x rot_y rot_z'

  # dynamic simulation using consistent mass/inertia matrix
  dynamic_nodal_translational_inertia = true

  velocities = 'vel_x vel_y vel_z'
  accelerations = 'accel_x accel_y accel_z'
  rotational_velocities = 'rot_vel_x rot_vel_y rot_vel_z'
  rotational_accelerations = 'rot_accel_x rot_accel_y rot_accel_z'

  beta = 0.25 # Newmark time integration parameter
  gamma = 0.5 # Newmark time integration parameter

  [./block_3]
    block = 3
    area = 500
    Iy = 1e3
    Iz = 1e3
    y_orientation = '0.0 1.0 0.0'
    nodal_mass = 0.1
    boundary = 'B0'
  [../]
  [./block_4]
    block = 4
    area = 500
    Iy = 1e3
    Iz = 1e3
    y_orientation = '1.0 0.0 0.0'
    nodal_mass = 0.1
    boundary = 'B0'
  [../]
  [./block_6]
    block = 6
    area = 500
    Iy = 1e3
    Iz = 1e3
    y_orientation = '0.0 1.0 0.0'
    nodal_mass = 166.0
    density = 2.7
    boundary = 'B1'
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    block = '1 2'
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = '1 2'
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    block = '1 2'
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    block = '1 2'
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./layer_id]
    type = UniformLayerAuxKernel
    block = '1'
    variable = layer_id
    interfaces = '29.6'
    direction = '0.0 0.0 1.0'
    execute_on = initial
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
    block = '1'
  [../]
  [./elasticity_base]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1e10
    poissons_ratio = 0
    block = '2'
  [../]
  [./stress_beam2]
    type = ComputeFiniteStrainElasticStress
    block = '1 2'
  [../]
  [./strain_15]
    type = ComputeFiniteStrain
    block = '1 2'
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./density]
    type = GenericConstantMaterial
    block = '2'
    prop_names = density
    prop_values =  2.7
  [../]
  [./elasticity_beam_rigid]
    type = ComputeElasticityBeam
    youngs_modulus = 1e10
    poissons_ratio = 0.3
    shear_coefficient = 0.85
    block = '3 4 6'
  [../]
  [./stress_beam]
    type = ComputeBeamResultants
    block = '3 4 6'
  [../]
[]

[BCs]
  [./disp_x]
    type = PresetAcceleration
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    function = accel_bottom
    boundary = 'NZ'
  [../]
  [./disp_y]
    type = DirichletBC
    boundary = 'NZ'
    variable = disp_y
    value = 0.0
  [../]
  [./disp_z]
    type = DirichletBC
    boundary = 'NZ'
    variable = disp_z
    value = 0.0
  [../]
  [./Periodic]
    [./x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'NX'
      secondary = 'PX'
      translation = '75.0 0.0 0.0'
    [../]
    [./y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'NY'
      secondary = 'PY'
      translation = '0.0 75.0 0.0'
    [../]
  [../]
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Ormsby_USE1.csv
    format = 'columns'
    scale_factor = 1
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  nl_abs_tol = 1e-7
  nl_rel_tol = 1e-7
  l_tol = 1e-7
  l_max_its = 12
  start_time = 0.0
  end_time = 3.0
  dt = 0.001
  timestep_tolerance = 1e-6
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
[]

[VectorPostprocessors]
  [./accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x'
    nodes =  '23935 2331 104 5'
    outputs = out
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
    file_base = SSI_NoDamp_RigidStr
  [../]
  [./out2]
    type = Exodus
    file_base = Exod_SSI_NoDamp_RigidStr
    time_step_interval = 10
  [../]
[]
