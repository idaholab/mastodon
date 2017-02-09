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
  [./accel_x]
  [../]
  [./vel_y]
  [../]
  [./accel_y]
  [../]
[]

[BCs]
  [./SeismicDisplacement]
    [./top_x]
      displacements = 'disp_x disp_y disp_x'
      velocities = 'vel_x vel_y vel_x'
      accelerations = 'accel_x accel_y accel_x'
      input_components = '0 1 0'
      input_functions = 'x_disp y_disp x_disp' #input displacements
      boundary = bottom
      beta = 0.4225
    [../]
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1
[]

[Problem]
  kernel_coverage_check = false
[]

[Outputs]
[]
