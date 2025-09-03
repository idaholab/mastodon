[Mesh]
  type = FileMesh
  file = STR_2D_3.e
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
  [./rot_x]
    block = '2 3'
  [../]
  [./rot_y]
    block = '2 3'
  [../]
  [./rot_z]
    block = '2 3'
  [../]
[]

[AuxVariables]
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
    block = '2 3'
  [../]
  [./rot_vel_y]
    block = '2 3'
  [../]
  [./rot_vel_z]
    block = '2 3'
  [../]
  [./rot_accel_x]
    block = '2 3'
  [../]
  [./rot_accel_y]
    block = '2 3'
  [../]
  [./rot_accel_z]
    block = '2 3'
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

    [./block_2]
      block = 2
      area = 130.06
      Iy = 24166.729
      Iz = 24166.729
      y_orientation = '0.0 1.0 0.0'

      nodal_mass = 0.1
      boundary = 1
    [../]
  [./block_3]
    block = 3

    nodal_mass = 166.0
    area = 500
    Iy = 1e3
    Iz = 1e3
    y_orientation = '1.0 0.0 0.0'
    density = 2.7

    boundary = 1000
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y'
    block = '1'
  [../]
  [./inertia_x1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = '1'
  [../]
  [./inertia_y1]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    block = '1'
  [../]
  [./inertia_z1]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    block = '1'
  [../]
  [./gravity]
    type = Gravity
    variable = disp_y
    value = -9.81
    block = '1'
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
    block = '1'
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = 'timestep_end'
    block = '1'
  [../]
  [./vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = 'timestep_end'
    block = '1'
  [../]
[]

[Materials]
  [./elasticity_base]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1e11
    poissons_ratio = 0
    block = '1'
  [../]
  [./stress_beam2]
    type = ComputeFiniteStrainElasticStress
    block = '1'
  [../]
  [./strain_15]
    type = ComputeFiniteStrain
    block = '1'
    displacements = 'disp_x disp_y'
  [../]
  [./density]
    type = GenericConstantMaterial
    block = '1'
    prop_names = density
    prop_values =  2.7
  [../]
  [./elasticity_beam_rigid]
    type = ComputeElasticityBeam
    youngs_modulus = 1e10
    poissons_ratio = 0.3
    shear_coefficient = 0.85
    block = '2 3'
  [../]
  [./stress_beam]
    type = ComputeBeamResultants
    block = '2 3'
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
    boundary = 'NY_ns'
  [../]
  [./disp_y]
    type = DirichletBC
    boundary = 'NY'
    variable = disp_y
    value = 0.0
  [../]
  [./disp_z]
    type = DirichletBC
    boundary = 'NY'
    variable = disp_z
    value = 0.0
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
  solve_type = 'NEWTON'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  start_time = 0.0
  end_time = 4.0
  dt = 0.001
  dtmin = 0.0001
  nl_abs_tol = 1e-3
  nl_rel_tol = 1e-9
  l_tol = 1e-9
  l_max_its = 50
  timestep_tolerance = 1e-8
[]

[VectorPostprocessors]
  [./accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x'
    nodes = '79 109'
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
  file_base = Str_Final_NoDamp
  [../]
  [./out2]
    type = Exodus
    file_base = Exod_Str_Final_NoDamp
    time_step_interval = 10
  [../]
[]
