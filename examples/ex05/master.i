# One element model to demonstrate PRA
# MASTERFILE
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
    pattern = '../gm_data/ground_motion_*.csv'
  [../]
  [./hazard]
    type = HazardCurve
    filename = '../gm_data/hazard.csv'
    number_of_bins = 5
    ground_motions = motions
    reference_acceleration = 0.4
  [../]
[]

[MultiApps]
  [./run_hazard]
    type = HazardCurveMultiApp
    hazard = hazard
    input_files = 'gm_sampler_sub.i'
    execute_on = 'initial timestep_end'
  [../]
[]

[Transfers]
  [./transfer]
    type = HazardCurveTransfer
    multi_app = run_hazard
    function = func
    component = z
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
  kernel_coverage_check = false
[]

[Executioner]
  type = Transient
  num_steps = 1
[]
