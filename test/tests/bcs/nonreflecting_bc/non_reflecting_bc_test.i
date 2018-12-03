#@requirement F6.2
# Test for non-reflecting boundary condition and application of seismic input

# This test consists of 20 brick elements stacked in the z direction.

# Back surface (z=0) is a non-reflecting boundary modeled using Lysmer damper.

# A sawtooth displacement of period 1 s is applied in the x direction to the front surface (z=1).

# The shear wave velocity of the soil material is 10 m/s. So the wave would take 0.1 second to reach from the front to back surface.

# The nodes on the sides are constrained to move together to periodic boundary.

# Result: The displacement at the back surface should almost be zero once the end of the wave has passed the back boundary (i.e., after 1.35 seconds in this case). This implies that most of the wave that that is incident on the boundary is absorbed by the boundary. Less than 1% of the incident wave is reflected back into the soil domain.

[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 20
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
 [./front]
   type = PresetDisplacement
   variable = disp_x
   function = x_vel
   boundary = front
   acceleration = accel_x
   velocity = vel_x
   beta = 0.25
 [../]
 [./NonReflectingBC]
    [./back]
      displacements = 'disp_x disp_y disp_z'
      velocities = 'vel_x vel_y vel_z'
      accelerations = 'accel_x accel_y accel_z'
      beta = 0.25
      gamma = 0.5
      boundary = 'back'
    [../]
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
    shear_modulus = '1.0e+2'
    poissons_ratio = '0.2'
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
  end_time = 3.0
  l_tol = 1e-6
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6
  dt = 0.01
  timestep_tolerance = 1e-12
[]

[Functions]
  [./x_vel]
    type = PiecewiseLinear
    x = '0.0 0.25 0.5 0.75 1.0 1.25 2.0 3.0'
    y = '0.0 0.0  0.5 0.0 -0.5  0.0 0.0 0.0'
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
    nodeid = 83 # z=1(input)
    variable = disp_x
  [../]
  [./disp_2]
    type = NodalVariableValue
    nodeid = 77 # z=0.95
    variable = disp_x
  [../]
  [./disp_3]
    type = NodalVariableValue
    nodeid = 0 # z=0 (location of non-reflecting boundary)
    variable = disp_x
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  print_linear_residuals = true
  perf_graph = true
[]
