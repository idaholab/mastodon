# One element model to test Mastodon Block Actions. Periodic boundary conditions are
# applied and an acceleration is prescribed at the bottom surface.

# UPDATE THIS FILE WITH MASTODON BLOCK ACTIONS AS THEY ARE IMPLEMENTED

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

[Mastodon]
  [./Model]
    dynamic_analysis = true
  [../]
  [./Outputs]
    stress_strain_output = true
  [../]
[]

[AuxVariables]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
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
    type = PresetAcceleration
    boundary = 0
    function = accel_bottom_y
    variable = disp_y
    beta = 0.25
    acceleration = accel_y
    velocity = vel_y
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
  [./accel_bottom_x]
    type = PiecewiseLinear
    data_file = 'accel.csv'
    format = columns
    scale_factor = 1.0
  [../]
  [./accel_bottom_y]
    type = PiecewiseLinear
    data_file = 'accel.csv'
    format = columns
    scale_factor = 2.0
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensorSoil
    block = 0
    layer_variable = layer_id
    layer_ids = '0'
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

[Preconditioning]
  [./andy]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  nl_abs_tol = 1e-11
  nl_rel_tol = 1e-11
  start_time = 32
  end_time = 33
  dt = 0.01
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
[]

[Outputs]
  exodus = true
  csv = true
  perf_graph = false
[]
