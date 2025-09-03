# Test for a protal frame installed with a diagonal Fluid Viscous Damper element
# The mesh for the geometry is imported from exodus file "fv_damper_frame.e"
# Beam-column elements are modeled as Euler-Bernouli beam elements

# Dimesnions
# columns
# E = 20e9, G = 100e9, A = 9e-2, Iy = Iz = 6.75e-4

# Beams
# E = 20e9, G = 100e9, A = 9e-2, Iy = Iz = 6.75e-1 (I_beam = 1000*I_Column to simulate rigid beam)

[Mesh]
  type = FileMesh
  file = 'fv_damper_frame.e'
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./rot_x]
    order = FIRST
    family = LAGRANGE
    block = '1 2'
  [../]
  [./rot_y]
    order = FIRST
    family = LAGRANGE
    block = '1 2'
  [../]
  [./rot_z]
    order = FIRST
    family = LAGRANGE
    block = '1 2'
  [../]
[]

[AuxVariables]
  [./vel_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./vel_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./vel_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./accel_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./accel_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./accel_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./reaction_x]
    block = '1 2 3'
  [../]
  [./reaction_y]
    block = '1 2 3'
  [../]
  [./reaction_z]
    block = '1 2 3'
  [../]
  [./reaction_xx]
    block = '1 2 3'
  [../]
  [./reaction_yy]
    block = '1 2 3'
  [../]
  [./reaction_zz]
    block = '1 2 3'
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
[]

[Kernels]
  [./damper_disp_x]
    type = StressDivergenceDamper
    block = '3'
    displacements = 'disp_x disp_y disp_z'
    component = 0
    variable = disp_x
    save_in = reaction_x
  [../]
  [./damper_disp_y]
    type = StressDivergenceDamper
    block = '3'
    displacements = 'disp_x disp_y disp_z'
    component = 1
    variable = disp_y
    save_in = reaction_y
  [../]
  [./damper_disp_z]
    type = StressDivergenceDamper
    block = '3'
    displacements = 'disp_x disp_y disp_z'
    component = 2
    variable = disp_z
    save_in = reaction_z
  [../]
[]

[NodalKernels]
  [./x_inertial]
    type = NodalTranslationalInertia
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    boundary = '2 3'
    beta = 0.25
    gamma = 0.5
    mass = 25000
    alpha = 0
    eta = 0
  [../]
  [./y_inertial]
    type = NodalTranslationalInertia
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    boundary = '2 3'
    beta = 0.25
    gamma = 0.5
    mass = 25000
    alpha = 0
    eta = 0
  [../]
  [./z_inertial]
    type = NodalTranslationalInertia
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    boundary = '2 3'
    beta = 0.25
    gamma = 0.5
    mass = 25000
    alpha = 0
    eta = 0
  [../]
[]

[Physics/SolidMechanics/LineElement/QuasiStatic]
  displacements = 'disp_x disp_y disp_z'
  rotations = 'rot_x rot_y rot_z'
  velocities = 'vel_x vel_y vel_z'
  accelerations = 'accel_x accel_y accel_z'
  rotational_velocities = 'rot_vel_x rot_vel_y rot_vel_z'
  rotational_accelerations = 'rot_accel_x rot_accel_y rot_accel_z'
  save_in = 'reaction_x reaction_y reaction_z'

  beta = 0.25 # Newmark time integration parameter
  gamma = 0.5 # Newmark time integration parameter

  # parameters for 5% Rayleigh damping
  zeta = 0  # stiffness proportional damping
  eta = 0   # Mass proportional Rayleigh damping

  [./block_1]
    block = 1
    area = 9e-2
    Iy = 6.75e-4
    Iz = 6.75e-4
    y_orientation = '0.0 0.0 1.0'
  [../]
  [./block_2]
    block = 2
    area = 9e-2
    Iy = 6.75e-1
    Iz = 6.75e-1
    y_orientation = '0.0 1.0 0.0'
  [../]
[]

[Materials]
  [./elasticity_beamcolumn]
    type = ComputeElasticityBeam
    youngs_modulus = 20e9
    poissons_ratio = -0.9
    shear_coefficient = 1
    block = '1 2'
  [../]
  [./stress_beam]
    type = ComputeBeamResultants
    block = '1 2'
  [../]
  [./elasticity_damper]
    type = ComputeFVDamperElasticity
    block = '3'
    y_orientation = '-0.5144 0.85749 0'
    displacements = 'disp_x disp_y disp_z'
    cd = 232764
    alpha = 0.35
    k = 25000000
    gamma = 0.5
    beta = 0.25
  [../]
[]

[BCs]
  [./base_accel_x]
    type = PresetAcceleration
    boundary = 1
    function = acceleration_x
    variable = disp_x
    beta = 0.25
    acceleration = 'accel_x'
    velocity = 'vel_x'
  [../]
  [./fix_y]
    type = DirichletBC
    variable = disp_y
    boundary = 1
    value = 0.0
  [../]
  [./fix_z]
    type = DirichletBC
    variable = disp_z
    boundary = 1
    value = 0.0
  [../]
  [./fix_rot_x]
    type = DirichletBC
    variable = rot_x
    boundary = 1
    value = 0.0
  [../]
  [./fix_rot_y]
    type = DirichletBC
    variable = rot_y
    boundary = 1
    value = 0.0
  [../]
  [./fix_rot_z]
    type = DirichletBC
    variable = rot_z
    boundary = 1
    value = 0.0
  [../]
[]

[Functions]
  [./acceleration_x]
    type = PiecewiseLinear
    data_file = acceleration.csv
    format = columns
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
  line_search = none
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-8
  start_time = 0
  end_time = 0.2
  dt = 0.0005
  dtmin = 0.000001
  timestep_tolerance = 1e-6
[]

[Postprocessors]
  # For basicdisp and basicforce postprocessors, divide the output values
  # by a scale factor equal to 'length of the damper element; (5.83 for this
  # problem). Becasue the object 'ElementIntegralMaterialProperty', calculates
  # the material property value integrating over the element length.
  [./basicdisp]
    type = ElementIntegralMaterialProperty
    mat_prop = basic_deformation
    block = '3'
  [../]
  [./basicforce]
    type = ElementIntegralMaterialProperty
    mat_prop = basic_force
    block = '3'
  [../]
  [./disp_bottom_right]
    type = PointValue
    variable = disp_x
    point = '5 0 0'
  [../]
  [./disp_top_right]
    type = PointValue
    variable = disp_x
    point = '5 3 0'
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  time_step_interval = 10
[]
