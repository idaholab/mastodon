[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 20
  xmin = -0.5
  ymin = -0.5
  zmin = 0.0
  xmax = 0.5
  ymax = 0.5
  zmax = 20.0
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
    zeta = 0.000781
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    eta = 0.64026
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    eta = 0.64026
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    eta = 0.64026
  [../]
  [./gravity]
    type = Gravity
    variable = disp_z
    value = -9.81
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
     interfaces = '1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20.2'
     direction = '0.0 0.0 1.0'
     execute_on = initial
  [../]
[]

[BCs]
  [./bottom_z]
    type = PresetBC
    variable = disp_z
    boundary = 'back'
    value = 0.0
  [../]
  [./bottom_y]
    type = PresetBC
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
     data_file = chichi_bc_mpss_xp20.csv
     format = 'columns'
  [../]
  [./initial_zz]
    type = ParsedFunction
    value = '-2000.0 * 9.81 * (20.0 - z)'
  [../]
  [./initial_xx]
    type = ParsedFunction
    value = '-2000.0 * 9.81 * (20.0 - z) * 0.3/0.7'
  [../]
[]

[Materials]
  [./I_Soil]
    [./soil_all]
       block = 0
       layer_variable = layer_id
       layer_ids = '0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19'
       initial_soil_stress = 'initial_xx 0 0 0 initial_xx 0 0 0 initial_zz'
       poissons_ratio = '0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3'
       soil_type = 'gqh'
       number_of_points = 100
       ### GQ/H ####
       initial_shear_modulus = '125000000 118098000 111392000 103968000 96800000 89888000 83232000 76832000 70688000 64800000 59168000 53792000 48672000 43808000 39200000 34848000 30752000 26912000 23328000 20000000'
       theta_1 = '-6.66 -6.86 -7.06 -7.35 -7.65 -7.95 -8.3 -8.61 -8.95 -9.3 -9.61 -9.92 -10.0 -10.0 -10.0 -10.0 -10.0 -9.31 -7.17 -5.54'
       theta_2 = '5.5 5.7 5.9 6.2 6.6 6.9 7.3 7.6 8.0 8.4 8.6 8.8 8.82 8.71 8.55 8.3 7.88 6.4 2.4 -2.28'
       theta_3 = '1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0'
       theta_4 = '1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0'
       theta_5 = '0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99 0.99'
       taumax = '292500 277500 262500 247500 232500 217500 202500 187500 172500 157500 142500 127500 112500 97500 82500 67500 52500 37500 22500 7500'
       p_ref = '236841 224695 212550 200404 188258 176112 163967 151821 139675 127530 115384 103238 91092 78947 66801 54655 42510 30364 18218 6072'
       ######
       density = '2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0 2000.0'
       a0 = 1.0
       a1 = 0.0
       a2 = 0.0
       b_exp = 0.0
    [../]
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
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-6
  l_tol = 1e-6
  l_max_its = 50
  start_time = 0
  end_time = 85.4
  dt = 0.001
  timestep_tolerance = 1e-6
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  line_search = 'none'
[]

[Postprocessors]
  [./accelx_top]
    type = PointValue
    point = '0.0 0.0 20.0'
    variable = accel_x
  [../]
  [./accelx_top1]
    type = PointValue
    point = '0.0 0.0 19.0'
    variable = accel_x
  [../]
  [./accelx_mid]
    type = PointValue
    point = '0.0 0.0 10.0'
    variable = accel_x
  [../]
  [./accelx_bot]
    type = PointValue
    point = '0.0 0.0 0.0'
    variable = accel_x
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = false
  [./screen]
    type = Console
    max_rows = 1
    interval = 1000
  [../]
[]
