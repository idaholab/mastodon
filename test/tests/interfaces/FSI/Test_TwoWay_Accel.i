[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 100
    xmax = 200
  []
  [./subdomain1]
    input = gen
    type = SubdomainBoundingBoxGenerator
    bottom_left = '100.0 0 0'
    block_id = 1
    top_right = '200.0 0.0 0'
  [../]
  [./interface]
    type = SideSetsBetweenSubdomainsGenerator
    input = subdomain1
    master_block = '0'
    paired_block = '1'
    new_boundary = 'master0_interface'
  [../]
  [./interface_again]
    type = SideSetsBetweenSubdomainsGenerator
    input = interface
    master_block = '1'
    paired_block = '0'
    new_boundary = 'master1_interface'
  [../]
[]

[GlobalParams]
  # order = SECOND
  # family = LAGRANGE
[]

[Variables]
  [./p]
    # order = FIRST
    # family = LAGRANGE
    block = 1
  [../]
  [./disp_x]
    # order = SECOND
    # family = LAGRANGE
    # order = FIRST
    # family = LAGRANGE
    block = 0
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
      block = 0
  [../]
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
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
[]

# [AuxVariables]
#   [./disp_x]
#     # order = SECOND
#     # family = LAGRANGE
#   [../]
#   [./disp_y]
#     # order = SECOND
#     # family = LAGRANGE
#   [../]
#   [./disp_z]
#     # order = SECOND
#     # family = LAGRANGE
#   [../]
# []

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = 'p'
    block = 1
    # D =  1.0 # 2250000
  [../]
  [./inertia]
    type = InertialForce
    variable = p
    block = 1
  [../]
  [./DynamicTensorMechanics]
    displacements = 'disp_x'
    block = '0'
  [../]
  [./inertia_x1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = '0'
  [../]
[]

[AuxKernels]
  # [./disp_z]
  #   type = WaveHeightAuxKernel
  #   variable = 'disp_z'
  #   pressure = p
  #   dens = 1000.0
  #   grav = 9.81
  #   execute_on = timestep_end
  # [../]
  [./grad_press]
    block = 1
    type = FluidFluxAuxKernel
    variable = 'flux_p'
    pressure = 'p'
    fluiddens = 1.0
  [../]
  [./acc]
    block = 0
    type = StructureFluxAuxKernel
    variable = 'acc1'
    dispx = disp_x
    # pressure = 'disp_x'
    fluiddens = 1000.0 # 1000.0
  [../]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = 'timestep_end'
    block = '0'
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
    block = '0'
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    block = '0'
  [../]
  [./strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
    block = '0'
  [../]
[]

[InterfaceKernels]
  [./interface]
    type =  FluidStructureInterface # FSIntFlxMat #
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'master0_interface'
    D = 1000.0
    D_neighbor = 1.0
  [../]
  # [./interface1]
  #   type =  FSIStressMatch # FSIntFlxMat #
  #   variable = disp_x
  #   neighbor_var = 'p'
  #   boundary = 'master0_interface'
  #   D = 2e11
  #   D_neighbor = 1.0
  # [../]
[]

[BCs]
  [./bottom_accel]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'left' #'right'
    function = accel_bottom
  [../]
  # [./disp_x]
  #   type = PresetAcceleration
  #   variable = disp_x
  #   velocity = vel_x
  #   acceleration = accel_x
  #   beta = 0.25
  #   function = accel_bottom
  #   boundary = 'left'
  # [../]
  # [./disp_x]
  #   type = PresetBC
  #   boundary = 'left'
  #   variable = disp_x
  #   value = 0.0
  # [../]
  # [./p_x]
  #   type = PresetBC
  #   boundary = 'right'
  #   variable = p
  #   value = 0.0
  # [../]
  # [./bottom_accel]
  #   type = FunctionDirichletBC
  #   variable = disp_x
  #   boundary = 'left'
  #   function = accel_bottom
  # [../]
  # [./bottom_accel1]
  #   type = DirichletBC
  #   variable = p
  #   boundary = 'right'
  #   value = 0
  # [../]
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Ormsby_USE1.csv # Input_5Sines.csv
    scale_factor = 0.01
    format = 'columns'
  [../]
  # [./accel_bottom]
  #   type = ParsedFunction
  #   value = sin(t)
  # [../]
  # [./accel_bottom]
  #   type = ParsedFunction
  #   value = pi*sin(alpha*pi*t)
  #   # vars = 'alpha'
  #   # vals = '16'
  # [../]
[]

[Materials]
  [./density]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 4.44e-7 # 4.44e-7
    block = '1'
  [../]
  [./density0]
    type = GenericConstantMaterial
    block = '0'
    prop_names = density
    prop_values = 8050 # 2070 8050
  [../]
  [./elasticity_base]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 2e11 #2e11 5500
    poissons_ratio = 0.3
    block = '0'
  [../]
  [./strain]
    type = ComputeFiniteStrain #ComputeIncrementalSmallStrain
    block = '0'
    displacements = 'disp_x'
  [../]
  [./stress]
    type =  ComputeFiniteStrainElasticStress # ComputeLinearElasticStress
    block = '0'
  [../]
  # [./stress_beam2]
  #   type = ComputeFiniteStrainElasticStress
  #   block = '0'
  # [../]
  # [./strain_15]
  #   type = ComputeFiniteStrain
  #   block = '0'
  #   displacements = 'disp_x'
  # [../]
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
  start_time = 0.8
  end_time = 1.2
  dt = 0.001
  dtmin = 0.00001
  nl_abs_tol = 1e-6 # 1e-3
  nl_rel_tol = 1e-6 # 1e-3
  l_tol = 1e-6 # 1e-3
  l_max_its = 25
  timestep_tolerance = 1e-8
  automatic_scaling = true
  # [TimeStepper]
  #   type = IterationAdaptiveDT
  #   optimal_iterations = 1
  #   linear_iteration_ratio = 1
  #   dt = 0.001
  # []
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
  [./grad_p]
    type = PointValue
    point = '100.0 0.0 0.0'
    variable = flux_p
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
    file_base = Test_acc
  [../]
[]
