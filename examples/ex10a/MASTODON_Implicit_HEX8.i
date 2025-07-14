[Mesh]
  [generate]
    type = GeneratedMeshGenerator
    nx = 1
    ny = 1
    nz = 10
    xmin = -1
    ymin = -1
    zmin = 0.0
    xmax = 1
    ymax = 1
    zmax = 20.0
    dim = 3
    elem_type = HEX8
  []
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
  [./vel_y]
  [../]
  [./vel_z]
  [../]
  [./accel_x]
  [../]
  [./accel_y]
  [../]
  [./accel_z]
  [../]
  [./stress_zz]
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
    stiffness_damping_coefficient = 0
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
 [./stress_zz]
   type = RankTwoAux
   rank_two_tensor = stress
   variable = stress_zz
   index_i = 2
   index_j = 2
 [../]
[]

[Materials]
  [./elasticity_tensor_block]
    youngs_modulus = 119751.528    # kip/ft^2
    poissons_ratio = 0.3
    type = ComputeIsotropicElasticityTensor
  [../]
  [./den_block]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 0.00367 # kip s^2/ft^4
  [../]
  [./strain_1]
    type = ComputeSmallStrain
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./stress_1]
    type = ComputeLinearElasticStress
  [../]
[]

[BCs]
  [./press]
    type = Pressure
    boundary = 'front'
    variable = disp_z
    component = 2
    factor = 1  # kip/ft^2
  [../]
  [./disp_x]
    type = DirichletBC
    displacements = 'disp_x disp_y disp_z'
    variable = disp_x
    boundary = 'back'
    value = 0
  [../]
  [./disp_y]
    type = DirichletBC
    displacements = 'disp_x disp_y disp_z'
    variable = disp_y
    boundary = 'back'
    value = 0
  [../]
  [./disp_z]
    type = DirichletBC
    displacements = 'disp_x disp_y disp_z'
    variable = disp_z
    boundary = 'back'
    value = 0
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
  solve_type = 'LINEAR'
#  petsc_options = '-snes_ksp_ew'
#  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
#  petsc_options_value = '201                hypre    boomeramg      4'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu     superlu_dist'
  end_time = 2e-2
  dt = 1e-5
  nl_abs_tol = 1e-06
  nl_rel_tol = 1e-06
  l_tol = 1e-06
  l_max_its = 20
  timestep_tolerance = 1e-06
[]

[Postprocessors]
  [./dispx_20]
    type = PointValue
    variable = disp_x
    point = '0.0 0.0 20'
  [../]
  [./dispy_20]
    type = PointValue
    variable = disp_y
    point = '0.0 0.0 20'
  [../]
  [./dispz_20]
    type = PointValue
    variable = disp_z
    point = '0.0 0.0 20'
  [../]
   [./stress_zz_5]
    type = PointValue
    variable = stress_zz
    point = '0.0 0.0 5'
  [../]
   [./stress_zz_10]
    type = PointValue
    variable = stress_zz
    point = '0.0 0.0 10'
  [../]
   [./stress_zz_15]
    type = PointValue
    variable = stress_zz
    point = '0.0 0.0 15'
  [../]
   [./stress_zz_20]
    type = PointValue
    variable = stress_zz
    point = '0.0 0.0 20'
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  [./console]
    type = Console
    max_rows = 1
  [../]
[]
