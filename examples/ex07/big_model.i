[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 100
  ny = 50
  xmin = 0.0
  xmax = 1000.0
  ymin = 0.0
  ymax = 500.0
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
  [./layer]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y'
    block = 0
    stiffness_damping_coefficient = 0.00006366
  [../]
  [./inertia_x1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = 0
    eta = 7.854
  [../]
  [./inertia_y1]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    block = 0
    eta = 7.854
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
  [./layer_id]
     type = UniformLayerAuxKernel
     interfaces = '550.0'
     direction = '0 1 0'
     variable = layer
     execute_on = initial
  [../]
[]

[DiracKernels]
  [./SeismicSource]
   [./one_source]
    displacements = 'disp_x disp_y'
    slip = y_force
    strike = 0.0 # strike angle =0, x aligned with North
    dip = 0.0 # dip angle = 0, angle fault makes with horizontal
    rake = 90.0 # rake angle = pi/2, gives slip direction
    shear_modulus = 1.996e7
    area = 1.0
    number = 501
    position_function = 'x_pos y_pos'
   [../]
  [../]
[]

[BCs]
[./NonReflectingBC]
   [./back]
     displacements = 'disp_x disp_y'
     velocities = 'vel_x vel_y'
     accelerations = 'accel_x accel_y'
     beta = 0.25
     gamma = 0.5
     boundary = 'left right bottom'
     shear_wave_speed = 1000.0
     density = 1700.0
     p_wave_speed=1870.829
   [../]
 [../]
[]

[Functions]
  [./y_force]
    type = PiecewiseLinear
    data_file = 'slip_displacement_6_5_hz.csv'
    format = 'columns'
  [../]
  [./x_pos]
    type = PiecewiseLinear
    data_file = 'x_position.csv'
    format = 'columns'
  [../]
  [./y_pos]
    type = PiecewiseLinear
    data_file = 'y_position.csv'
    format = 'columns'
  [../]
[]

[Materials]
  [./linear]
    type = ComputeIsotropicElasticityTensorSoil
    layer_variable = layer
    block = 0
    layer_ids = '0'
    shear_modulus = '1.7e9'
    density = '1700.0'
    poissons_ratio = '0.3'
  [../]
  [./strain_1]
    type = ComputeSmallStrain
    block = '0'
    displacements = 'disp_x disp_y'
  [../]

  [./stress_1]
    type = ComputeLinearElasticStress
    block = '0'
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimestepSize
  [../]
  [./node_x_50]
    type = PointValue
    point = '500.0 500.0 0.0'
    variable = vel_x
  [../]
  [./node_y_50]
    type = PointValue
    point = '500.0 500.0 0.0'
    variable = vel_y
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  nl_abs_tol = 1e-8
  start_time = 0.0
  end_time = 1.0
  dt = 0.01
  timestep_tolerance = 1e-6
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre     boomeramg      4'
  line_search = 'none'
[]

[Outputs]
  file_base = output/big_model_out
  exodus = true
  csv = true
[]
