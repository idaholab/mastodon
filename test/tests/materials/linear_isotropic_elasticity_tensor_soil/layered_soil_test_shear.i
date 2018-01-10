#@requirement F4.1
# Test for layered soil material using traditional approach.

# In this test, a 1x1 soil domain is split into 9 elements (3x3 mesh).
# The bottom edge of this element is fixed and the top edge is pulled by
# applying nodal forces in the +y direction. The soil domain is made of 3
# different soil layers distributed in the y direction.

# Result: The bottom nodes remain fixed. The other nodes have a x and y displacement which
# increases with +y.

# At x = -0.5, y = -0.5, displacement: 0,0
# At x = -0.5, y = -0.1667, displacement: 0.019364, 0.035915
# At x = -0.5, y = 0.1667, displacement:0.035965, 0.095781
# At x = -0.5, y = 0.5, displacement: 0.096357, 0.24403

# The above result was obtained by running a similar model in which three different blocks
# were constructed for each soil material and a different input block was written for each
# soil material.

[Mesh]
  type = GeneratedMesh
  nx = 3
  ny = 3
  dim = 2
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
[]


[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[Kernels]
  [./TensorMechanics]
    displacements = 'disp_x disp_y'
  [../]
[]

[AuxVariables]
  [./layer]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[ICs]
  [./layer_ic]
    type = FunctionIC
    variable = layer
    function = layers_y
  [../]
[]

[Functions]
  [./layers_y]
    type = PiecewiseConstant
    x = '-0.1667, 0.1667, 0.5'
    y = ' 30,   20,      10'
    axis=Y
    direction = right
  [../]
[]

[NodalKernels]
  [./top_y]
    variable = disp_y
    type = ConstantRate
    rate = 20.0
    boundary = top
  [../]
[]

[BCs]
  [./bottom_x]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0.0
  [../]
  [./bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensorSoil
    block = 0
    layer_variable = layer
    layer_ids = '10 20 30'
    poissons_ratio = '0.3 0.3 0.3'
    density = '2.0 2.0 2.0'
    shear_modulus = '100.0 200.0 300.0'
  [../]

  [./strain_1]
    type = ComputeSmallStrain
    block = 0
    displacements = 'disp_x disp_y'
  [../]

  [./stress_1]
    type = ComputeLinearElasticStress
    store_stress_old = true
    block = 0
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  print_linear_residuals = true
  print_perf_log = true
  [./console]
    type = Console
    perf_log = true
    output_linear = true
  [../]
[]
