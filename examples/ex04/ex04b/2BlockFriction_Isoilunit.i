[Mesh]
  type = FileMesh
  file = '2blockfriction_isoilunit.e'
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
    [./stress_xy]
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
    [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
    [../]
    [./stress_zx]
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
    [./strain_xy]
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
    [./vel_x]
    # initial_condition = 0.0523
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
[]

[Kernels]
    [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    zeta = 0.00006366
    [../]
    [./gravity]
    type = Gravity
    variable = disp_z
    value = -9.81
    [../]
    [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = 'vel_x'
    acceleration = 'accel_x'
    beta = 0.25
    gamma = 0.5
    eta = 0
    [../]
    [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    eta = 0
    [../]
    [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    eta = 0
    [../]
[]

[AuxKernels]
    [./stress_zx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zx
    index_i = 2
    index_j = 0
    [../]
    [./strain_zx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zx
    index_i = 2
    index_j = 0
    [../]
    [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    [../]
    [./strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
    [../]
    [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
    [../]
    [./strain_yy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_yy
    index_i = 1
    index_j = 1
    [../]
    [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
    [../]
    [./strain_zz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zz
    index_i = 2
    index_j = 2
    [../]
    [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
    [../]
    [./strain_xy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xy
    index_i = 0
    index_j = 1
    [../]
    [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 1
    index_j = 2
    [../]
    [./strain_yz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_yz
    index_i = 1
    index_j = 2
    [../]
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
    [./layers]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '11.0'
    direction = '0.0 0.0 1.0'
    block = 1002
    execute_on = initial
    [../]
[]

[BCs]
    [./bottom_x]
    type = PresetBC
    variable = disp_x
    boundary = '109 111 112 113 114'
    value = 0.0
    [../]
    [./bottom_y]
    type = PresetBC
    variable = disp_y
    boundary = '109 111 112 113 114'
    value = 0.0
    [../]
    [./bottom_z]
    type = PresetBC
    variable = disp_z
    boundary = '109 111 112 113 114'
    value = 0.0
    [../]
    [./top_x]
    type = PresetDisplacement
    boundary = 502
    variable = disp_x
    beta = 0.25
    velocity = vel_x
    acceleration = accel_x
    function = loading_bc
    [../]
[]

[Functions]
    [./loading_bc]
    type = PiecewiseLinear
    data_file = 'Displacement2.csv'
    format = columns
    [../]
    [./initial_zztop]
    type = ParsedFunction
    value = '8792.0 * -9.81 * (21.0 - z)'
    [../]
    [./initial_ytop]
    type = ParsedFunction
    value = '8792.0 * -9.81 * (21.0 - z) * 0.2/0.8'
    [../]
    [./initial_zzmid]
    type = ParsedFunction
    value = '-862495.2 + 2500.0 * -9.81 * (11.0 - z)'
    [../]
    [./initial_ymid]
    type = ParsedFunction
    value = '(-862495.2 + 2500.0 * -9.81 * (11.0 - z)) * 0.45/0.55'
    [../]
    [./initial_zzbot]
    type = ParsedFunction
    value = '-887020.2 + 2500.0 * -9.81 * (10.0 - z)'
    [../]
    [./initial_ybot]
    type = ParsedFunction
    value = '(-887020.2 + 2500.0 * -9.81 * (10.0 - z)) * 0.3/0.7'
    [../]
[]

[Materials]
  [./elasticity_tensor_top]
    youngs_modulus = 6.5e10 #Pa
    poissons_ratio = 0.2
    type = ComputeIsotropicElasticityTensor
    block = 1003
  [../]
  [./strain_top]
    #Computes the strain, assuming small strains
    type = ComputeIncrementalSmallStrain
    block = 1003
    displacements = 'disp_x disp_y disp_z'
    # eigenstrain_names = ini_stress
  [../]
  # [./strain_from_initial_stress]
  #   type = ComputeEigenstrainFromInitialStress
  #   initial_stress = '0 0 0 0 0 0 0 0 initial_zztop'
  #   eigenstrain_name = ini_stress
  # [../]
  [./stress_top]
    #Computes the stress, using linear elasticity
    type = ComputeFiniteStrainElasticStress
    store_stress_old = true
    block = 1003
  [../]
  [./den_top]
    type = GenericConstantMaterial
    block = 1003
    prop_names = density
    prop_values = 8792 #kg/m3
  [../]

  [./elasticity_tensor_bot]
    youngs_modulus = 7e9#Pa
    poissons_ratio = 0.3
    type = ComputeIsotropicElasticityTensor
    block = 1001
  [../]
  [./strain_bot]
    #Computes the strain, assuming small strains
    type = ComputeIncrementalSmallStrain
    block = 1001
    displacements = 'disp_x disp_y disp_z'
    # eigenstrain_names = 'ini_stress_bot'
  [../]
  [./stress_bot]
    #Computes the stress, using linear elasticity
    type = ComputeFiniteStrainElasticStress
    store_stress_old = true
    block = 1001
  [../]
  # [./strain_from_initial_stress_bot]
  #   type = ComputeEigenstrainFromInitialStress
  #   block = 1001
  #   initial_stress = 'initial_ybot 0 0 0 initial_ybot 0 0 0 initial_zzbot'
  #   eigenstrain_name = ini_stress_bot
  # [../]
  [./den_bot]
    type = GenericConstantMaterial
    block = 1001
    prop_names = density
    prop_values = 2500 #kg/m3
  [../]
  [./I_Soil]
    [./soil1]
      layer_variable = layer_id
      layer_ids = '0'
      soil_type = 'user_defined'
      backbone_curve_files = 'backbone_curveunit.csv'
      poissons_ratio = '0.45'
      block = 1002
      # initial_soil_stress = 'initial_ymid 0 0 0 initial_ymid 0 0 0 initial_zzmid'
      density = '8792'
      #initial_bulk_modulus = '7.83e10'
      initial_shear_modulus = '2.71e10'
      pressure_dependency = true
      # b_exp = 0.0
      p_ref = 904918
      # tension_pressure_cut_off = -1
      a0 = 0
      a1 = 0
      a2 = 1
    [../]
  [../]
[]

[Preconditioning]
  [./andy]
    type = SMP
    full = true
  [../]
[]

[Controls]
  [./inertia_switch]
    type = TimePeriod
    start_time = 0
    end_time = 3.0
    disable_objects = '*/inertia_x */inertia_y */inertia_z */vel_x */vel_y */vel_z */accel_x */accel_y */accel_z'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-7
  nl_rel_tol = 1e-7
  # l_tol = 1e-9
  # l_max_its = 20
  start_time =  0#-0.05
  end_time = 8
  dt = 0.01
  timestep_tolerance = 1e-6
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  line_search = 'none'
[]

[Postprocessors]
  [./_dt]
  type = TimestepSize
  [../]
  [./stres_xx_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_xx
  [../]
  [./stres_xy_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_xy
  [../]
  [./stres_yy_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_yy
  [../]
  [./stres_yz_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_yz
  [../]
  [./stres_zz_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_zz
  [../]
  [./stress_zx_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_zx
  [../]
  [./strain_zx_interface]
  type = ElementAverageValue
  block = '1002'
  variable = strain_zx
[../]
  [./accel_top_x]
      type = PointValue
      point = '5 5 21'
      variable = accel_x
    [../]
    [./accel_top_y]
      type = PointValue
      point = '5 5 21'
      variable = accel_y
    [../]
    [./accel_top_z]
      type = PointValue
      point = '5 5 21'
      variable = accel_z
    [../]
    [./disp_top_x]
        type = PointValue
        point = '5 -5 21'
        variable = disp_x
      [../]
      [./disp_top_y]
        type = PointValue
        point = '5 -5 21'
        variable = disp_x
      [../]
      [./disp_top_z]
        type = PointValue
        point = '5 -5 21'
        variable = disp_x
      [../]
[]

[Outputs]
  csv = true
  exodus = true
  print_perf_log = true
  print_linear_residuals = false
  [./screen]
    type = Console
    max_rows = 1
  [../]
[]
