# Test for seismic source applied at multiple points with all the points rupturing at the same time.

# A 10m x 10m soil domain is considered for this test and it is meshed using 100 QUAD4 elements.

# 7 earthquake sources are placed along an earthquake fault that has a 45 degrees dip starting at
# (2m, 8m) and ending at (8m, 2m). The strike angle for this fault is 0.0 degrees and the rake angle
# is 90 degrees implying that the slip is parallel to the fault.

# The slip is a triangular pulse in time.

# Since all the earthquake sources rupture at the same time, it should generate a 45 degree inclined
# plane wave. Note that dynamic effects, a much finer mesh and smaller time step is required to
# visualize the propagation of the inclined wave. This test just ensures that the sources are placed
# at the correct location and that they rupture at the same time.

[Mesh]
  type = GeneratedMesh
  nx = 10
  ny = 10
  dim = 2
  xmin = 0.0
  xmax = 10.0
  ymin = 0.0
  ymax = 10.0
[]


[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y'
  [../]
[]

[DiracKernels]
  [./SeismicSource]
    [./sync_source]
      displacements = 'disp_x disp_y'
      position_function = 'x_pos y_pos'
      number = 7 # number of point sources
      slip = y_force
      strike = 0.0 # strike angle =0, x aligned with North
      dip = 45.0 # dip angle = pi/4, angle fault makes with horizontal
      rake = 90.0 # rake angle = pi/2, gives slip direction
      shear_modulus = 100.0
      area = 1.0
    [../]
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensor
    block = 0
    youngs_modulus = 2e4
    poissons_ratio = 0.4
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

  [./density_1]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'density'
    prop_values = '1.'
  [../]

[]

[BCs]
  [./x_fixed]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right bottom top'
    value = 0.0
  [../]
  [./y_fixed]
    type = DirichletBC
    variable = disp_y
    boundary = 'left right bottom top'
    value = 0.0
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  start_time = 0
  end_time = 5.0
  l_tol = 1e-10
  nl_abs_tol = 1e-10
  dt = 1.0
  timestep_tolerance = 1e-12
[]

[Functions]
  [./y_force]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 0.0 0.0 0.0 0.0'
  [../]
  [./x_pos]
    type = PiecewiseLinear
    data_file = 'x_pos.csv'
    format = columns
  [../]
  [./y_pos]
    type = PiecewiseLinear
    data_file = 'y_pos.csv'
    format = columns
  [../]
[]

[Outputs]
  exodus = true
[]
