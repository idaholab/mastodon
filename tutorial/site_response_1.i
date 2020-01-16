[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  type = GeneratedMesh
  nx = 1
  ny = 1
  nz = 100
  xmin = -0.5
  ymin = -0.5
  zmin = 0.0
  xmax = 0.5
  ymax = 0.5
  zmax = 100.0
  dim = 3
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
  [./layer_id]
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    # displacements = 'disp_x disp_y disp_z'
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
  [./layer_id]
     type = UniformLayerAuxKernel
     variable = layer_id
     interfaces = '20 60 100.1'
     direction = '0.0 0.0 1.0'
     execute_on = initial
  [../]
[]


[BCs]
  [./bottom_z]
    type = PresetBC
    variable = disp_z
    boundary = 'back'
    value = 0.0
  [../]
  [./bottom_y]
    type = PresetBC
    variable = disp_y
    boundary = 'back'
    value = 0.0
  [../]
  [./bottom_accel]
    type = PresetAcceleration
    boundary = 'back'
    function = accel_bottom
    variable = 'disp_x'
    beta = 0.25
    acceleration = 'accel_x'
    velocity = 'vel_x'
  [../]
  [./Periodic]
    [./y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'bottom'
      secondary = 'top'
      translation = '0.0 1.0 0.0'
    [../]
    [./x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 'left'
      secondary = 'right'
      translation = '1.0 0.0 0.0'
    [../]
  [../]
[]

[Functions]
  [./accel_bottom]
     type = PiecewiseLinear
     data_file = 'acceleration_hist.csv'
     format = 'columns'
  [../]
[]

[Materials]
  [./linear_soil]
    type = ComputeIsotropicElasticityTensorSoil
    layer_variable = layer_id
    layer_ids = '0 1 2'
    shear_modulus = '3e6 2e6 1e6'
    poissons_ratio = '0.2 0.2 0.2'
    density = '1200 1000 800'
  [../]
  [./strain]
    type = ComputeSmallStrain
    # displacements = 'disp_x disp_y disp_z'
  [../]
  [./stress]
    type = ComputeLinearElasticStress
  [../]
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  start_time = 0
  end_time = 4
  dt = 0.01
  timestep_tolerance = 1e-6
[]

[Postprocessors]
  [top_ax_pv]
    type = PointValue
    point = '0 0 100'
    variable = 'accel_x'
  []
  [./top_ax_nvv]
    type = NodalVariableValue
    variable = accel_x
    nodeid = 401
  [../]
[]

[VectorPostprocessors]
  [./accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_x accel_y accel_z'
    nodes = '401'
  [../]
  [./accel_spec]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist
    regularize_dt = 0.01
    # outputs = out
  [../]
[]


[Outputs]
  csv = true
  exodus = true
[]
