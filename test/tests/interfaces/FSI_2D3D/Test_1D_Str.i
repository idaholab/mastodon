[Mesh]
  # [Mesh]
  # type = MeshGeneratorMesh
  #
  # [./cmg]
  #   type = CartesianMeshGenerator
  #   dim = 1
  #   dx = '100.0 100.0 100.0'
  #   ix = '200 200 200'
  #   subdomain_id = '0 1 2'
  # [../]
  # []
  [gen]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 660
    xmax = 110
  []
  [./subdomain1]
    input = gen
    type = SubdomainBoundingBoxGenerator
    bottom_left = '10.0 0 0'
    block_id = 1
    top_right = '110.0 0.0 0'
  [../]
  # [./subdomain2]
  #   input = subdomain1
  #   type = SubdomainBoundingBoxGenerator
  #   bottom_left = '200.0 0 0'
  #   block_id = 2
  #   top_right = '300.0 0.0 0'
  # [../]
  # [./break_boundary1]
  #   input = subdomain1
  #   type = BreakBoundaryOnSubdomainGenerator
  # [../]
  # [./break_boundary2]
  #   input = subdomain2
  #   type = BreakBoundaryOnSubdomainGenerator
  # # [../]
  [./interface1]
    type = SideSetsBetweenSubdomainsGenerator
    input = subdomain1
    master_block = '0'
    paired_block = '1'
    new_boundary = 'Interface'
  [../]
  # [./interface2]
  #   type = SideSetsBetweenSubdomainsGenerator
  #   input = interface1
  #   master_block = '2'
  #   paired_block = '1'
  #   new_boundary = 'master2_interface'
  # [../]
  # [interface]
  #   type = SideSetsAroundSubdomainGenerator
  #   input = subdomain2
  #   block = 1
  #   new_boundary = 'interface'
  # []
[]

[GlobalParams]
[]

[Variables]
  [./p]
    block = 1
  [../]
  [./disp_x]
    block = 0
  [../]
[]

[AuxVariables]
  [./vel_x]
    block = 0
  [../]
  [./accel_x]
    block = 0
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
[]

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = 'p'
    block = 1
  [../]
  [./inertia]
    type = InertialForce
    variable = p
    block = 1
  [../]
  [./DynamicTensorMechanics]
    displacements = 'disp_x'
    block = 0
  [../]
  [./inertia_x1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = 0
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
    block = 0
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
    block = 0
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    block = 0
  [../]
[]

[InterfaceKernels]
  [./interface1]
    type =  FluidStructureInterface_3D
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'Interface'
    D = 1e-6
    D_neighbor = 1.0
    component = 0
  [../]
[]

[BCs]
  [./bottom_accel]
    type = FunctionDirichletBC
    variable = p
    boundary = 'right'
    function = accel_bottom
  [../]
  [./disp_x1]
    type = DirichletBC
    boundary = 'left'
    variable = disp_x
    value = 0.0
  [../]
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Input_1Peak_highF.csv # Input_1Peak.csv
    scale_factor = 1e-2
    format = 'columns'
  [../]
[]

[Materials]
  [./density]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 4.44e-7
    block = 1
  [../]
  [./density0]
    type = GenericConstantMaterial
    block = 0
    prop_names = density
    prop_values = 1e-6
  [../]
  [./elasticity_base]
    type = ComputeIsotropicElasticityTensor
    # youngs_modulus = 2e2
    # poissons_ratio = 0.3
    bulk_modulus = 2.25
    shear_modulus = 0.0
    block = 0
  [../]
  [./strain]
    type = ComputeFiniteStrain
    block = 0
    displacements = 'disp_x'
  [../]
  [./stress]
    type =  ComputeFiniteStrainElasticStress
    block = 0
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
  solve_type = 'NEWTON'
  # petsc_options = '-snes_ksp_ew'
  # petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  # petsc_options_value = '201                hypre    boomeramg      4'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  start_time = 0.0
  end_time = 0.12
  dt = 0.0001
  dtmin = 0.00001
  nl_abs_tol = 1e-12 # 1e-3
  nl_rel_tol = 1e-12 # 1e-3
  l_tol = 1e-12 # 1e-3
  l_max_its = 25
  timestep_tolerance = 1e-8
  automatic_scaling = true
  [TimeIntegrator]
    type = NewmarkBeta
  []
[]

[Postprocessors]
  [./p_int]
    type = PointValue
    point = '10.0 0.0 0.0'
    variable = p
  [../]
  [./stress1_int]
    type = PointValue
    point = '10.0 0.0 0.0'
    variable = stress_xx
  [../]
  [./dispx_int]
    type = PointValue
    point = '10.0 0.0 0.0'
    variable = disp_x
  [../]
  [./p_30]
    type = PointValue
    point = '30.0 0.0 0.0'
    variable = p
  [../]
  [./stress1_mid]
    type = PointValue
    point = '5.0 0.0 0.0'
    variable = stress_xx
  [../]
  [./dispx_mid]
    type = PointValue
    point = '5.0 0.0 0.0'
    variable = disp_x
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = true
  file_base = Ex_1D_Test
  [./out]
    execute_on = 'TIMESTEP_BEGIN'
    type = CSV
    file_base = 1D_Test
  [../]
[]
