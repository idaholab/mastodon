# Test for Damper in Axial (Sinewave loading)

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
[]

[Kernels]
  [./lr_disp_x]
    type = StressDivergenceDamper
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    component = 0
    variable = disp_x
    save_in = reaction_x
  [../]
  [./lr_disp_y]
    type = StressDivergenceDamper
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    component = 1
    variable = disp_y
    save_in = reaction_y
  [../]
  [./lr_disp_z]
    type = StressDivergenceDamper
    block = '0'
    displacements = 'disp_x disp_y disp_z'
    component = 2
    variable = disp_z
    save_in = reaction_z
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

[Materials]
  [./elasticity]
    type = ComputeFVDamperElasticity
    block = 0
    y_orientation = '0.0 1.0 0.0'
    displacements = 'disp_x disp_y disp_z'
    cd = 2226502
    alpha = 0.3
    k =300000000
    gamma = 0.5
    beta = 0.25
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
  [./disp_x_1]
    type = PresetDisplacement
    boundary = right
    function = displacement
    variable = disp_x
    beta = 0.25
    acceleration = accel_x
    velocity = vel_x
  [../]
[]

[Functions]
  [./displacement]
    type = PiecewiseLinear
    data_file = disp_axial.csv
    format = columns
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
  end_time = 2
  dt = 0.0025
  dtmin = 0.00001
  timestep_tolerance = 1e-6
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
[]

[Outputs]
  csv = true
  exodus = true
  time_step_interval = 2
  perf_graph = true
[]
