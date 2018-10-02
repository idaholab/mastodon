# Test for lead rubber isolator in Axial (compression and tension)

#Loading conditions

# i)  A ramp displacement in shear(y_direction)
# ii) A cyclic displacement loading in axial (x_direction)

[Mesh]
  type = GeneratedMesh
  xmin = 0
  xmax = 1
  nx = 1
  dim = 1
  displacements = 'disp_x disp_y disp_z'
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
[]

[BCs]
  [./fixx0]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./fixy0]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  [../]
  [./fixz0]
    type = DirichletBC
    variable = disp_z
    boundary = left
    value = 0.0
  [../]
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
  [./disp_x_1]
    type = PresetDisplacement
    boundary = right
    function = history_disp_axial
    variable = disp_x
    beta = 0.25
    acceleration = accel_x
    velocity = vel_x
  [../]
  [./disp_y_1]
    type = PresetDisplacement
    boundary = right
    function = history_disp_shear
    variable = disp_y
    beta = 0.25
    acceleration = accel_y
    velocity = vel_y
  [../]
[]

[Functions]
  [./history_disp_axial]
    type = PiecewiseLinear
    data_file = disp_axial_ct.csv
    format = columns
  [../]
  [./history_disp_shear]
    type = PiecewiseLinear
    x = '0.0 10.0'
    y = '0.0 0.3048'
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
  start_time = -0.02
  end_time = 10
  dt = 0.005
  dtmin = 0.001
  timestep_tolerance = 1e-6
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

[Materials]
  [./deformation]
    type = ComputeIsolatorDeformation
    block = 0
    sd_ratio = 0.5
    y_orientation = '0.0 1.0 0.0'
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    velocities = 'vel_x vel_y vel_z'
    accelerations = 'accel_x accel_y accel_z'
  [../]
  [./elasticity]
    type = ComputeLRIsolatorElasticity
    block = 0
    fy = 207160
    alpha = 0.03812
    G_rubber = 0.87e6
    K_rubber = 2e9
    D1 = 0.1397
    D2 = 0.508
    ts = 0.00476
    tr = 0.009525
    n = 16
    tc = 0.0127
    kc = 20
    phi_m = 0.75
    ac = 1
    cd = 0
    gamma = 0.5
    beta = 0.25
    k_steel = 50
    a_steel = 1.41e-5
    rho_lead = 11200
    c_lead = 130
    cavitation = true
    horizontal_stiffness_variation = true
    vertical_stiffness_variation = true
    strength_degradation = true
    buckling_load_variation = true
  [../]
[]

[Postprocessors]
  [./disp_x]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_x
  [../]
  [./vel_x]
    type = NodalVariableValue
    nodeid = 1
    variable = vel_x
  [../]
  [./accel_x]
    type = NodalVariableValue
    nodeid = 1
    variable = accel_x
  [../]
  [./reaction_x]
    type = NodalSum
    variable = reaction_x
    boundary = left
  [../]
  [./disp_y]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_y
  [../]
  [./vel_y]
    type = NodalVariableValue
    nodeid = 1
    variable = vel_y
  [../]
  [./accel_y]
    type = NodalVariableValue
    nodeid = 1
    variable = accel_y
  [../]
  [./reaction_y]
    type = NodalSum
    variable = reaction_y
    boundary = left
  [../]
  [./disp_z]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_z
  [../]
  [./vel_z]
    type = NodalVariableValue
    nodeid = 1
    variable = vel_z
  [../]
  [./accel_z]
    type = NodalVariableValue
    nodeid = 1
    variable = accel_z
  [../]
  [./reaction_z]
    type = NodalSum
    variable = reaction_z
    boundary = left
  [../]
  [./rot_x]
    type = NodalVariableValue
    nodeid = 1
    variable = rot_x
  [../]
  [./reaction_xx]
    type = NodalSum
    variable = reaction_xx
    boundary = left
  [../]
  [./rot_y]
    type = NodalVariableValue
    nodeid = 1
    variable = rot_y
  [../]
  [./reaction_yy]
    type = NodalSum
    variable = reaction_yy
    boundary = left
  [../]
  [./rot_z]
    type = NodalVariableValue
    nodeid = 1
    variable = rot_z
  [../]
  [./reaction_zz]
    type = NodalSum
    variable = reaction_zz
    boundary = left
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
[]
