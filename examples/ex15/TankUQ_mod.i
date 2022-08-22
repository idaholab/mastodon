[Mesh]
  [file]
    type = FileMeshGenerator
    file = Vessel_NoIsolator_Coarse.e
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
  [top_accel1]
    type = PresetAcceleration
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    function = groundmotion
    boundary = 'Top'
  []
  [disp_x2]
    type = DirichletBC
    variable = disp_y
    boundary = 'Top'
    value = 0.0
  []
  [disp_x3]
    type = DirichletBC
    variable = disp_z
    boundary = 'Top'
    value = 0.0
  []
  [free]
    type = FluidFreeSurfaceBC
    variable = p
    boundary = 'Fluid_top'
    alpha = '0.1'
  []
[]

[Functions]
  [groundmotion]
    type = GroundMotionSim
    M = 0
    R = 0
    Vs30 = 0
    F = 'SS'
    n = 0
    execute_on = INITIAL
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
  end_time = 10.0
  dt = 0.005
  dtmin = 0.001
  nl_abs_tol = 1e-14
  nl_rel_tol = 1e-14
  l_tol = 1e-14
  l_max_its = 25
  timestep_tolerance = 1e-8
  automatic_scaling = true
  [TimeIntegrator]
    type = NewmarkBeta
  []
[]

[Controls]
  [stochastic]
    type = SamplerReceiver
  []
[]

[Postprocessors]
  [PSide]
    type = PointValue
    point = '2.412779 -0.425438 0.050000'
    variable = p
  []
  [Wave]
    type = PointValue
    point = '2.450000 0.000000 5.050000'
    variable = Wave1
  []
  [DispBottom]
    type = PointValue
    point = '2.462019 0.434120 0.000'
    variable = disp_x
  []
  [AccBottom]
    type = PointValue
    point = '2.462019 0.434120 0.000'
    variable = accel_x
  []
  [PeakDisp]
    type = TimeExtremeValue
    postprocessor = DispBottom
    output_type = extreme_value
  []
  [PeakAcc]
    type = TimeExtremeValue
    postprocessor = AccBottom
    output_type = extreme_value
  []
  [PeakPressure]
    type = TimeExtremeValue
    postprocessor = PSide
    output_type = extreme_value
  []
  [PeakWaveHeight]
    type = TimeExtremeValue
    postprocessor = Wave
    output_type = extreme_value
  []
  [AppliedAcc]
     type = FunctionValuePostprocessor
     function = groundmotion
  []

[]
[VectorPostprocessors]
  [accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x'
    nodes = '578'
    block = 1
  []
  [accel_spec]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist
    regularize_dt = 0.005
    outputs = out
  []
[]

[Outputs]
  [accout]
    type = CSV
    show = 'AppliedAcc PSide Wave DispBottom AccBottom'
  []
  [out]
    type = CSV
    execute_on = FINAL
  []
[]
