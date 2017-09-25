[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 1
  ny = 1
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
  [./vel_y]
  [../]
[]

[BCs]
  [./front]
    type = SeismicForce
    variable = 'disp_x'
    boundary = 'left'
  [../]
[]

[Functions]
  [./func_x]
    type = ConstantFunction
    value = 1
  [../]
[]

[Problem]
  kernel_coverage_check = false
[]

[Executioner]
  type = Transient
  num_steps = 1
[]

[Outputs]
[]
