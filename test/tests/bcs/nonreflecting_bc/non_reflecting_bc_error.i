# This input file will not run and should not, it is used for error checking.

[Mesh]
  type = GeneratedMesh
  dim = 2
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[BCs]
 [./test]
   type = NonReflectingBC
   displacements = 'disp_x disp_y'
   accelerations = 'disp_x disp_y'
   velocities = 'disp_x disp_y'
   variable = 'disp_x'
   component = 0
   boundary = 'left'
   beta = 1
   gamma = 1
 [../]
[]

[Problem]
  kernel_coverage_check = false
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  num_steps = 1
[]

[Outputs]
[]
