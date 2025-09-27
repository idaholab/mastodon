#@requirement F6.4

# Test for application of seismic force which converts ground velocity in x and y directions into shear stress.

# This test consists of 20 brick elements stacked in the z direction.

# Back surface (z=0) is fixed in all three coordinate directions.

# Seismic stress proportional to a 2D input velocity is applied to the front surface (z=1).

# The nodes on the sides are constrained to move together to using periodic boundary.

# The shear wave speed in the soil is 1 m/s. So it will take 1 second for a shear wave from the front surface to reach the back surface and 2 seconds for the wave reflected off the back surface to reach the front surface.

# Result: The velocity at the front surface should almost be same as input velocity in the x and y directions before 2 seconds (i.e., before the reflected shear wave reaches the front surface).

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 25
  xmin = 0.0
  ymin = 0.0
  zmin = 0.0
  xmax = 1.0
  ymax = 1.0
  zmax = 1.0
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
  [./layer]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[ICs]
  [./layer_ic]
    type = FunctionIC
    variable = layer
    function = layers
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
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


[BCs]
  [./SeismicForce]
    [./front]
      displacements = 'disp_x disp_y disp_z'
      input_components = '0 1'
      boundary = 'front'
      velocity_functions = 'x_vel y_vel' # input velocity functions
      shear_wave_speed = 1.0
      p_wave_speed = 3.5
      density = 1.0
    [../]
  [../]
  [./back_x]
    type = DirichletBC
    variable = disp_x
    value = 0.0
    boundary = back
  [../]
  [./back_y]
    type = DirichletBC
    variable = disp_y
    value = 0.0
    boundary = back
  [../]
  [./back_z]
    type = DirichletBC
    variable = disp_z
    value = 0.0
    boundary = back
  [../]
  [./Periodic]
    [./left_right]
      variable = 'disp_x disp_y disp_z'
      primary = 'left'
      secondary = 'right'
      translation = '1 0 0'
    [../]
    [./top_bottom]
      variable = 'disp_x disp_y disp_z'
      primary = 'bottom'
      secondary = 'top'
      translation = '0 1 0'
    [../]
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensorSoil
    block = 0
    layer_variable = layer
    layer_ids = '10'
    shear_modulus = '1.0'
    poissons_ratio = '0.3'
    density = '1.0'
  [../]

  [./strain_1]
    type = ComputeSmallStrain
    block = 0
    displacements = 'disp_x disp_y disp_z'
  [../]

  [./stress_1]
    type = ComputeLinearElasticStress
    block = 0
  [../]

[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  start_time = 0
  end_time = 2.0
  l_tol = 1e-10
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-12
  dt = 0.02
  timestep_tolerance = 1e-12
[]

[Functions]
  [./y_vel]
    type = PiecewiseLinear
    x = '0.0 0.25 0.5 0.75 1.0 1.25 1.5 2.0 3.0'
    y = '0.0 0.0  0.5 0.0 -0.5 0.0  0.0 0.0 0.0'
  [../]
  [./x_vel]
    type = ParsedFunction
    expression = 'if (t<=1.0, sin(pi*t), 0.0)'
  [../]
  [./layers]
    type = ConstantFunction
    value = 10
  [../]
[]

[Postprocessors]
  [./_dt]
    type = TimestepSize
  [../]
  [./disp_1]
    type = NodalVariableValue
    nodeid = 103
    variable = vel_x
  [../]
  [./disp_2]
    type = NodalVariableValue
    nodeid = 103
    variable = vel_y
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  print_linear_residuals = true
  perf_graph = true
[]
