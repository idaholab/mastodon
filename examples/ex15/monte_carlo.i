[StochasticTools]
[]

[Distributions]
  [M_uniform]
    type = Normal
    mean = 4
    standard_deviation = 1
  []
  [R_uniform]
    type = Normal
    mean = 25
    standard_deviation = 5.5
  []
  [V_uniform]
    type = Normal
    mean = 475
    standard_deviation = 65
  []
[]

[Samplers]
  [sample]
    type = MonteCarlo
    num_rows = 20
    distributions = 'M_uniform R_uniform V_uniform'
    execute_on = PRE_MULTIAPP_SETUP
  []
[]

[MultiApps]
  [runner]
    type = SamplerFullSolveMultiApp
    input_files = 'TankUQ_mod.i'
    sampler = sample
    execute_on = 'TIMESTEP_BEGIN'
    mode = batch-reset
  []
[]

[Transfers]
  [results]
    type = SamplerReporterTransfer
    from_multi_app = runner
    sampler = sample
    stochastic_reporter = results
    from_reporter = 'PeakPressure/value PeakWaveHeight/value PeakAcc/value PeakDisp/value'
  []
[]

[Controls]
  [cmdline]
    type = MultiAppSamplerControl
    multi_app = runner
    sampler = sample
    param_names = 'Functions/groundmotion/M Functions/groundmotion/R Functions/groundmotion/Vs30'
  []
[]

[Reporters]
  [results]
    type = StochasticReporter
    parallel_type = ROOT
  []
[]

[VectorPostprocessors]
  [data]
    type = SamplerData
    sampler = sample
    execute_on = 'initial'
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
csv = true
  [out]
    type = JSON
  []
[]
