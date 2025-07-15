[Mesh]
  [./generate]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 400.0
    xmax = 600.0
    ymin = 400.0
    ymax = 500.0
    nx = 20
    ny = 10
  [../]
  [./outer_1]
    type = ParsedAddSidesetGenerator
    input = generate
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
   [./solution_uo_outer_react_x]
     type = SolutionUserObject
     mesh = output/DRM_layer_2d_outer_out.e
     system_variables = resid_x
   [../]
   [./solution_uo_outer_react_y]
     type = SolutionUserObject
     mesh = output/DRM_layer_2d_outer_out.e
     system_variables = resid_y
   [../]
   [./solution_uo_inner_react_x]
     type = SolutionUserObject
     mesh = output/DRM_layer_2d_inner_out.e
     system_variables = resid_x
   [../]
   [./solution_uo_inner_react_y]
     type = SolutionUserObject
     mesh = output/DRM_layer_2d_inner_out.e
     system_variables = resid_y
   [../]
[]

[Functions]
   [./react_x_outer_fn]
     type = SolutionFunction
     solution = solution_uo_outer_react_x
   [../]
   [./react_y_outer_fn]
     type = SolutionFunction
     solution = solution_uo_outer_react_y
   [../]
   [./react_x_inner_fn]
     type = SolutionFunction
     solution = solution_uo_inner_react_x
   [../]
   [./react_y_inner_fn]
     type = SolutionFunction
     solution = solution_uo_inner_react_y
   [../]
[]

[NodalKernels]
  [./xforce_inner_1]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = react_x_inner_fn
    boundary = inner_1
  [../]
  [./xforce_inner_2]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = react_x_inner_fn
    boundary = inner_2
  [../]
  [./xforce_inner_3]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = react_x_inner_fn
    boundary = inner_3
  [../]
  [./yforce_inner_1]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    function = react_y_inner_fn
    boundary = inner_1
  [../]
  [./yforce_inner_2]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    function = react_y_inner_fn
    boundary = inner_2
  [../]
  [./yforce_inner_3]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    function = react_y_inner_fn
    boundary = inner_3
  [../]
  [./xforce_outer_1]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = react_x_outer_fn
    boundary = outer_1
  [../]
  [./xforce_outer_2]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = react_x_outer_fn
    boundary = outer_2
  [../]
  [./xforce_outer_3]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = react_x_outer_fn
    boundary = outer_3
  [../]
  [./yforce_outer_1]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    function = react_y_outer_fn
    boundary = outer_1
  [../]
  [./yforce_outer_2]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    function = react_y_outer_fn
    boundary = outer_2
  [../]
  [./yforce_outer_3]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    function = react_y_outer_fn
    boundary = outer_3
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

[Outputs]
  file_base = output/small_model_out
  exodus = true
  csv = true
[]
