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
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = TestNewmarkTI
    variable = accel_x
    displacement = disp_x
    first = false
  [../]
  [./vel_x]
    type = TestNewmarkTI
    variable = vel_x
    displacement = disp_x
  [../]
  [./accel_y]
    type = TestNewmarkTI
    variable = accel_y
    displacement = disp_y
    first = false
  [../]
  [./vel_y]
    type = TestNewmarkTI
    variable = vel_y
    displacement = disp_y
  [../]
  [./accel_z]
    type = TestNewmarkTI
    variable = accel_z
    displacement = disp_z
    first = false
  [../]
  [./vel_z]
    type = TestNewmarkTI
    variable = vel_z
    displacement = disp_z
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
  [./strain]
    type = ComputeSmallStrain
    displacements = 'disp_x disp_y disp_z'
    implicit = false
  [../]
  [./stress]
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
  start_time = 0
  end_time = 2e-2
  timestep_tolerance = 1e-06
  [TimeIntegrator]
    type = CentralDifference
    solve_type = consistent
  []
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = time_step
    dt = 1e-5
  [../]
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./time_step]
    type = CriticalTimeStep
    factor = 0.9
  [../]
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
