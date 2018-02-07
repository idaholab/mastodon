#@requirement F4.2
# One element test to test the user-defined  backbone curve.
# The back surface of the element (z=0) is fixed and the front surface (z=1)
# is moved by applying a cyclic preset displacement.

# The resulting shear stress-strain curve is same as the one provided as input.

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
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./layers]
    type = UniformLayerAuxKernel
    variable = layer_id
    interfaces = '2.0'
    direction = '0 0 1'
    execute_on = initial
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  [../]
[]

[BCs]
  [./x_bot]
    type = PresetBC
    variable = disp_x
    boundary = back
    value = 0.0
  [../]
  [./y_bot]
    type = PresetBC
    variable = disp_y
    boundary = back
    value = 0.0
  [../]
  [./z_bot]
    type = PresetBC
    variable = disp_z
    boundary = back
    value = 0.0
  [../]
  [./top_x]
    type = FunctionPresetBC
    boundary = front
    variable = disp_z
    function = top_disp
  [../]
[]

[Functions]
  [./top_disp]
    type = PiecewiseLinear
#    data_file = Displacement2.csv
#    format = columns
    x = '0.0 2.0'
    y = '0.0 1.0e-1'
  [../]
[]

[Materials]
  [./I_Soil]
    [./soil_1]
      layer_variable = layer_id
      layer_ids = '0'
      soil_type = user_defined
      backbone_curve_files = 'test_backbone.csv'
      poissons_ratio = '0.3'
      block = 0
      density = '2000'
      tangent_formulation = 3
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
  solve_type = NEWTON
  nl_abs_tol = 1e-11
  nl_rel_tol = 1e-11
  start_time = 0
  end_time = 2
  nl_max_its = 2
  dt = 1.0
  dtmin = 1.0
  timestep_tolerance = 1e-6
  petsc_options = '-snes_check_jacobian'
#  petsc_options = '-snes_ksp_ew'
#  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
#  petsc_options_value = '201                hypre    boomeramg      4'
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
[]

[Outputs]
  exodus = true
  csv = true
  print_perf_log = false
[]
