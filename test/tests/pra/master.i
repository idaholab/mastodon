# Test to demonstrate the Seismic Probabilistic Risk Assessment (SPRA)
# infrastructure in MASTODON. This test involves three input files:
#
# 1. master.i - The Master file that bins the hazard curve and scales ground
# motions for each bin for use in probabilistic simulations.
#
# 2. sub.i - The file that obtains the scaled ground motions from master.i and
# transfers these ground motions as inputs to the finite-element model and also
# contains the parameters for probabilistic simulation. This file acts as the
# sub file for master.i and master file for subsub.i.
#
# 3. subsub.i - The file that contains the finite-element model.

[Mesh]
  # dummy mesh
  type = GeneratedMesh
  dim = 3
[]

[Variables]
  # dummy variables
  [./u]
  [../]
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[UserObjects]
  [./motions]
    # Reads ground motion files for input
    type = GroundMotionReader
    pattern = 'gm_data/ground_motion_*.csv'
  [../]
  [./hazard]
    # Bins hazard curves into number_of_bins and scales ground motions
    type = HazardCurve
    filename = 'gm_data/hazard.csv'
    number_of_bins = 2
    ground_motions = motions
    reference_acceleration = 0.04
  [../]
[]

[MultiApps]
  [./run_hazard]
    type = HazardCurveMultiApp
    hazard = hazard
    input_files = 'sub.i'
    execute_on = 'initial timestep_end'
  [../]
[]

[Transfers]
  [./transfer_x]
    # Transfers scaled ground motions to multiapp
    type = HazardCurveTransfer
    multi_app = run_hazard
    function = accel_bottom_x # Function in sub.i that receives the scaled ground motions
    component = x # component that is being transferred
  [../]
[]

[Executioner]
  # governs the PRA execution in case of conflict with sub.i or subsub.i
  type = Transient
  end_time = 0.05
  dt = 0.005
[]

[Outputs]
  csv = true
[]
