[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 2
  xmin = -0.5
  ymin = -0.5
  zmin = 0.0
  xmax = 0.5
  ymax = 0.5
  zmax = 2.0
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
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    stiffness_damping_coefficient = 0.000781
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
    interfaces = '1.0 2.02'
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
  [./bottom_disp]
    type = PresetDisplacement
    boundary = 'back'
    variable = disp_x
    beta = 0.25
    velocity = vel_x
    acceleration = accel_x
    function = disp_bottom
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
  [./disp_bottom]
    type = PiecewiseLinear
    data_file = Displacement2.csv
    format = columns
  [../]
  [./initial_zz]
    type = ParsedFunction
    expression = '-2000.0 * 9.81 * (2.0 - z)'
  [../]
  [./initial_xx]
    type = ParsedFunction
    expression = '-2000.0 * 9.81 * (2.0 - z) * 0.3/0.7'
  [../]
[]

[Materials]
  [./I_Soil]
    [./soil_all]
       block = 0
       layer_variable = layer_id
       layer_ids = '0 1'
       initial_soil_stress = 'initial_xx 0 0 0 initial_xx 0 0 0 initial_zz'
       poissons_ratio = '0.3 0.3'
       soil_type = 'gqh'
       number_of_points = 100
       ### GQ/H ####
       initial_shear_modulus = '125000000 118098000'
       theta_1 = '-6.66 -6.86'
       theta_2 = '5.5 5.7'
       theta_3 = '1.0 1.0'
       theta_4 = '1.0 1.0'
       theta_5 = '0.99 0.99'
       taumax = '292500 277500'
       p_ref = '236841 224695'
       density = '2000.0 2000.0'
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
  nl_abs_tol = 1e-11
  nl_rel_tol = 1e-11
  start_time = 0
  end_time = 8
  dt = 0.05
  timestep_tolerance = 1e-8
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  line_search = 'none'
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_6x]
    type = NodalVariableValue
    nodeid = 10
    variable = disp_x
  [../]
  [./disp_6y]
    type = NodalVariableValue
    nodeid = 10
    variable = disp_y
  [../]
  [./disp_6z]
    type = NodalVariableValue
    nodeid = 10
    variable = disp_z
  [../]
  [./vel_6x]
    type = NodalVariableValue
    nodeid = 10
    variable = vel_x
  [../]
  [./vel_6y]
    type = NodalVariableValue
    nodeid = 10
    variable = vel_y
  [../]
  [./vel_6z]
    type = NodalVariableValue
    nodeid = 10
    variable = vel_z
  [../]
  [./accel_6x]
    type = NodalVariableValue
    nodeid = 10
    variable = accel_x
  [../]
  [./accel_6y]
    type = NodalVariableValue
    nodeid = 10
    variable = accel_y
  [../]
  [./accel_6z]
    type = NodalVariableValue
    nodeid = 10
    variable = accel_z
  [../]
  [./stress_xy_el]
    type = ElementalVariableValue
    variable = stress_xy
    elementid = 1
  [../]
  [./stress_yz_el]
    type = ElementalVariableValue
    variable = stress_yz
    elementid = 1
  [../]
  [./stress_zx_el]
    type = ElementalVariableValue
    variable = stress_zx
    elementid = 1
  [../]
  [./strain_xy_el]
    type = ElementalVariableValue
    variable = strain_xy
    elementid = 1
  [../]
  [./strain_yz_el]
    type = ElementalVariableValue
    variable = strain_yz
    elementid = 1
  [../]
  [./strain_zx_el]
    type = ElementalVariableValue
    variable = strain_zx
    elementid = 1
  [../]
  [./gamma_zx_el] # engineering shear strain
    type = ScalePostprocessor
    value = strain_zx_el
    scaling_factor = 2
  [../]
  [./stress_xx_el]
    type = ElementalVariableValue
    variable = stress_xx
    elementid = 1
  [../]
  [./stress_yy_el]
    type = ElementalVariableValue
    variable = stress_yy
    elementid = 1
  [../]
  [./stress_zz_el]
    type = ElementalVariableValue
    variable = stress_zz
    elementid = 1
  [../]
  [./strain_xx_el]
    type = ElementalVariableValue
    variable = strain_xx
    elementid = 1
  [../]
  [./strain_yy_el]
    type = ElementalVariableValue
    variable = strain_yy
    elementid = 1
  [../]
  [./strain_zz_el]
    type = ElementalVariableValue
    variable = strain_zz
    elementid = 1
  [../]
[]

[Outputs]
  exodus = true
  csv = true
[]
