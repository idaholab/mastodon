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
    num_gms = 30
    demand_filename = 'stoc_demands_03_cpump.csv'
    median_capacity = 3.1
    beta_capacity = 0.5
    num_samples = 1
    num_bins = 6
    im_values = '0.33 0.65 0.98 1.30 1.63 1.95'
    median_fragility_limits = '0.1 10.0'
    beta_fragility_limits = '0.01 3.0'
    execute_on = TIMESTEP_END
  [../]
[]

[Outputs]
  csv = true
  execute_on = 'final'
  perf_graph = 'true'
[]
