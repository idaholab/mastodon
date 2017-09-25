#@requirement F7.1
# Test for seismic source applied at one point.

# This test consists of one brick element of size 1x1x1.

# An earthquake source (double couple) is applied at 0.25,0.25,0.25. The earthquake fault
# has 0 strike angle (i.e., x axis is aligned with geographic north). The dip angle is 45 degrees
# and the rake angle is 90 degrees. The slip (s) is a linearly increasing function of time.

# The forces resulting due to the earthquake source is distributed to the 8 nodes of the element.
# The expression for nodal forces for this particular fault configuration is
# f_x^n = 0.0
# f_y^n = -Mo*dphi_n/dy
# f_z^n = Mo*dphi_n/dz
# where f_x^n, f_y^n and f_z^n are the forces in x, y and z directions, respectively. Mo is the
# seismic moment = area*shear_modulus* slip and dphi_n/dy and dphi_n/dz are the derivatives of
# shape functions corresponding to node n in y and z directions, respectively, evaluated at the
# point (0.25, 0.25, 0.25).
#
# To check if forces are distributed as expected, we apply negative of these nodal forces to the
# nodal points using nodalkernels. The nodeset numbers used for applying nodal forces matches with the node
# numbers in the output exodus file.
# Result: Net force on the element is zero. So element is in equilibrium with zero displacements.

[Mesh]
  file = test_element_3d.e
[]


[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  [../]
[]

[DiracKernels]
  [./SeismicSource]
    [./one_source]
      displacements = 'disp_x disp_y disp_z'
      point = '0.25 0.25 0.25'
      slip = y_force
      strike = 0.0 # strike angle =0, x aligned with North
      dip = 45.0 # dip angle = pi/4, angle fault makes with horizontal
      rake = 90.0 # rake angle = pi/2, gives slip direction
      shear_modulus = 1.0
      area = 1.0
    [../]
  [../]
[]

[NodalKernels]
  [./force_1y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 1
    function = force_1y
  [../]
  [./force_2y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 2
    function = force_2y
  [../]
  [./force_3y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 3
    function = force_3y
  [../]
  [./force_4y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 4
    function = force_4y
  [../]
  [./force_5y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 5
    function = force_5y
  [../]
  [./force_6y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 6
    function = force_6y
  [../]
  [./force_7y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 7
    function = force_7y
  [../]
  [./force_8y]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 8
    function = force_8y
  [../]
  [./force_1z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 1
    function = force_1z
  [../]
  [./force_2z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 2
    function = force_2z
  [../]
  [./force_3z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 3
    function = force_3z
  [../]
  [./force_4z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 4
    function = force_4z
  [../]
  [./force_5z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 5
    function = force_5z
  [../]
  [./force_6z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 6
    function = force_6z
  [../]
  [./force_7z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 7
    function = force_7z
  [../]
  [./force_8z]
    type = UserForcingFunctionNodalKernel
    variable = disp_z
    boundary = 8
    function = force_8z
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensor
    block = 1
    youngs_modulus = 2.e+2
    poissons_ratio = 0.4
  [../]

  [./strain_1]
    type = ComputeSmallStrain
    block = 1
    displacements = 'disp_x disp_y disp_z'
  [../]

  [./stress_1]
    type = ComputeLinearElasticStress
    store_stress_old = true
    block = 1
  [../]

  [./density_1]
    type = GenericConstantMaterial
    block = 1
    prop_names = 'density'
    prop_values = '1.'
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
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
  [../]
  [./force_1y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.1875
  [../]
  [./force_2y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.0625
  [../]
  [./force_3y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.0625
  [../]
  [./force_4y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.1875
  [../]
  [./force_5y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.5625
  [../]
  [./force_6y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.1875
  [../]
  [./force_7y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.1875
  [../]
  [./force_8y]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.5625
  [../]
  [./force_1z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.0625
  [../]
  [./force_2z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.0625
  [../]
  [./force_3z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.1875
  [../]
  [./force_4z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.1875
  [../]
  [./force_5z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.1875
  [../]
  [./force_6z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.1875
  [../]
  [./force_7z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = 0.5625
  [../]
  [./force_8z]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
    scale_factor = -0.5625
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
  print_perf_log = true
[]
