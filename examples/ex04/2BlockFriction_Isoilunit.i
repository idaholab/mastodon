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
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
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
[]

[Kernels]
  [./TensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./gravity]
    type = Gravity
    variable = disp_z
    value = -9.81
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    eta=0.0
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    eta=0.0
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    eta = 0.0
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
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
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
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
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
 type = DirichletBC
  variable = disp_x
  boundary = 109
  value = 0.0
[../]
[./bottom_y]
   type = DirichletBC
   variable = disp_y
   boundary = 109
   value = 0.0
[../]
[./bottom_z]
   type = DirichletBC
   variable = disp_z
   boundary = 109
   value = 0.0
[../]
[./Periodic]
  [./x_dir1]
    variable = 'disp_x disp_y disp_z'
    primary = '101'
    secondary = '103'
    translation = '10.0 0.0 0.0'
  [../]
  [./y_dir1]
    variable = 'disp_x disp_y disp_z'
    primary = '102'
    secondary = '104'
    translation = '0.0 10.0 0.0'
  [../]
  [./x_dir2]
    variable = 'disp_x disp_y disp_z'
    primary = '105'
    secondary = '107'
    translation = '10.0 0.0 0.0'
  [../]
  [./y_dir2]
    variable = 'disp_x disp_y disp_z'
    primary = '106'
    secondary = '108'
    translation = '0.0 10.0 0.0'
  [../]
[../]
  [./X-dir]
      type = FunctionDirichletBC
      boundary = 110
      variable = disp_x
      function = loading_bc
  [../]
[]

[Functions]
[./loading_bc]
   type = PiecewiseLinear
   data_file = 'rampload8_unit.csv'
   format = columns
[../]
[./initial_zztop]
  type = ParsedFunction
  expression = '8792.0 * -9.81 * (21.0 - z)'
[../]
[./initial_ytop]
  type = ParsedFunction
  expression = '8792.0 * -9.81 * (21.0 - z) * 0.2/0.8'
[../]
[./initial_zzmid]
  type = ParsedFunction
  expression = '-862495.2 - 2500.0 * -9.81 * (11.0 - z)'
[../]
[./initial_ymid]
  type = ParsedFunction
  expression = '(-862495.2 - 2500.0 * -9.81 * (11.0 - z)) * 0.45/0.55'
[../]
[./initial_zzbot]
  type = ParsedFunction
  expression = '-887020.2 - 2500.0 * -9.81 * (10.0 - z)'
[../]
[./initial_ybot]
  type = ParsedFunction
  expression = '(-887020.2 - 2500.0 * -9.81 * (10.0 - z)) * 0.3/0.7'
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
    eigenstrain_names = ini_stress
  [../]
  [./strain_from_initial_stress]
    type = ComputeEigenstrainFromInitialStress
    initial_stress = '0 0 0 0 0 0 0 0 initial_zztop'
    eigenstrain_name = ini_stress
  [../]
  [./stress_top]
    #Computes the stress, using linear elasticity
    type = ComputeFiniteStrainElasticStress
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
    eigenstrain_names = 'ini_stress_bot'
  [../]
  [./stress_bot]
    #Computes the stress, using linear elasticity
    type = ComputeFiniteStrainElasticStress
    block = 1001
  [../]
  [./strain_from_initial_stress_bot]
    type = ComputeEigenstrainFromInitialStress
    block = 1001
    initial_stress = 'initial_ybot 0 0 0 initial_ybot 0 0 0 initial_zzbot'
    eigenstrain_name = ini_stress_bot
  [../]
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
      initial_soil_stress = 'initial_ymid 0 0  0 initial_ymid 0  0 0 initial_zzmid'
      density = '2500'
      #initial_bulk_modulus = '7.83e10'
      initial_shear_modulus = '2.70588e11'
      #taumax = 275998
  #    theta_1 = '0'
  #    theta_2 = '0'
  #    theta_3 = '0'
  #    theta_4 = '0'
  #    theta_5 = '0'
      pressure_dependency = true
      #b_exp = 0.0
      p_ref = 880500
      #tension_pressure_cut_off = -1
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

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-7
  nl_rel_tol = 1e-7
  l_tol = 1e-6
  l_max_its = 20
  start_time = 0
  end_time = 1.2
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
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = false
  [./screen]
    type = Console
    max_rows = 1
  [../]
[]
