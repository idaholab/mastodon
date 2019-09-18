# Test for small strain Timoshenko beam vibration in y direction

# An impulse load is applied at the end of a cantilever beam of length 4m.
# The properties of the cantilever beam are as follows:
# Young's modulus (E) = 2e4
# Shear modulus (G) = 1e4
# Shear coefficient (k) = 1.0
# Cross-section area (A) = 1.0
# Iy = 1.0 = Iz
# Length (L)= 4 m
# density (rho) = 1.0

# For this beam, the dimensionless parameter alpha = kAGL^2/EI = 8
# Therefore, the beam behaves like a Timoshenko beam.

# The FEM solution for this beam with 100 elements give first natural period of 0.2731s with a time step of 0.005.
# The acceleration, velocity and displacement time histories obtained from MOOSE matches with those obtained from ABAQUS.

# Values from the first few time steps are as follows:
# time    disp_y                vel_y                 accel_y
# 0.0     0.0                   0.0                   0.0
# 0.005   2.5473249455812e-05   0.010189299782325     4.0757199129299
# 0.01    5.3012872677486e-05   0.00082654950634483  -7.8208200233219
# 0.015   5.8611622914354e-05   0.0014129505884026    8.055380456145
# 0.02    6.766113649781e-05    0.0022068548449798   -7.7378187535141
# 0.025   7.8981810558437e-05   0.0023214147792709    7.7836427272305

# Note that the theoretical first frequency of the beam using Euler-Bernoulli theory is:
# f1 = 1/(2 pi) * (3.5156/L^2) * sqrt(EI/rho) = 4.9455

# This implies that the corresponding time period of this beam (under Euler-Bernoulli assumption) is 0.2022s.

# This shows that Euler-Bernoulli beam theory under-predicts the time period of a thick beam. In other words, the Euler-Bernoulli beam theory predicts a more compliant beam than reality for a thick beam.

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmin = 0.0
  xmax = 4.0
  displacements = 'disp_x disp_y disp_z'
[]

[AuxVariables]
  [./youngs_modulus1]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./poisson_ratio1]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./shear_coefficient1]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[BCs]
  [./fixx1]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./fixy1]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  [../]
  [./fixz1]
    type = DirichletBC
    variable = disp_z
    boundary = left
    value = 0.0
  [../]
  [./fixr1]
    type = DirichletBC
    variable = rot_x
    boundary = left
    value = 0.0
  [../]
  [./fixr2]
    type = DirichletBC
    variable = rot_y
    boundary = left
    value = 0.0
  [../]
  [./fixr3]
    type = DirichletBC
    variable = rot_z
    boundary = left
    value = 0.0
  [../]
  [./app_acc]
    type = PresetAcceleration
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    function = accel_y
    boundary = right
  [../]
[]

[NodalKernels]
  [./force_y2]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = right
    function = force
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
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = 'timestep_end'
  [../]
  [./vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = 'timestep_end'
  [../]
  [./youngs_modulus1]
    type = ConstantAux
    variable = youngs_modulus1
    value = 1e04
  [../]
  [./poisson_ratio1]
    type = ConstantAux
    variable = poisson_ratio1
    value = 0.3
  [../]
  [./shear_coefficient1]
    type = ConstantAux
    variable = shear_coefficient1
    value = 1
  [../]
[]

[Functions]
  # [./force]
  #   type = PiecewiseLinear
  #   x = '0.0 0.1 0.2 10.0'
  #   y = '0.0 1e-2  0.0  0.0'
  # [../]
  [./force]
    type = ConstantFunction
    value = 0
  [../]
  [./accel_y]
    type = PiecewiseLinear
    data_file = 'GM_00.csv'
    format = columns
    scale_factor = 1.0
    xy_in_file_only = false
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
  solve_type = 'PJFNK'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  end_time = 48.0
  dt = 0.01
  dtmin = 0.01
  nl_abs_tol = 1e-3
    nl_rel_tol = 1e-3
  l_tol = 1e-3
  l_max_its = 20
  timestep_tolerance = 1e-3
[]

[Modules/TensorMechanics/LineElementMaster]
  [./all]
    add_variables = true
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'

    # Geometry parameters
    area = 0.01
    Iy = 1e-4
    Iz = 1e-4
    y_orientation = '0.0 1.0 0.0'

    # dynamic simulation using consistent mass/inertia matrix
    dynamic_nodal_translational_inertia = true
    nodal_mass = 18.99772

    dynamic_nodal_rotational_inertia = true
    nodal_Ixx = 2e-1
    nodal_Iyy = 1e-1
    nodal_Izz = 1e-1

    velocities = 'vel_x vel_y vel_z'
    accelerations = 'accel_x accel_y accel_z'
    rotational_velocities = 'rot_vel_x rot_vel_y rot_vel_z'
    rotational_accelerations = 'rot_accel_x rot_accel_y rot_accel_z'

    beta = 0.25 # Newmark time integration parameter
    gamma = 0.5 # Newmark time integration parameter

    boundary = right # Node set where nodal mass and nodal inertia are applied

    # parameters for 5% Rayleigh damping
    zeta = zeta1 # stiffness proportional damping
    eta = eta1 # Mass proportional Rayleigh damping
  [../]
[]

[Materials]
  [./elasticity]
    type = ComputeElasticityBeam
    youngs_modulus = youngs_modulus1
    poissons_ratio = poisson_ratio1
    shear_coefficient = shear_coefficient1
  [../]
  [./stress]
    type = ComputeBeamResultants
  [../]
[]

[Controls]
  [./stochastic]
    type = SamplerReceiver
  [../]
[]

[Postprocessors]
  [./disp_y]
    type = PointValue
    point = '4.0 0.0 0.0'
    variable = disp_y
  [../]
  [./vel_y]
    type = PointValue
    point = '4.0 0.0 0.0'
    variable = vel_y
  [../]
  [./accel_y]
    type = PointValue
    point = '4.0 0.0 0.0'
    variable = accel_y
  [../]
[]

[Outputs]
  file_base = 'Outputs1'
  exodus = true
  csv = true

  perf_graph = true
[]
