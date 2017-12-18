[Mesh]
  type = GeneratedMesh
  dim = 2
[]

[Variables]
  [./u]
  [../]
[]

[UserObjects]
  [./motions]
    type = GroundMotionReader
    pattern = '../../data/ground_motion_*.csv'
  [../]
  [./hazard]
    type = HazardCurve
    filename = '../../data/hazard.csv'
    number_of_bins = 5
    ground_motions = motions
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
