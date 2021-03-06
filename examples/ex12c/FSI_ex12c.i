[Mesh]
  [file]
    type = FileMeshGenerator
    file = Model_2.e
  []
  [interface1]
    type = SideSetsBetweenSubdomainsGenerator
    input = file
    primary_block = '2'
    paired_block = '1'
    new_boundary = 'Interface'
  []
[]

[GlobalParams]
[]

[Variables]
  [p]
   block = 2
  []
  [disp_x]
    block = 1
  []
  [disp_y]
    block = 1
  []
  [disp_z]
    block = 1
  []
[]

[AuxVariables]
  [Wave1]
    block = 2
  []
  [vel_x]
    order = FIRST
    family = LAGRANGE
    block = 1
  []
  [accel_x]
    order = FIRST
    family = LAGRANGE
    block = 1
  []
  [vel_y]
    order = FIRST
    family = LAGRANGE
    block = 1
  []
  [accel_y]
    order = FIRST
    family = LAGRANGE
    block = 1
  []
  [vel_z]
    order = FIRST
    family = LAGRANGE
    block = 1
  []
  [accel_z]
    order = FIRST
    family = LAGRANGE
    block = 1
  []
  [stress_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [stress_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [stress_xy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [stress_zz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [stress_yz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
  [stress_xz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = 'p'
    block = 2
  []
  [inertia]
    type = AcousticInertia
    variable = p
    block = 2
  []
  [DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    block = 1
  []
  [inertia_x1]
    type = InertialForce
    variable = disp_x
    block = 1
  []
  [inertia_y1]
    type = InertialForce
    variable = disp_y
    block = 1
  []
  [inertia_z1]
    type = InertialForce
    variable = disp_z
    block = 1
  []
[]

[AuxKernels]
  [waves]
    type = WaveHeightAuxKernel
    variable = 'Wave1'
    pressure = p
    density = 1e-6
    gravity = 9.81
    execute_on = timestep_end
    block = 2
  []
  [accel_x]
    type = TestNewmarkTI
    displacement = disp_x
    variable = accel_x
    first = false
    block = 1
  []
  [vel_x]
    type = TestNewmarkTI
    displacement = disp_x
    variable = vel_x
    block = 1
  []
  [accel_y]
    type = TestNewmarkTI
    displacement = disp_y
    variable = accel_y
    first = false
    block = 1
  []
  [vel_y]
    type = TestNewmarkTI
    displacement = disp_y
    variable = vel_y
    block = 1
  []
  [accel_z]
    type = TestNewmarkTI
    displacement = disp_z
    variable = accel_z
    first = false
    block = 1
  []
  [vel_z]
    type = TestNewmarkTI
    displacement = disp_z
    variable = vel_z
    block = 1
  []
  [stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
    block = 1
  []
  [stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
    block = 1
  []
  [stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
    block = 1
  []
  [stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
    block = 1
  []
  [stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 1
    index_j = 2
    block = 1
  []
  [stress_xz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xz
    index_i = 0
    index_j = 2
    block = 1
  []
[]

[InterfaceKernels]
  [interface1]
    type = StructureAcousticInterface
    variable = p
    neighbor_var = disp_x
    boundary = 'Interface'
    D = 1e-6
    component = 0
  []
  [interface2]
    type = StructureAcousticInterface
    variable = p
    neighbor_var = disp_y
    boundary = 'Interface'
    D = 1e-6
    component = 1
  []
  [interface3]
    type = StructureAcousticInterface
    variable = p
    neighbor_var = disp_z
    boundary = 'Interface'
    D = 1e-6
    component = 2
  []
[]

[BCs]
  [bottom_accel1]
    type = PresetAcceleration
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    function = accel_bottom
    boundary = 'Bottom'
  []
  [disp_x2]
    type = DirichletBC
    variable = disp_y
    boundary = 'Bottom'
    value = 0.0
  []
  [disp_x3]
    type = DirichletBC
    variable = disp_z
    boundary = 'N6'
    value = 0.0
  []
  [free]
    type = FluidFreeSurfaceBC
    variable = p
    boundary = 'Fluid_top'
    alpha = '0.1'
  []
  [Nod1]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node1
    boundary = 'N1'
  []
  [Nod2]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node2
    boundary = 'N2'
  []
  [Nod3]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node3
    boundary = 'N3'
  []
  [Nod4]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node4
    boundary = 'N4'
  []
  [Nod5]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node5
    boundary = 'N5'
  []
  [Nod7]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node7
    boundary = 'N7'
  []
  [Nod8]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node8
    boundary = 'N8'
  []
  [Nod9]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node9
    boundary = 'N9'
  []
  [Nod10]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node10
    boundary = 'N10'
  []
  [Nod11]
    type = PresetAcceleration
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    function = Node11
    boundary = 'N11'
  []
[]

[Functions]
  [accel_bottom]
    type = PiecewiseLinear
    data_file = Case1_Lat.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node1]
    type = PiecewiseLinear
    data_file = Case1_N1.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node2]
    type = PiecewiseLinear
    data_file = Case1_N2.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node3]
    type = PiecewiseLinear
    data_file = Case1_N3.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node4]
    type = PiecewiseLinear
    data_file = Case1_N4.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node5]
    type = PiecewiseLinear
    data_file = Case1_N5.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node7]
    type = PiecewiseLinear
    data_file = Case1_N7.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node8]
    type = PiecewiseLinear
    data_file = Case1_N8.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node9]
    type = PiecewiseLinear
    data_file = Case1_N9.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node10]
    type = PiecewiseLinear
    data_file = Case1_N10.csv
    scale_factor = 1.0
    format = 'columns'
  []
  [Node11]
    type = PiecewiseLinear
    data_file = Case1_N11.csv
    scale_factor = 1.0
    format = 'columns'
  []
[]

[Materials]
  [density]
    type = GenericConstantMaterial
    prop_names = inv_co_sq
    prop_values = 4.65e-7
    block = 2
  []
  [density0]
    type = GenericConstantMaterial
    block = 1
    prop_names = density
    prop_values = 7.85e-6
  []
  [elasticity_base]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 2e2
    poissons_ratio = 0.27
    block = 1
  []
  [strain]
    type = ComputeFiniteStrain
    block = 1
    displacements = 'disp_x disp_y disp_z'
  []
  [stress]
    type =  ComputeFiniteStrainElasticStress
    block = 1
  []
[]

[Preconditioning]
  [andy]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  start_time = 0.0
  end_time = 11.0
  dt = 0.0025
  dtmin = 0.00001
  nl_abs_tol = 1e-10 # 1e-3
  nl_rel_tol = 1e-10 # 1e-3
  l_tol = 1e-10 # 1e-3
  l_max_its = 25
  timestep_tolerance = 1e-8
  automatic_scaling = true
  [TimeIntegrator]
    type = NewmarkBeta
  []
[]

[Postprocessors]
  [PBottom]
    type = PointValue
    point = '-0.76 0.0 0.3445'
    variable = p
  []
  [Wave]
    type = PointValue
    point = '0.6372078659 0.2070413862 1.6645'
    variable = Wave1
  []
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = true
  file_base = Ex_MirCase1
  [out]
    execute_on = 'TIMESTEP_BEGIN'
    type = CSV
    file_base = MirCase1
  []
[]
