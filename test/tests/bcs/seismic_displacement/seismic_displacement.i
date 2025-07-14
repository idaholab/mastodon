# Test for within soil seismic input. Test for outcrop seismic input is
# with non reflecting boundary condition

# This test consists of one brick element.

# A sawtooth displacement history is applied as seismic input to the bottom surface.

# Result: The result at the bottom should be same as the input displacement.

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 1
  ny = 1
  xmin = 0.0
  xmax = 1.0
  ymin = 0.0
  ymax = 1.0
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
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
  [./layer]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[ICs]
  [./layer_ic]
    type = FunctionIC
    variable = layer
    function = layer_y
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y'
    alpha = -0.3
  [../]
  [./inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.4225
    gamma = 0.8
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.4225
    gamma = 0.8
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.4225
    execute_on = timestep_end
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.8
    execute_on = timestep_end
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.4225
    execute_on = timestep_end
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.8
    execute_on = timestep_end
  [../]
[]


[BCs]
  [./SeismicDisplacement]
    [./top_x]
      displacements = 'disp_x disp_y'
      velocities = 'vel_x vel_y'
      accelerations = 'accel_x accel_y'
      input_components = '0 1'
      input_functions = 'x_disp y_disp' #input displacements
      boundary = bottom
      beta = 0.4225
    [../]
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensorSoil
    block = 0
    layer_variable = layer
    layer_ids = '10'
    shear_modulus = '71.42857'
    poissons_ratio = '0.4'
    density = '1.0'
  [../]

  [./strain_1]
    type = ComputeSmallStrain
    block = 0
    displacements = 'disp_x disp_y'
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
  l_tol = 1e-12
  nl_abs_tol = 1e-15
  nl_rel_tol = 1e-12
  dt = 0.01
  timestep_tolerance = 1e-12
[]

[Functions]
  [./x_disp]
    type = PiecewiseLinear
    x = '0.0 0.25 0.5 0.75 1.0 1.25 1.5 2.0 3.0'
    y = '0.0 0.0  0.5 0.0 -0.5 0.0  0.0 0.0 0.0'
  [../]
  [./y_disp]
    type = PiecewiseLinear
    x = '0.0 0.25 0.5 0.75 1.0 1.25 1.5 2.0 3.0'
    y = '0.0 0.0  0.5 0.0 -0.5 0.0  0.0 0.0 0.0'
  [../]
  [./layer_y]
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
    nodeid = 0
    variable = disp_x
  [../]
  [./disp_2]
    type = NodalVariableValue
    nodeid = 0
    variable = disp_y
  [../]
[]

[Outputs]
  exodus = true
  csv = true
  print_linear_residuals = true
  perf_graph = true
[]
