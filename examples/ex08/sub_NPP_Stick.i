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
  type = FileMesh
  file = foundbeam_noded.e
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
  [./rot_x]
    block = '1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16'
  [../]
  [./rot_y]
    block = '1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16'
  [../]
  [./rot_z]
    block = '1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16'
  [../]
[]

[Problem]
  solve = false
  kernel_coverage_check = false
[]

[Distributions]
  [./uniform_zeta]
    type = BoostLognormalDistribution
    location = -7.5167
    scale = 0.35
  [../]
  [./uniform_eta]
    type = BoostLognormalDistribution
    location = 1.1837
    scale = 0.35
  [../]
 [./uniform_E1]
   type = UniformDistribution
   lower_bound = 1.9e5
   upper_bound = 9.9e5
 [../]
 [./uniform_E2]
   type = UniformDistribution
   lower_bound = 1.9e5
   upper_bound = 9.9e5
 [../]
 [./uniform_E3]
   type = UniformDistribution
   lower_bound = 5e7
   upper_bound = 5e8
 [../]
[]

[Samplers]
  [./sample]
    type = MonteCarloSampler
    n_samples = 2
    distributions = 'uniform_zeta uniform_eta uniform_E1 uniform_E2 uniform_E3'
    execute_on = INITIAL # create random numbers on initial and use them for each timestep
  [../]
[]

[MultiApps]
  [./sub]
    # creates sub files for each monte carlo sample and each scaled ground motion
    # Total number of simulations = number_of_bins * num_gms * n_samples
    type = SamplerTransientMultiApp
    input_files = 'sub_sub_NPP_Stick.i'
    sampler = sample
    execute_on = TIMESTEP_BEGIN
  [../]
[]

[Transfers]
  [./sub]
    # transfers monte carlo samples to multiapp
    type = SamplerTransfer
    multi_app = sub
    parameters = 'Materials/material_zeta/prop_values Materials/material_eta/prop_values AuxKernels/youngs_modulus1/value AuxKernels/youngs_modulus2/value AuxKernels/youngs_modulus3/value'
    to_control = 'stochastic'
    execute_on = INITIAL
    check_multiapp_execute_on = false
  [../]
  [./transfer]
    # transfers scaled ground motions to multiapp
    type = PiecewiseFunctionTransfer
    multi_app = sub
    direction = to_multiapp
    to_function = accel_x # name of function in subsub.i which uses the scaled ground motions
    from_function = accel_x # name of the function in sub.i, which receives the scaled ground motions
  [../]
[]

[Functions]
  [./accel_x]
    type = PiecewiseLinear
    # Piecewiselinear function that receiving scaled GMs from master.i. Input here is dummy.
    x = '32 34'
    y = '0 0'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      4'
  end_time = 2.0
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
    execute_on = 'LINEAR'
  [../]
[]

[Outputs]
  csv = true
[]
