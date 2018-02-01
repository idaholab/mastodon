[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 1
  ny = 1
[]

[Variables]
  [./dummy]
  [../]
[]

[Distributions]
  [./lognormal_density]
    type = LognormalDistribution
    location = 1.0
    scale = 1.5
  [../]
  [./lognormal_shear_modulus]
    type = LognormalDistribution
    location = 1.0
    scale = 1.3
  [../]
[]

[Samplers]
  [./sample]
    type = MonteCarloSampler
    n_samples = 10
    distributions = 'lognormal_density lognormal_shear_modulus'
    execute_on = INITIAL # create random numbers on initial and use them for each timestep
  [../]
[]

[MultiApps]
  [./sub]
    type = SamplerMultiApp
    input_files = sub_shearbeam.i
    sampler = sample
  [../]
[]

[Transfers]
  [./sub]
    type = SamplerTransfer
    multi_app = sub
    parameters = 'Materials/elasticity_tensor/scale_density Materials/elasticity_tensor/scale_shear_modulus'
    to_control = 'stochastic'
    execute_on = INITIAL
    check_multiapp_execute_on = false
  [../]
[]

[Executioner]
  type = Transient
  start_time = 32
  #end_time = 33
  num_steps = 202
  dt = 0.005
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Outputs]
  execute_on = 'timestep_end final'
  csv = true
  #exodus = true
[]
