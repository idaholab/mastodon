# Test to demonstrate the Seismic Probabilistic Risk Assessment (SPRA)
# infrastructure in MASTODON.
#

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  xmin = 0.0
  xmax = 4.0
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
  [./rot_x]
  [../]
  [./rot_y]
  [../]
  [./rot_z]
  [../]
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Distributions]
 [./uniform_E1]
   type = UniformDistribution
   lower_bound = 1e04
   upper_bound = 8e04
 [../]
[]

[Samplers]
  [./sample]
    type = MonteCarloSampler
    n_samples = 3
    distributions = 'uniform_E1'
    execute_on = INITIAL # create random numbers on initial and use them for each timestep
  [../]
[]

[MultiApps]
  [./sub]
    # creates sub files for each monte carlo sample and each scaled ground motion
    # Total number of simulations = number_of_bins * num_gms * n_samples
    type = SamplerTransientMultiApp
    input_files = 'sub_sub_Timoshenko.i'
    sampler = sample
    execute_on = TIMESTEP_BEGIN
  [../]
[]

[Transfers]
  [./sub]
    # transfers monte carlo samples to multiapp
    type = SamplerTransfer
    multi_app = sub
    parameters = 'AuxKernels/youngs_modulus1/value'
    to_control = 'stochastic'
    execute_on = INITIAL
    check_multiapp_execute_on = false
  [../]
[]

[Functions]
  [./force]
    type = ConstantFunction
    value = 0.01
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  end_time = 0.3
  dt = 0.1
  dtmin = 0.01
  nl_abs_tol = 1e-3
    nl_rel_tol = 1e-03
  l_tol = 1e-3
  l_max_its = 20
  timestep_tolerance = 1e-3
[]

[VectorPostprocessors]
  [./data]
    type = SamplerData
    sampler = sample
    execute_on = 'INITIAL'
  [../]
[]

[Outputs]
  csv = true
[]
