# Test for Friction Pendulum isolator under seismic loading

# coefficient of friction depends on velocity, pressure and temperature

#Loading conditions

# i) Seismic loading(ground motion) as acceleration profile at bottom node in 3 directions

[Mesh]
  type = GeneratedMesh
  xmin = 0
  xmax = 0.224
  nx =  1
  dim = 1
  displacements = 'disp_x disp_y disp_z'
[]

[Controls]
  [./C1]
    type = TimePeriod
    disable_objects = '*::x_inertial1 *::y_inertial1 *::z_inertial1 *::vel_x *::vel_y *::vel_z *::accel_x *::accel_y *::accel_z'
    start_time = '0'
    end_time = '0.10'
  [../]
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./vel_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./vel_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./vel_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./accel_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./accel_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./accel_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_vel_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_vel_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_vel_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_accel_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_accel_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_accel_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./reaction_x]
  [../]
  [./reaction_y]
  [../]
  [./reaction_z]
  [../]
  [./reaction_xx]
  [../]
  [./reaction_yy]
  [../]
  [./reaction_zz]
  [../]
[]


[Kernels]
  [./lr_disp_x]
    type = StressDivergenceIsolator
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 0
    variable = disp_x
    save_in = reaction_x
  [../]
  [./lr_disp_y]
    type = StressDivergenceIsolator
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 1
    variable = disp_y
    save_in = reaction_y
  [../]
  [./lr_disp_z]
    type = StressDivergenceIsolator
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 2
    variable = disp_z
    save_in = reaction_z
  [../]
  [./lr_rot_x]
    type = StressDivergenceIsolator
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 3
    variable = rot_x
    save_in = reaction_xx
  [../]
  [./lr_rot_y]
    type = StressDivergenceIsolator
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 4
    variable = rot_y
    save_in = reaction_yy
  [../]
  [./lr_rot_z]
    type = StressDivergenceIsolator
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 5
    variable = rot_z
    save_in = reaction_zz
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
  [./rot_accel_x]
    type = NewmarkAccelAux
    variable = rot_accel_x
    displacement = rot_x
    velocity = rot_vel_x
    beta = 0.25
    execute_on = timestep_end
  [../]
  [./rot_vel_x]
    type = NewmarkVelAux
    variable = rot_vel_x
    acceleration = rot_accel_x
    gamma = 0.5
    execute_on = timestep_end
  [../]
  [./rot_accel_y]
    type = NewmarkAccelAux
    variable = rot_accel_y
    displacement = rot_y
    velocity = rot_vel_y
    beta = 0.25
    execute_on = timestep_end
  [../]
  [./rot_vel_y]
    type = NewmarkVelAux
    variable = rot_vel_y
    acceleration = rot_accel_y
    gamma = 0.5
    execute_on = timestep_end
  [../]
  [./rot_accel_z]
    type = NewmarkAccelAux
    variable = rot_accel_z
    displacement = rot_z
    velocity = rot_vel_z
    beta = 0.25
    execute_on = timestep_end
  [../]
  [./rot_vel_z]
    type = NewmarkVelAux
    variable = rot_vel_z
    acceleration = rot_accel_z
    gamma = 0.5
    execute_on = timestep_end
  [../]
[]

[BCs]
  [./fixrx0]
    type = DirichletBC
    variable = rot_x
    boundary = left
    value = 0.0
  [../]
  [./fixry0]
    type = DirichletBC
    variable = rot_y
    boundary = left
    value = 0.0
  [../]
  [./fixrz0]
    type = DirichletBC
    variable = rot_z
    boundary = left
    value = 0.0
  [../]
  [./fixrx1]
    type = DirichletBC
    variable = rot_x
    boundary = right
    value = 0.0
  [../]
  [./fixry1]
    type = DirichletBC
    variable = rot_y
    boundary = right
    value = 0.0
  [../]
  [./fixrz1]
    type = DirichletBC
    variable = rot_z
    boundary = right
    value = 0.0
  [../]
  [./accel_x0]
    type = PresetAcceleration
    boundary = 'left'
    function = acceleration_x
    variable = disp_x
    beta = 0.25
    acceleration = 'accel_x'
    velocity = 'vel_x'
  [../]
  [./accel_y0]
    type = PresetAcceleration
    boundary = 'left'
    function = acceleration_y
    variable = disp_y
    beta = 0.25
    acceleration = 'accel_y'
    velocity = 'vel_y'
  [../]
  [./accel_z0]
    type = PresetAcceleration
    boundary = 'left'
    function = acceleration_z
    variable = disp_z
    beta = 0.25
    acceleration = 'accel_z'
    velocity = 'vel_z'
  [../]
[]

[NodalKernels]
  [./x_inertial1]
    type = NodalTranslationalInertia
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    boundary = right
    beta = 0.25
    gamma = 0.5
    mass = 640745
    alpha =0
    eta =0
  [../]
  [./y_inertial1]
    type = NodalTranslationalInertia
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    boundary = right
    beta = 0.25
    gamma = 0.5
    mass = 640745
    alpha =0
    eta =0
  [../]
  [./z_inertial1]
    type = NodalTranslationalInertia
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    boundary = right
    beta = 0.25
    gamma = 0.5
    mass = 640745
    alpha =0
    eta =0
  [../]
  [./force_x]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    boundary = 'right'
    function = force_x
  [../]
[]

[Functions]
  [./acceleration_x]
    type = PiecewiseLinear
    data_file = accel_x.csv
    format=columns
  [../]
  [./acceleration_y]
    type = PiecewiseLinear
    data_file = accel_y.csv
    format=columns
  [../]
  [./acceleration_z]
    type = PiecewiseLinear
    data_file = accel_z.csv
    format=columns
  [../]
  [./force_x]
    type = PiecewiseLinear
    x='0.0 0.05 50.11'
    y='0.0 -6285710 -6285710'
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
  line_search = none
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-8
  start_time = 0
  end_time =0.15
  dt = 0.001
  dtmin = 0.0000001
  timestep_tolerance = 1e-8
[]

[Materials]
  [./deformation]
    type = ComputeIsolatorDeformation
    block = '0'
    sd_ratio = 0.5
    y_orientation = '0.0 1.0 0.0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    velocities = 'vel_x vel_y vel_z'
  [../]
  [./elasticity]
    type = ComputeFPIsolatorElasticity
    block = '0'
    mu_ref = 0.06
    p_ref = 50e6
    diffusivity = 4.4e-6
    conductivity = 18
    a = 100
    r_eff = 2.2352
    r_contact = 0.2
    uy = 0.001
    unit = 1
    gamma = 0.5
    beta = 0.25
    pressure_dependent = true
    temperature_dependent = true
    velocity_dependent = true
  [../]
[]

[Postprocessors]
  [./disp_x0]
    type = NodalVariableValue
    nodeid = 0
    variable = disp_x
  [../]
  [./vel_x0]
    type = NodalVariableValue
    nodeid = 0
    variable = vel_x
  [../]
  [./accel_x0]
    type = NodalVariableValue
    nodeid = 0
    variable = accel_x
  [../]
  [./disp_x1]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_x
  [../]
  [./vel_x1]
    type = NodalVariableValue
    nodeid = 1
    variable = vel_x
  [../]
  [./accel_x1]
    type = NodalVariableValue
    nodeid = 1
    variable = accel_x
  [../]
  [./reaction_x]
    type = NodalSum
    variable = reaction_x
    boundary = left
  [../]
  [./disp_y0]
    type = NodalVariableValue
    nodeid = 0
    variable = disp_y
  [../]
  [./vel_y0]
    type = NodalVariableValue
    nodeid = 0
    variable = vel_y
  [../]
  [./accel_y0]
    type = NodalVariableValue
    nodeid = 0
    variable = accel_y
  [../]
  [./disp_y1]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_y
  [../]
  [./vel_y1]
    type = NodalVariableValue
    nodeid = 1
    variable = vel_y
  [../]
  [./accel_y1]
    type = NodalVariableValue
    nodeid = 1
    variable = accel_y
  [../]
  [./reaction_y]
    type = NodalSum
    variable = reaction_y
    boundary = left
  [../]
  [./disp_z0]
    type = NodalVariableValue
    nodeid = 0
    variable = disp_z
  [../]
  [./vel_z0]
    type = NodalVariableValue
    nodeid = 0
    variable = vel_z
  [../]
  [./accel_z0]
    type = NodalVariableValue
    nodeid = 0
    variable = accel_z
  [../]
  [./disp_z1]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_z
  [../]
  [./vel_z1]
    type = NodalVariableValue
    nodeid = 1
    variable = vel_z
  [../]
  [./accel_z1]
    type = NodalVariableValue
    nodeid = 1
    variable = accel_z
  [../]
  [./reaction_z]
    type = NodalSum
    variable = reaction_z
    boundary = left
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  time_step_interval = 1
[]
