[Mesh]
  [./generate]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 450.0
    xmax = 550.0
    ymin = 450.0
    ymax = 500.0
    nx = 10
    ny = 5
  [../]
  [./SubdomainBoundingBoxGenerator]
    type = SubdomainBoundingBoxGenerator
    input = generate
    block_id = 1
    bottom_left = '460.0 460.0 0.0'
    top_right = '540.0 500.0 0.0'
  [../]
  [./ed0]
    type = BlockDeletionGenerator
    input = SubdomainBoundingBoxGenerator
    block_id = 1
    depends_on = SubdomainBoundingBoxGenerator
  [../]
  [./outer_1]
    type = ParsedAddSidesetGenerator
    input = ed0
    combinatorial_geometry = 'x > 449.9 & x < 450.1 & y > 449.9'
    new_sideset_name = outer_1
    normal = '-1.0 0.0 0.0'
  [../]
  [./outer_2]
    type = ParsedAddSidesetGenerator
    input = outer_1
    combinatorial_geometry = 'x > 459.1 & x < 541.0 & y > 449.9 & y < 450.1'
    new_sideset_name = outer_2
    normal = '0.0 -1.0 0.0'
  [../]
  [./outer_3]
    type = ParsedAddSidesetGenerator
    input = outer_2
    combinatorial_geometry = 'x > 549.1 & x < 550.1 & y > 449.9'
    new_sideset_name = outer_3
    normal = '1.0 0.0 0.0'
  [../]
  [./inner_1]
    type = ParsedAddSidesetGenerator
    input = outer_3
    combinatorial_geometry = 'x > 459.9 & x < 460.1 & y > 459.9'
    new_sideset_name = inner_1
    normal = '1.0 0.0 0.0'
  [../]
  [./inner_2]
    type = ParsedAddSidesetGenerator
    input = inner_1
    combinatorial_geometry = 'x > 469.1 & x < 530.1 & y > 459.9 & y < 460.1'
    new_sideset_name = inner_2
    normal = '0.0 1.0 0.0'
  [../]
  [./inner_3]
    type = ParsedAddSidesetGenerator
    input = inner_2
    combinatorial_geometry = 'x > 539.9 & x < 540.1 & y > 459.9'
    new_sideset_name = inner_3
    normal = '-1.0 0.0 0.0'
  [../]
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
  [./resid_x]
  [../]
  [./resid_y]
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y'
    block = 0
    save_in = 'resid_x resid_y'
    stiffness_damping_coefficient = 0.00006366
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    save_in = resid_x
    block = 0
    eta = 7.854
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    save_in = resid_y
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

[UserObjects]
   [./solution_disp_x]
     type = SolutionUserObject
     mesh = output/big_model_out.e
     system_variables = disp_x
   [../]
   [./solution_disp_y]
     type = SolutionUserObject
     mesh = output/big_model_out.e
     system_variables = disp_y
   [../]
[]

[Functions]
   [./soln_dispx_fn]
     type = SolutionFunction
     solution = solution_disp_x
     scale_factor = -1.0
   [../]
   [./soln_dispy_fn]
     type = SolutionFunction
     solution = solution_disp_y
     scale_factor = -1.0
   [../]
[]

[BCs]
  [./disp_outer_1x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = outer_1
    function = soln_dispx_fn
  [../]
  [./disp_outer_2x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = outer_2
    function = soln_dispx_fn
  [../]
  [./disp_outer_3x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = outer_3
    function = soln_dispx_fn
  [../]
  [./disp_inner_1x]
    type = DirichletBC
    variable = disp_x
    boundary = inner_1
    value = 0.0
  [../]
  [./disp_inner_2x]
    type = DirichletBC
    variable = disp_x
    boundary = inner_2
    value = 0.0
  [../]
  [./disp_inner_3x]
    type = DirichletBC
    variable = disp_x
    boundary = inner_3
    value = 0.0
  [../]
  [./disp_outer_1y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = outer_1
    function = soln_dispy_fn
  [../]
  [./disp_outer_2y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = outer_2
    function = soln_dispy_fn
  [../]
  [./disp_outer_3y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = outer_3
    function = soln_dispy_fn
  [../]
  [./disp_inner_1y]
    type = DirichletBC
    variable = disp_y
    boundary = inner_1
    value = 0.0
  [../]
  [./disp_inner_2y]
    type = DirichletBC
    variable = disp_y
    boundary = inner_2
    value = 0.0
  [../]
  [./disp_inner_3y]
    type = DirichletBC
    variable = disp_y
    boundary = inner_3
    value = 0.0
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

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  nl_abs_tol = 1e-8
  start_time = 0.0
  end_time = 1.0
  dt = 0.01
  timestep_tolerance = 1e-6
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre     boomeramg      4'
  line_search = 'none'
[]

[Postprocessors]
  [./dt]
    type = TimestepSize
  [../]
[]

[Outputs]
  file_base = output/DRM_layer_2d_inner_out
  exodus = true
  csv = true
[]
