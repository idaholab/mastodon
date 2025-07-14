# @requirement F1.2
# HHT time integration test

# In this one element test, the base and top of the 2D element is fixed in y
# direction. An acceleration is prescribed at the base of the element in the x direction.

# HHT time integration is used to obtain the displacement, velocity and acceleration
# at the top of the element.

# The equation of motion is

# [M] accel_x(t+ delta t) + [K]((1+alpha)disp_x(t+deltat) - alpha disp_x(t)) = 0

# The row of the mass matrix that corresponds to the top node is density * area
# * length * [1/3 1/6]. The row of the stiffness matrix that corresponds to the
# top node is Youngs's modulus * Area * l * [1 -1]. The accel_x and disp_x in
# above equation are column vectors which contain the x acceleration and
# displacement of the top and bottom node, respectively.

# Solving the above equation analytically for alpha = -0.3, beta = 0.4225 and
# gamma = 0.8 gives:

#     t        disp_x     vel_x       accel_x
#     0          0          0           0
#     1          0.25       0.473       0.591
#     2          0.871      0.785       0.241
#     3          1.467      0.439      -0.493

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
[]

[Kernels]
  [./DynamicTensorMechanics]
     displacements = 'disp_x disp_y'
     hht_alpha = -0.3
  [../]
  [./interia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
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
[]

[BCs]
  [./fixed_base_x]
    type = DirichletBC
    value = 0.0
    variable = disp_x
    boundary = bottom
  [../]
  [./fixed_base_y]
    type = DirichletBC
    value = 0.0
    variable = disp_y
    boundary = bottom
  [../]
  [./top_x]
    type = PresetAcceleration
    variable = disp_x
    boundary = top
    function = acceleration_top
    beta = 0.25
    acceleration = accel_x
    velocity = vel_x
  [../]
  [./top_y]
    type = DirichletBC
    variable = disp_y
    boundary = top
    value = 0.0
  [../]
[]

[Functions]
  [./acceleration_top]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0'
    y = '0.0 1.0 0.0 -1.0'
  [../]
[../]

[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    poissons_ratio = 0.3
    youngs_modulus = 1e9
    block = 0
  [../]
  [./strain]
    type = ComputeIncrementalSmallStrain
    displacements = 'disp_x disp_y'
    block = 0
  [../]
  [./stress]
    type = ComputeFiniteStrainElasticStress
    block = 0
  [../]
  [./density]
    type = GenericConstantMaterial
    prop_names = 'density'
    prop_values = '2000.0'
  [../]
[../]

[Executioner]
  type = Transient
  start_time = 0.0
  end_time = 3.0
  dt = 1.0
[]

[Postprocessors]
  [./disp_x]
    type = PointValue
    point = '1.0 1.0 0.0'
    variable = disp_x
  [../]
  [./vel_x]
    type = PointValue
    point = '1.0 1.0 0.0'
    variable = vel_x
  [../]
  [./accel_x]
    type = PointValue
    point = '1.0 1.0 0.0'
    variable = accel_x
  [../]
[]

[Outputs]
  csv = true
  exodus = true
[]
