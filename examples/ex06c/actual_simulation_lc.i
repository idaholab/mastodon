[Mesh]
  type = FileMesh
  file = ../ex06c/stabilize_lc_out1_cp/LATEST
  patch_update_strategy = iteration
  patch_size = 40
  partitioner = centroid
  centroid_partitioner_direction = y
[]

[Problem]
  restart_file_base = ../ex06c/stabilize_lc_out1_cp/LATEST
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  volumetric_locking_correction = true
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
  [./accel_y]
  [../]
  [./vel_y]
  [../]
  [./vel_z]
  [../]
  [./accel_z]
  [../]
  [./nor_forc]
    order = FIRST
    family = LAGRANGE
  [../]
  [./tang_forc_x]
    order = FIRST
    family = LAGRANGE
  [../]
  []

[Kernels]
  [./TensorMechanics]
    use_displaced_mesh = true
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./gravity]
    type = Gravity
    variable = disp_z
    value = -386.09   #in/s2
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
  [./nor_forc]
    type = PenetrationAux
    variable = nor_forc
    quantity = normal_force_magnitude
    boundary = 102
    paired_boundary = 103
  [../]
  [./tang_forc_x]
    type = PenetrationAux
    variable = tang_forc_x
    quantity = tangential_force_x
    boundary = 102
    paired_boundary = 103
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
[]

[BCs]
  [./fix_x_soil]
    type = PresetBC
    variable = disp_x
    boundary = 100
    value = 0.0
  [../]
  [./fix_y_soil]
    type = PresetBC
    variable = disp_y
    boundary = 100
    value = 0.0
  [../]
  [./fix_z_soil]
    type = PresetBC
    variable = disp_z
    boundary = 100
    value = 0.0
  [../]
  [./concrete_pressure]
    type = Pressure
    boundary = 101
    variable = disp_z
    component = 2
    factor = 5 #psi
  [../]
[]


[NodalKernels]
  [./force_x]
    type = UserForcingFunctionNodalKernel
    boundary = 1000
    function = force
    variable = disp_x
  [../]
[]

[Functions]
  [./force]
    type = PiecewiseLinear
    x = '0 1 2'
    y= '0  0.1953125 0.390625'  #number of nodes = 512, force 0,100,200
  [../]
[]


[Materials]
  [./elasticity_tensor_block]
    youngs_modulus = 4e6 #psi
    poissons_ratio = 0.25
    type = ComputeIsotropicElasticityTensor
    block = 2
  [../]
  [./strain_block]
    type = ComputeFiniteStrain
    block = 2
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./stress_block]
    type = ComputeFiniteStrainElasticStress
    block = 2
  [../]
  [./den_block]
    type = GenericConstantMaterial
    block = 2
    prop_names = density
    prop_values = 0.0002248 #slug/in^3
  [../]
  [./elasticity_tensor_soil]
    youngs_modulus = 1.3983e+05 #psi
    poissons_ratio = 0.3
    type = ComputeIsotropicElasticityTensor
    block = 1
  [../]
  [./strain_soil]
    type = ComputeFiniteStrain
    block = 1
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./stress_soil]
    type = ComputeFiniteStrainElasticStress
    block = 1
  [../]
  [./den_soil]
    type = GenericConstantMaterial
    block = 1
    prop_names = density
    prop_values = 0.0001356 #slug/in^3
  [../]
[]

[Contact]
  [./leftright]
    slave = 102
    master = 103
    system = constraint
    model = coulomb
    formulation = penalty
    normalize_penalty = true
    friction_coefficient = 0.7
    penalty = 1e5
    displacements = 'disp_x disp_y disp_z'
  [../]
[]

[Preconditioning]
  [./andy]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  [./nor_forc]
    type = NodalSum
    variable = nor_forc
    boundary = 102
  [../]
  [./tang_forc_x]
    type = NodalSum
    variable = tang_forc_x
    boundary = 102
  [../]
  [./dispx]
    type = NodalMaxValue
    block = '2'
    variable = disp_x
  [../]
  [./force2x]
    type = FunctionValuePostprocessor
    function = force
    scale_factor = 512 #Number of nodes
  [../]
[]


[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu     superlu_dist'
  line_search = 'none'
  start_time = 0
  end_time = 1.55
  dt = 0.005
  dtmin = 0.001
  nl_abs_tol = 1e-1
  nl_rel_tol = 1e-4
  l_tol = 1e-2
  l_max_its = 20
  timestep_tolerance = 1e-3
[]




 [Outputs]
   csv = true
   exodus = true
   file_base = results55
   perf_graph = true
   print_linear_residuals = false
   [./screen]
     type = Console
     max_rows = 1
   [../]
 []
