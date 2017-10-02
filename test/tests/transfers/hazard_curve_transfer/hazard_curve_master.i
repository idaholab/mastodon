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
    number_of_bins = 2
    ground_motions = motions
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
  kernel_coverage_check = false
[]

[MultiApps]
  [./run_hazard]
    type = HazardCurveMultiApp
    hazard = hazard
    input_files = 'hazard_curve_sub.i'
    execute_on = 'initial timestep_end'
  [../]
[]

[Transfers]
  [./transfer]
    type = HazardCurveTransfer
    multi_app = run_hazard
    function = func
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1
[]
