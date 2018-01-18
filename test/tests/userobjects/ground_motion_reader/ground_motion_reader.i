[Mesh]
  type = GeneratedMesh
  dim = 1
[]

[Variables]
  [./u]
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
  kernel_coverage_check = false
[]

[Executioner]
  type = Steady
[]

[UserObjects]
  [./motions]
    type = GroundMotionReader
    pattern = '../../data/ground_motion_*.csv'
  [../]
[]

[Outputs]
[]
