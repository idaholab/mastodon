# One element test to test the auto-generated GQ/H backbone curve.
# The back surface of the element (z=0) is fixed and the front surface (z=1)
# is moved by applying a cyclic preset displacement.

# The resulting shear stress-strain curve was verified against obtained from DEEPSOIL.

[Mesh]
  type = GeneratedMesh # Can generate simple lines, rectangles and rectangular prisms
  dim = 3 # Dimension of the mesh
  nx = 1 # Number of elements in the x direction
  ny = 1 # Number of elements in the y direction
  nz = 1 # Number of elements in the z direction
  xmin = 0.0
  xmax = 1
  ymin = 0.0
  ymax = 1
  zmin = 0.0
  zmax = 1
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
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
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_zz]
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
    # stiffness_damping_coefficient = 0.00006366
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    eta = 7.854
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    eta = 7.854
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    eta = 7.854
  [../]
  [./gravity]
    type = Gravity
    variable = disp_z
    value = -9.81
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
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 1
    index_j = 0
  [../]
  [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 2
    index_j = 1
  [../]
  [./stress_zx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zx
    index_i = 0
    index_j = 2
  [../]
  [./strain_xy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = stress_xy
    index_i = 1
    index_j = 0
  [../]
  [./strain_yz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_yz
    index_i = 2
    index_j = 1
  [../]
  [./strain_zx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zx
    index_i = 0
    index_j = 2
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
  [./strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_xx
    index_i = 0
    index_j = 0
  [../]
  [./strain_yy]
    type = RankTwoAux
    rank_two_tensor =total_strain
    variable = strain_yy
    index_i = 1
    index_j = 1
  [../]
  [./strain_zz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zz
    index_i = 2
    index_j = 2
  [../]
  [./layer]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '2.0'
    direction = '0 0 1'
    execute_on = initial
  [../]
[]

[BCs]
  [./x_bot]
    type = DirichletBC
    variable = disp_x
    boundary = 0
    value = 0.0
  [../]
  [./y_bot]
    type = DirichletBC
    variable = disp_y
    boundary = 0
    value = 0.0
  [../]
  [./z_bot]
    type = DirichletBC
    variable = disp_z
    boundary = 0
    value = 0.0
  [../]
  [./Periodic]
    [./x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = '4'
      secondary = '2'
      translation = '1.0 0.0 0.0'
    [../]
    [./y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = '1'
      secondary = '3'
      translation = '0.0 1.0 0.0'
    [../]
  [../]
  [./top_x]
    type = FunctionDirichletBC
    boundary = 5
    function = top_disp
    variable = 'disp_x'
  [../]
[]

[Functions]
  [./top_disp]
    type = PiecewiseLinear
    data_file = '../ex01a/Displacement2.csv'
    format = columns
  [../]
[]

# [Materials]
#   [./sample_isoil]
#     type = ComputeISoilStress
#     soil_type = 'gqh'
#     layer_variable = layer_id
#     layer_ids = '0'
#     initial_soil_stress = '-4204.286 0 0  0 -4204.286 0  0 0 -9810'
#     poissons_ratio = '0.3'
#     number_of_points = 10
#     ### GQ/H ####
#     initial_shear_modulus = '20000000'
#     theta_1 = '-2.28'
#     theta_2 = '-5.54'
#     theta_3 = '1.0'
#     theta_4 = '1.0'
#     theta_5 = '0.99'
#     taumax = '7500'
#     block = '0'
#   [../]
#   [./sample_isoil_strain]
#     type = ComputeIncrementalSmallStrain
#     block = '0'
#     displacements = 'disp_x disp_y disp_z'
#     implicit = false
#   [../]
#   [./sample_isoil_elasticitytensor]
#     type = ComputeIsotropicElasticityTensorSoil
#     block = '0'
#     density = '2000'
#     wave_speed_calculation = false
#     layer_ids = '0'
#     poissons_ratio = '0.3'
#     shear_modulus = '20000000'
#     layer_variable = layer_id
#   [../]
# []

[Materials]
  [./I_Soil]
    [./soil_1]
      soil_type = 'gqh'
      layer_variable = layer_id
      layer_ids = '0'
      theta_1 = '-2.28'
      theta_2 = '-5.54'
      theta_3 = '1.0'
      theta_4 = '1.0'
      theta_5 = '0.99'
      taumax = '7500'
      initial_shear_modulus = '20000000'
      number_of_points = 10
      poissons_ratio = '0.3'
      block = 0
      initial_soil_stress = '-4204.286 0 0  0 -4204.286 0  0 0 -9810'
      density = '2000'
    [../]
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
  start_time = 0.0
  end_time = 8.0
  timestep_tolerance = 1e-6
  l_abs_tol = 1e-12
  [./TimeIntegrator]
    type = CentralDifference
  [../]
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = time_step
    dt = 0.001
  [../]
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_6x]
    type = NodalVariableValue
    nodeid = 6
    variable = disp_x
  [../]
  [./disp_6y]
    type = NodalVariableValue
    nodeid = 6
    variable = disp_y
  [../]
  [./disp_6z]
    type = NodalVariableValue
    nodeid = 6
    variable = disp_z
  [../]
  [./vel_6x]
    type = NodalVariableValue
    nodeid = 6
    variable = vel_x
  [../]
  [./vel_6y]
    type = NodalVariableValue
    nodeid = 6
    variable = vel_y
  [../]
  [./vel_6z]
    type = NodalVariableValue
    nodeid = 6
    variable = vel_z
  [../]
  [./accel_6x]
    type = NodalVariableValue
    nodeid = 6
    variable = accel_x
  [../]
  [./accel_6y]
    type = NodalVariableValue
    nodeid = 6
    variable = accel_y
  [../]
  [./accel_6z]
    type = NodalVariableValue
    nodeid = 6
    variable = accel_z
  [../]
  [./stress_xy_el]
    type = ElementalVariableValue
    variable = stress_xy
    elementid = 0
  [../]
  [./stress_yz_el]
    type = ElementalVariableValue
    variable = stress_yz
    elementid = 0
  [../]
  [./stress_zx_el]
    type = ElementalVariableValue
    variable = stress_zx
    elementid = 0
  [../]
  [./strain_xy_el]
    type = ElementalVariableValue
    variable = strain_xy
    elementid = 0
  [../]
  [./strain_yz_el]
    type = ElementalVariableValue
    variable = strain_yz
    elementid = 0
  [../]
  [./strain_zx_el]
    type = ElementalVariableValue
    variable = strain_zx
    elementid = 0
  [../]
  [./stress_xx_el]
    type = ElementalVariableValue
    variable = stress_xx
    elementid = 0
  [../]
  [./stress_yy_el]
    type = ElementalVariableValue
    variable = stress_yy
    elementid = 0
  [../]
  [./stress_zz_el]
    type = ElementalVariableValue
    variable = stress_zz
    elementid = 0
  [../]
  [./strain_xx_el]
    type = ElementalVariableValue
    variable = strain_xx
    elementid = 0
  [../]
  [./strain_yy_el]
    type = ElementalVariableValue
    variable = strain_yy
    elementid = 0
  [../]
  [./strain_zz_el]
    type = ElementalVariableValue
    variable = strain_zz
    elementid = 0
  [../]
  [./time_step]
    type = CriticalTimeStep
    factor = 0.9
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  perf_graph = false
  file_base = HYS_GQH_explicit
[]
