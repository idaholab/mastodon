#@requirement F6.4
# Test for application of seismic force which converts ground velocity in z directions into normal stress.

# This test consists of 20 elements for algorithm testing purposes (use 200 elements and dt of 0.005s to get a much cleaner wave).

# Non-reflecting boundary condition is applied on the back surface (z=0) to simulate an infinitely long soil column.

# Seismic stress proportional to a input velocity in z direction is applied to the front surface (z=1).

# The nodes on the sides are constrained to move together to using periodic boundary.

# Since an infinite soil column is being modeled using the non-reflecting boundary condition, there is no wave reflection from z = 0 boundary.

#Result : The velocity at the front surface should be same as the input velocity provided to the Seismic Force boundary condition.

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 4
  ny = 4
  nz = 40
  xmin = 0.0
  xmax = 25.0
  ymin = 0.0
  ymax = 25.0
  zmin = 0.0
  zmax = 5000.0
[]

[Controls]
   [./period0]
      type = TimePeriod
      disable_objects = 'Kernels/inertia_x Kernels/inertia_y Kernels/inertia_z AuxKernels/accel_x AuxKernels/accel_y AuxKernels/accel_z AuxKernels/vel_x AuxKernels/vel_y AuxKernels/vel_z'
      start_time = 0.0
      end_time = 0.00125 # same as dt used in the analysis
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
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
    stiffness_damping_coefficient = 0.000781
    static_initialization = True
    use_displaced_mesh = True
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    eta = 0.64026
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = True
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    eta = 0.64026
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = True
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    eta = 0.64026
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = True
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
  [./layer_id]
     type = UniformLayerAuxKernel
     variable = layer_id
     interfaces = '5000'
     direction = '0 0 1'
     execute_on = initial
  [../]
[]

[Materials]
   [./elasticity_tensor]
      type = ComputeIsotropicElasticityTensorSoil
      block = 0
      layer_variable = layer_id
      layer_ids = 0
      poissons_ratio = 0.33
      density = 4.9419e-3
      shear_modulus = 68322.
   [../]
   [./stress1]
     #Computes the stress, using linear elasticity
     type = ComputeLinearElasticStress
     block = 0
    [../]
    [./strain1]
      #Computes the strain, assuming small strains
      type = ComputeSmallStrain
      block = 0
      displacements = 'disp_x disp_y disp_z'
  [../]
[]

[BCs]
  [./SeismicForce]
    [./top_stress]
      displacements = 'disp_x disp_y disp_z'
      input_components = '2'
      boundary = 'front'
      velocity_functions = 'z_vel'
      shear_wave_speed = 3718.204815052883833
      p_wave_speed = 7381.527628151251754
      density = 4.9419e-3
    [../]
  [../]
  [./NonReflectingBC]
    [./back]
      displacements = 'disp_x disp_y disp_z'
      velocities = 'vel_x vel_y vel_z'
      accelerations = 'accel_x accel_y accel_z'
      beta = 0.25
      gamma = 0.5
      boundary = 'back'
      shear_wave_speed = 3718.204815052883833
      p_wave_speed = 7381.527628151251754
      density = 4.9419e-3
    [../]
  [../]
  [./Periodic]
    [./x_dir]
      primary = 'left'
      secondary = 'right'
      translation = '25.0 0.0 0.0'
    [../]
    [./y_dir]
      primary = 'bottom'
      secondary = 'top'
      translation = '0.0 25.0 0.0'
    [../]
  [../]
[]

[Functions]
  [./z_vel]
    type = PiecewiseLinear
    data_file = 'vel_wav_00125.csv'
    format = columns
    scale_factor = -10.0
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-10
  l_tol = 1e-12
  start_time = 0
  end_time = 2.6
  dt = 0.1
  timestep_tolerance = 1e-6
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  line_search = 'none'
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_top_x]
    type = PointValue
    point = '0 0 5000'
    variable = disp_x
  [../]
  [./disp_top_y]
    type = PointValue
    point = '0 0 5000'
    variable = disp_y
  [../]
  [./disp_top_z]
    type = PointValue
    point = '0 0 5000'
    variable = disp_z
  [../]
  [./vel_top_x]
    type = PointValue
    point = '0 0 5000'
    variable = vel_x
  [../]
  [./vel_top_y]
    type = PointValue
    point = '0 0 5000'
    variable = vel_y
  [../]
  [./vel_top_z]
    type = PointValue
    point = '0 0 5000'
    variable = vel_z
  [../]
  [./accel_top_x]
    type = PointValue
    point = '0 0 5000'
    variable = accel_x
  [../]
  [./accel_top_y]
    type = PointValue
    point = '0 0 5000'
    variable = accel_y
  [../]
  [./accel_top_z]
    type = PointValue
    point = '0 0 5000'
    variable = accel_z
  [../]
[]

[Outputs]
   exodus = true
   csv = true
   perf_graph = true
   [./screen]
     type = Console
     max_rows = 1
   [../]
[]
