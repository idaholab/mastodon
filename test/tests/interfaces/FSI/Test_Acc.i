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
    nx = 600
    xmax = 300
  []
  [./subdomain1]
    input = gen
    type = SubdomainBoundingBoxGenerator
    bottom_left = '100.0 0 0'
    block_id = 1
    top_right = '200.0 0.0 0'
  [../]
  [./subdomain2]
    input = subdomain1
    type = SubdomainBoundingBoxGenerator
    bottom_left = '200.0 0 0'
    block_id = 2
    top_right = '300.0 0.0 0'
  [../]
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
    input = subdomain2
    master_block = '0'
    paired_block = '1'
    new_boundary = 'master1_interface'
  [../]
  [./interface2]
    type = SideSetsBetweenSubdomainsGenerator
    input = interface1
    master_block = '2'
    paired_block = '1'
    new_boundary = 'master2_interface'
  [../]
  # [interface]
  #   type = SideSetsAroundSubdomainGenerator
  #   input = subdomain2
  #   block = 1
  #   new_boundary = 'interface'
  # []
[]

[MeshModifiers]
  [./middle_node]
    type = AddExtraNodeset
    new_boundary = 'middle_node'
    coord = '150.0 0.0 0.0'
  [../]
[]

[GlobalParams]
[]

[Variables]
  [./p]
    block = 1
  [../]
  [./disp_x]
    block = '0 2'
  [../]
[]

[AuxVariables]
  [./flux_p]
      order = FIRST
      family = MONOMIAL
      block = 1
  [../]
  [./acc1]
      order = FIRST
      family = MONOMIAL
      block = '0 2'
  [../]
  [./vel_x]
    block = '0 2'
  [../]
  [./accel_x]
    block = '0 2'
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
    block = '0 2'
  [../]
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = '0 2'
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
    block = '0 2'
  [../]
  [./inertia_x1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = '0 2'
  [../]
[]

[AuxKernels]
  [./grad_press]
    block = 1
    type = FluidFluxAuxKernel
    variable = 'flux_p'
    pressure = 'p'
    fluiddens = 1.0
  [../]
  [./acc]
    block = '0 2'
    type = StructureFluxAuxKernel
    variable = 'acc1'
    dispx = disp_x
    fluiddens = 1e-6
  [../]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = 'timestep_end'
    block = '0 2'
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
    block = '0 2'
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    block = '0 2'
  [../]
  [./strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
    block = '0 2'
  [../]
[]

[InterfaceKernels]
  [./interface1]
    type =  FluidStructureInterface # FSIntFlxMat #
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'master1_interface'
    D = 1e-6
    D_neighbor = 1.0
  [../]
  [./interface2]
    type =  FluidStructureInterface # FSIntFlxMat #
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'master2_interface'
    D = -1e-6
    D_neighbor = -1.0
  [../]
[]

[BCs]
  # [./bottom_accel]
  #   type = FunctionDirichletBC
  #   variable = p
  #   boundary = 'middle_node'
  #   function = accel_bottom
  # [../]
  # [./disp_x1]
  #   type = DirichletBC
  #   boundary = 'left'
  #   variable = disp_x
  #   value = 0.0
  # [../]
  # [./disp_x2]
  #   type = DirichletBC
  #   boundary = 'right'
  #   variable = disp_x
  #   value = 0.0
  # [../]
  [./bottom_accel]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'left'
    function = accel_bottom
  [../]
  [./bottom_accel1]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'right'
    function = accel_bottom
  [../]
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Input_5Sines.csv
    scale_factor = 0.01
    format = 'columns'
  [../]
[]

[Materials]
  [./density]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 4.44e-7
    block = '1'
  [../]
  [./density0]
    type = GenericConstantMaterial
    block = '0 2'
    prop_names = density
    prop_values = 8.05e-6
  [../]
  [./elasticity_base]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 2e2
    poissons_ratio = 0.3
    block = '0 2'
  [../]
  [./strain]
    type = ComputeFiniteStrain
    block = '0 2'
    displacements = 'disp_x'
  [../]
  [./stress]
    type =  ComputeFiniteStrainElasticStress
    block = '0 2'
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
  end_time = 0.4
  dt = 0.001
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
  [./p1]
    type = PointValue
    point = '100.0 0.0 0.0'
    variable = p
  [../]
  [./grad_p1]
    type = PointValue
    point = '100.0 0.0 0.0'
    variable = flux_p
  [../]
  [./stress1]
    type = PointValue
    point = '100.0 0.0 0.0'
    variable = stress_xx
  [../]
  [./a1]
    type = PointValue
    point = '100.0 0.0 0.0'
    variable = acc1
  [../]
  [./p2]
    type = PointValue
    point = '200.0 0.0 0.0'
    variable = p
  [../]
  [./grad_p2]
    type = PointValue
    point = '200.0 0.0 0.0'
    variable = flux_p
  [../]
  [./stress2]
    type = PointValue
    point = '200.0 0.0 0.0'
    variable = stress_xx
  [../]
  [./a2]
    type = PointValue
    point = '200.0 0.0 0.0'
    variable = acc1
  [../]
[]

[Outputs]
  csv = true
  # exodus = true
  perf_graph = true
  print_linear_residuals = true
  # file_base = Exodus_Test_1
  [./out]
    execute_on = 'TIMESTEP_BEGIN'
    type = CSV
    file_base = Embed_Acc
  [../]
[]
