[Mesh]
  type = FileMesh
  file = Cubit/2D_Tank.e
  uniform_refine = 2
[]

[GlobalParams]
[]

[Variables]
  [./p]
    block = 2
  [../]
  [./disp_x]
    block = 1
  [../]
  [./disp_y]
    block = 1
  [../]
[]

[AuxVariables]
  [./disp_y1]
    # order = SECOND
    # family = LAGRANGE
    block = 2
  [../]
  # [./flux_p]
  #     order = FIRST
  #     family = MONOMIAL
  #     block = 1
  # [../]
  # [./acc1]
  #     order = FIRST
  #     family = MONOMIAL
  #     block = '0'
  # [../]
  [./vel_x]
    block = 1
  [../]
  [./accel_x]
    block = 1
  [../]
  [./vel_y]
    block = 1
  [../]
  [./accel_y]
    block = 1
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  # [./strain_xx]
  #   order = CONSTANT
  #   family = MONOMIAL
  #   block = '0'
  # [../]
[]

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = 'p'
    block = 2
  [../]
  [./inertia]
    type = InertialForce
    variable = p
    block = 2
  [../]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y'
    block = 1
  [../]
  [./inertia_x1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    block = 1
  [../]
  [./inertia_y1]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    block = 1
  [../]
[]

[AuxKernels]
  [./waves]
    type = WaveHeightAuxKernel
    variable = 'disp_y1'
    pressure = p
    dens = 1e-6
    grav = 9.81
    execute_on = timestep_end
  [../]
  # [./grad_press]
  #   block = 1
  #   type = FluidFluxAuxKernel
  #   variable = 'flux_p'
  #   pressure = 'p'
  #   fluiddens = 1.0
  # [../]
  # [./acc]
  #   block = '0'
  #   type = StructureFluxAuxKernel
  #   variable = 'acc1'
  #   dispx = disp_x
  #   fluiddens = 1e-6
  # [../]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = 'timestep_end'
    block = 1
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
    block = 1
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = 'timestep_end'
    block = 1
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = 'timestep_end'
    block = 1
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    block = 1
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
    block = 1
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
    block = 1
  [../]
  # [./strain_xx]
  #   type = RankTwoAux
  #   rank_two_tensor = total_strain
  #   variable = strain_xx
  #   index_i = 0
  #   index_j = 0
  #   block = '0'
  # [../]
[]

[InterfaceKernels]
  [./interface1x]
    type =  FluidStructureInterface_3D
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'Int_left'
    D = 1e-6
    D_neighbor = 1.0
    component = 0
  [../]
  [./interface1y]
    type =  FluidStructureInterface_3D
    variable = disp_y
    neighbor_var = 'p'
    boundary = 'Int_left'
    D = 1e-6
    D_neighbor = 1.0
    component = 0
  [../]
  [./interface2x]
    type =  FluidStructureInterface_3D
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'Int_right'
    D = 1e-6
    D_neighbor = 1.0
    component = 0
  [../]
  [./interface2y]
    type =  FluidStructureInterface_3D
    variable = disp_y
    neighbor_var = 'p'
    boundary = 'Int_right'
    D = 1e-6
    D_neighbor = 1.0
    component = 0
  [../]
  [./interface3x]
    type =  FluidStructureInterface_3D
    variable = disp_x
    neighbor_var = 'p'
    boundary = 'Int_bot'
    D = 1e-6
    D_neighbor = 1.0
    component = 1
  [../]
  [./interface3y]
    type =  FluidStructureInterface_3D
    variable = disp_y
    neighbor_var = 'p'
    boundary = 'Int_bot'
    D = 1e-6
    D_neighbor = 1.0
    component = 1
  [../]
[]

[BCs]
  [./free]
    type = FluidFreeSurfaceBC
    variable = p
    boundary = 'Fluid_top'
    alpha = '0.1'
  []
  # [./bottom_accel]
  #   type = FunctionDirichletBC
  #   variable = p
  #   boundary = 'right'
  #   function = accel_bottom
  # [../]
  # [./disp_x1]
  #   type = NeumannBC
  #   boundary = 'left'
  #   variable = disp_x
  #   value = 0.0
  # [../]
  [./bottom_accel]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 'Bottom'
    function = accel_bottom
    # value = 0
  [../]
  # [./disp_x1]
  #   type = FunctionNeumannBC
  #   boundary = 'left'
  #   variable = disp_x
  #   function = accel_bottom
  # [../]
  # [./disp_x1]
  #   type = DirichletBC
  #   boundary = 'Left'
  #   variable = disp_x
  #   value = 0.0
  # [../]
  [./disp_x2]
    type = DirichletBC
    boundary = 'Bottom'
    variable = disp_y
    value = 0.0
  [../]
  # [./disp_x2t]
  #   type = DirichletBC
  #   boundary = 'Str_topbot'
  #   variable = disp_y
  #   value = 0.0
  # [../]
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Ormsby_USE1.csv # Input_1Peak_highF.csv # Input_1Peak.csv
    scale_factor = 1e-2
    format = 'columns'
  [../]
  # [./accel_bottom]
  #   type = ParsedFunction
  #   value = 1e-3-1e-3*cos(t*3.141592653*10)
  #   # data_file = Input_5Sines.csv
  #   # scale_factor = 1e-2
  #   # format = 'columns'
  # [../]
[]

[Materials]
  [./density]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 4.44e-7
    block = 2
  [../]
  [./density0]
    type = GenericConstantMaterial
    block = 1
    prop_names = density
    prop_values = 7.7e-6
  [../]
  [./elasticity_base]
    type = ComputeIsotropicElasticityTensor
    # bulk_modulus = 2.25
    # shear_modulus = 0.0
    youngs_modulus = 2e2 # 2e2
    poissons_ratio = 0.3
    # lambda = -1e20
    # shear_modulus = 1e20
    block = 1
  [../]
  [./strain]
    type = ComputeFiniteStrain
    block = 1
    displacements = 'disp_x disp_y'
  [../]
  [./stress]
    type =  ComputeFiniteStrainElasticStress
    block = 1
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
  solve_type = 'PJFNK'
  # petsc_options = '-snes_ksp_ew'
  # petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  # petsc_options_value = '201                hypre    boomeramg      4'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  start_time = 0.0
  end_time = 2.0
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
    point = '-0.79 1.05 0.0'
    variable = p
  [../]
  [./p2]
    type = PointValue
    point = '-0.79 0.85 0.0'
    variable = p
  [../]
  [./p3]
    type = PointValue
    point = '-0.79 0.65 0.0'
    variable = p
  [../]
  [./p4]
    type = PointValue
    point = '-0.79 0.45 0.0'
    variable = p
  [../]
  [./p5]
    type = PointValue
    point = '-0.79 0.25 0.0'
    variable = p
  [../]
  [./p6]
    type = PointValue
    point = '-0.79 0.05 0.0'
    variable = p
  [../]
  [./disp1]
    type = PointValue
    point = '-0.79 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp2]
    type = PointValue
    point = '-0.632 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp3]
    type = PointValue
    point = '-0.474 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp4]
    type = PointValue
    point = '-0.318 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp5]
    type = PointValue
    point = '-0.158 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp6]
    type = PointValue
    point = '0.0 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp7]
    type = PointValue
    point = '0.158 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp8]
    type = PointValue
    point = '0.316 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp9]
    type = PointValue
    point = '0.474 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp10]
    type = PointValue
    point = '0.632 1.05 0.0'
    variable = disp_y1
  [../]
  [./disp11]
    type = PointValue
    point = '0.79 1.05 0.0'
    variable = disp_y1
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = true
  file_base = Ex_2D_Tank_Ormsby
  [./out]
    execute_on = 'TIMESTEP_BEGIN'
    type = CSV
    file_base = 2D_Tank_Ormsby
  [../]
[]
