# This is a test for Fragility VectorPostprocessor. This input file is set to
# run for one time step with dummy mesh and variables.
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

[Executioner]
  type = Transient
  num_steps = 1
[]

[VectorPostprocessors]
  [./fragility_pump]
    type = Fragility
    master_file = 'master' # name of the master file in this case is 'master.i'
    hazard_multiapp = 'run_hazard'
    probabilistic_multiapp = 'sub'
    num_gms = 3
    demand_variable = 'accel_2x'
    frequency = 4
    damping_ratio = 0.05
    dt = 0.005
    median_capacity = 0.01
    beta_capacity = 1.3
    num_samples = 3
    num_bins = 4
    im_values = '0.3 0.5 0.7 0.9'
    median_fragility_limits = '3.0 8.0'
    beta_fragility_limits = '1.0 3.0'
    optimization_method = 'CRUDE'
    execute_on = TIMESTEP_END
  [../]
[]

[Outputs]
  csv = true
  execute_on = 'final'
  perf_graph = 'true'
[]
