# Test for cyclic tensile and compressive nonlinear soil behavior (without AD)

# Soil column consists of 3 elements, each 1m x 1m x 1m, stacked on top of one
# another vertically. The middle element is nonlinear, while the top and bottom
# elements are linear elastic. The bottom boundary is fixed in all directions.
# A sawtooth pulse is applied in both X and Z directions of the soil column.

# The hydrostatic and vonMises stresses of the nonlinear element are printed out
# as output. When the nonlinear soil element is under tension, the hydrostatic
# stress reduces to zero, and the vonMises stress also reduces to zero
# indicating that the nonlinear soil element has failed. When the element is
# compressed, it regains strength. So, the hydrostatic stress becomes negative
# and vonMises stress becomes non-zero.


[Mesh]
  type = GeneratedMesh # Can generate simple lines, rectangles and rectangular prisms
  dim = 3 # Dimension of the mesh
  nx = 1 # Number of elements in the x direction
  ny = 1 # Number of elements in the y direction
  nz = 3 # Number of elements in the z direction
  xmin = 0.0
  xmax = 1
  ymin = 0.0
  ymax = 1
  zmin = 0.0
  zmax = 3.0
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  volumetric_locking_correction = true
[]

[MeshModifiers]
  [./block1]
    type = SubdomainBoundingBox
    bottom_left = '0.0 0.0 0.0'
    top_right = '1.0 1.0 1.0'
    block_id = '1'
  [../]
  [./block2]
    type = SubdomainBoundingBox
    bottom_left = '0.0 0.0 1.0'
    top_right = '1.0 1.0 2.0'
    block_id = '2'
  [../]
  [./block3]
    type = SubdomainBoundingBox
    bottom_left = '0.0 0.0 2.0'
    top_right = '1.0 1.0 3.0'
    block_id = '3'
  [../]
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
  [./accel_x]
  [../]
  [./vel_y]
  [../]
  [./accel_y]
  [../]
  [./vel_z]
  [../]
  [./accel_z]
  [../]
  [./stress_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./vonmises]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./hydrostatic]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    use_displaced_mesh = false
    zeta = 0.00006366
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
    eta = 7.854
    use_displaced_mesh = false
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    eta = 7.854
    use_displaced_mesh = false
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
    eta = 7.854
    use_displaced_mesh = false
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
[./accel_z]
  type = NewmarkAccelAux
  variable = accel_z
  displacement = disp_z
  velocity = vel_z
  beta = 0.25
  execute_on = timestep_end
[../]
[./vel_z]
  type = NewmarkVelAux
  variable = vel_z
  acceleration = accel_z
  gamma = 0.5
  execute_on = timestep_end
[../]
  [./layer]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '4.0'
    direction = '0 0 1'
    execute_on = initial
  [../]
  [./stress_zx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zx
    index_i = 0
    index_j = 2
  [../]
  [./strain_zx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zx
    index_i = 0
    index_j = 2
  [../]
  [./vonmises]
    type = RankTwoScalarAux
    rank_two_tensor = stress
    variable = vonmises
    scalar_type = vonmisesStress
  [../]
  [./hydrostatic]
    type = RankTwoScalarAux
    rank_two_tensor = stress
    variable = hydrostatic
    scalar_type = hydrostatic
  [../]
[]

[BCs]
  [./x_bot]
    type = PresetBC
    variable = disp_x
    boundary = 'back'
    value = 0.0
  [../]
  [./y_bot]
    type = PresetBC
    variable = disp_y
    boundary = 'back'
    value = 0.0
  [../]
  [./z_bot]
    type = PresetBC
    variable = disp_z
    boundary = 'back'
    value = 0.0
  [../]
  [./top_disp]
    type = FunctionPresetBC
    variable = disp_x
    function = top_disp
    boundary = front
  [../]
  [./top_disp_z]
    type = FunctionPresetBC
    variable = disp_z
    function = top_disp_z
    boundary = front
  [../]
[]

[Functions]
  [./top_disp]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 8.0'
    y = '0.0 1.0 0.0 -1.0 0.0 0.0'
    scale_factor = 5e-3
  [../]
  [./top_disp_z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0 6.0 8.0'
    y = '0.0 -2.0 0.0 3.0 0.0 -2.0 0.0 0.0'
    scale_factor = -5e-3
  [../]
[]

[Materials]
  [./sample_isoil]
    type = ComputeISoilStress
    soil_type = 'user_defined'
    layer_variable = layer_id
    layer_ids = '0'
    backbone_curve_files = 'stress_strain_test_1.csv'
    poissons_ratio = '0.3'
    pressure_dependency = true
    a0 = 0
    a1 = 0
    a2 = 1
    b_exp = 0.5
    p_ref = 1e-4
    tension_pressure_cut_off = -1e-9
    initial_shear_modulus = '5e-4'
    block = 2
  [../]
  [./sample_isoil_strain]
    type = ComputeIncrementalSmallStrain
    block = '1 2 3'
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./sample_isoil_elasticitytensor]
    type = ComputeIsotropicElasticityTensorSoil
    block = '2'
    shear_modulus = '5e-4'
    poissons_ratio = '0.3'
    density = '6'
    wave_speed_calculation = false
    layer_ids = '0'
    layer_variable = layer_id
  [../]
  [./sample_isoil_elasticitytensor2]
    type = ComputeIsotropicElasticityTensorSoil
    block = '1 3'
    elastic_modulus = '1.0e-2'
    poissons_ratio = '0.3'
    density = '2e-2'
    wave_speed_calculation = false
    layer_ids = '0'
    layer_variable = layer_id
  [../]
  [./sample_soil]
    type = ComputeFiniteStrainElasticStress
    block = '1 3'
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
  solve_type = NEWTON

  start_time = 0.0
  end_time = 6.0
  dt = 0.01
  dtmin = 0.01
  nl_max_its = 200
  timestep_tolerance = 1e-6
  nl_rel_tol = 5e-5
  nl_abs_tol = 1e-7
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_6x]
    type = PointValue
    point = '0.0 0.0 1.0'
    variable = disp_x
  [../]
  [./disp_6y]
    type = PointValue
    point = '0.0 0.0 1.0'
    variable = disp_y
  [../]
  [./disp_6z]
    type = PointValue
    point = '0.0 0.0 1.0'
    variable = disp_z
  [../]
  [./strain_zx]
    type = ElementalVariableValue
    variable = strain_zx
    elementid = 1
  [../]
  [./stress_zx]
    type = ElementalVariableValue
    variable = stress_zx
    elementid = 1
  [../]
  [./hydrostatic]
    type = ElementalVariableValue
    variable = hydrostatic
    elementid = 1
  [../]
  [./vonmises]
    type = ElementalVariableValue
    variable = vonmises
    elementid = 1
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  perf_graph = false
  [./console]
    type = Console
    max_rows = 1
  [../]
[]
