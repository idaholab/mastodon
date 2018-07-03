[Mesh]
  type = FileMesh
  file = '2blockexpnew.e'
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
  [./layer_id1]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./layer_id2]
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
    value = -32
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
  [./layer1]
     type = UniformLayerAuxKernel
     variable = layer_id1
     interfaces = '4'
     direction = '0.0 0.0 1.0'
     block = 1001
     execute_on = initial
  [../]
  [./layer2]
     type = UniformLayerAuxKernel
     variable = layer_id2
     interfaces = '4.1'
     direction = '0.0 0.0 1.0'
     block = 1002
     execute_on = initial
  [../]
[]

[BCs]
[./bottom_x]
  type = PresetBC
  variable = disp_x
  boundary = 20
  value = 0.0
[../]
[./bottom_y]
   type = PresetBC
   variable = disp_y
   boundary = 20
   value = 0.0
[../]
[./bottom_z]
   type = PresetBC
   variable = disp_z
   boundary = 20
   value = 0.0
[../]

[./normalPressure]
    type = Pressure
    boundary = 70
    variable = disp_z
    component = 2
    factor = 4608      #32 psi
[../]
[./top_x]
    type = PresetDisplacement
    boundary = 80
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
     data_file = 'rampload8cyclic_unit_new.csv'
     format = columns
    [../]
    [./initial_zztop]
    type = ParsedFunction
    value = '150 * -32 * (4.6 - z) - 4608' #normal pressure = 32 psi
    [../]
    [./initial_ytop]
    type = ParsedFunction
    value = '(150 * -32 * (4.6 - z) - 4608) * 0.15/0.85'
    [../]
    [./initial_zzmid]
    type = ParsedFunction
    value = '-2400 - 90.5 * 32 * (4.1 - z) - 4608'
    [../]
    [./initial_ymid]
    type = ParsedFunction
    value = '(-2400 - 90.5 * 32 * (4.1 - z) - 4608) * 0.45/0.55'
    [../]
    [./initial_zzbot]
    type = ParsedFunction
    value = '-2690 - 90.5 * 32 * (4.0 - z) - 4608'
    [../]
    [./initial_ybot]
    type = ParsedFunction
    value = '(-2690 - 90.5 * 32 * (4.0 - z) - 4608) * 0.3/0.7'
    [../]
[]

[Materials]
  [./elasticity_tensor_top]
    youngs_modulus = 6.33e8 #lbf/ft^2 / psf
    poissons_ratio = 0.15
    type = ComputeIsotropicElasticityTensor
    block = 1003
  [../]
  [./strain_top]
    #Computes the strain, assuming small strains
    type = ComputeIncrementalSmallStrain
    block = 1003
    displacements = 'disp_x disp_y disp_z'
    eigenstrain_names = 'ini_stress'
  [../]
  [./strain_from_initial_stress_top]
    type = ComputeEigenstrainFromInitialStress
    initial_stress = '0 0 0 0 0 0 0 0 initial_zztop'
    eigenstrain_name = ini_stress
  [../]
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
    prop_values = 150.0 #lb/ft3
  [../]
  [./I_Soil]
    [./NL_Soil]
      layer_variable = layer_id1
      layer_ids = '0'
      soil_type = 'user_defined'
      backbone_curve_files = 'NLSoil_32psi.csv'
      poissons_ratio = '0.3'
      block = 1001
      initial_soil_stress = 'initial_ybot 0 0  0 initial_ybot 0  0 0 initial_zzbot'
      density = '90.5'
      #initial_bulk_modulus = '695000'
      initial_shear_modulus = '2.517e6'
      pressure_dependency = false
      #b_exp = 0.0
      p_ref = 4608
      #tension_pressure_cut_off = -1
      a0 = 1
      a1 = 0
      a2 = 0
    [../]

    [./thinlayer]
      layer_variable = layer_id2
      layer_ids = '0'
      soil_type = 'user_defined'
      backbone_curve_files = 'thinlayer_32psi.csv'
      poissons_ratio = '0.15'
      block = 1002
      initial_soil_stress = 'initial_ymid 0 0  0 initial_ymid 0  0 0 initial_zzmid'
      density = '150'
      initial_shear_modulus = '2.75e8'
      pressure_dependency = false
      #b_exp = 0.0
      p_ref = 2420
      #tension_pressure_cut_off = -1
      a0 = 1
      a1 = 0
      a2 = 0
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
    start_time = 0.9
    end_time = 1
    # enable_objects = ''
    disable_objects = '*/inertia_x */inertia_y */inertia_z */vel_x */vel_y */vel_z */accel_x */accel_y */accel_z'
    execute_on = 'initial timestep_begin'
    set_sync_times = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-2
  nl_rel_tol = 1e-4
  l_tol = 1e-2
  l_max_its = 20
  start_time = 0.9
  end_time = 2.5
  dt = 0.01
  timestep_tolerance = 1e-2
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
  [./strain_zx_interface]
  type = ElementAverageValue
  block = '1002'
  variable = strain_zx
  [../]
  [./stress_zx_interface]
  type = ElementAverageValue
  block = '1002'
  variable = stress_zx
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  # file_base = outcyclin
  print_perf_log = true
  print_linear_residuals = false
  [./screen]
    type = Console
    max_rows = 1
  [../]
[]
