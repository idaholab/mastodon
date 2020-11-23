# Test to demonstrate the Seismic Probabilistic Risk Assessment (SPRA)
# infrastructure in MASTODON. This test involves three input files:
#
# 1. master.i - The Master file that bins the hazard curve and scales ground
# motions for each bin for use in probabilistic simulations.
#
# 2. sub.i - The file that obtains the scaled ground motions from master.i and
# transfers these ground motions as inputs to the finite-element model and also
# contains the parameters for probabilistic simulation. This file acts as the
# sub file for master.i and master file for subsub.i.
#
# 3. subsub.i - The file that contains the finite-element model.

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

[Controls]
  # needed for probabilistic simulations
  [./stochastic]
   type = SamplerReceiver
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
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
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
    use_displaced_mesh = false
  [../]
  [./inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
    use_displaced_mesh = false
  [../]
  [./inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
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
    interfaces = '2.0'
    direction = '0 0 1'
    execute_on = initial
  [../]
[]

[BCs]
  [./x_bot]
    type = PresetAcceleration
    boundary = 0
    function = accel_bottom_x
    variable = disp_x
    beta = 0.25
    acceleration = accel_x
    velocity = vel_x
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
[]

[Functions]
  # PiecewiseLinear function that is receiving scaled GMs from sub.i. Inputs here are dummy.
  [./accel_bottom_x]
    type = PiecewiseLinear
    data_file = 'gm_data/ground_motion_00.csv'
    format = columns
    scale_factor = 1.0
    xy_in_file_only = false
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensorSoil
    block = 0
    layer_variable = layer_id
    layer_ids = '0'
    poissons_ratio = '0.3'
    density = '2000.0'
    shear_modulus = '1.2e5'
    scale_factor_density = 1.0
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

[Preconditioning]
  [./andy]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  end_time = 0.05
  dt = 0.005
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
  [./accel_2x]
    type = NodalVariableValue
    nodeid = 2
    variable = accel_x
  [../]
  [./accel_2y]
    type = NodalVariableValue
    nodeid = 2
    variable = accel_y
  [../]
  [./accel_2z]
    type = NodalVariableValue
    nodeid = 2
    variable = accel_z
  [../]
[]

[Outputs]
  csv = true
[]
