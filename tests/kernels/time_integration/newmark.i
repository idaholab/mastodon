# @requirement F1.1
#
# Newmark time integration test
#
# In this one element test, the base of the 2D element is fixed in both x and y
# directions. The top of the block is also fixed in the y direction and a sawtooth
# acceleration is applied in the x direction.

# The prescribed acceleration is integrated in time using newmark time integration.
# For beta = 0.25 and gamma = 0.5 and timestep of 1.0 s, the displacement and velocity
# are as given below.

#     t        disp_x     vel_x     accel_x
#     0          0          0         0
#     1          0.25       0.5       1
#     2          1          1         0
#     3          1.75       0.5      -1

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
  [./TensorMechanics]
     displacements = 'disp_x disp_y'
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
