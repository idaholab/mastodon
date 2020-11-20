[GlobalParams]
  displacements = 'disp_x disp_y'
  volumetric_locking_correction = true
[]

[Problem]
  coord_type = RZ
[]

[Mesh]
  file = mesh_contact_strip.e
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[AuxVariables]
  [./T]
    order = FIRST
    family = LAGRANGE
    initial_condition = 35.0
  [../]

  [./ASR_ex]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]

  [./ASR_vstrain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ASR_strain_xx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_yy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_zz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_xy]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_yz]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]
  [./ASR_strain_zx]
    order = CONSTANT
    family = MONOMIAL
    block = 1
  [../]

  [./total_strain_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./total_strain_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./total_strain_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Modules/TensorMechanics/Master]
  [./concrete]
    block = 1
    strain = FINITE
    add_variables = true
    eigenstrain_names = 'thermal_expansion asr_expansion'
    generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx'
  [../]
  [./steel]
    block = 2
    strain = FINITE
    add_variables = true
    eigenstrain_names = 'thermal_expansion'
    generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx'
  [../]
[]

[Contact]
  [./leftright]
    primary = 6
    secondary = 5
    model = frictionless
    tangential_tolerance = 5e-4
    penalty = 1.0e12
    normalize_penalty = true
  [../]
[]

[AuxKernels]
  [./ASR_ex]
    type = MaterialRealAux
    variable = ASR_ex
    block = 1
    property = ASR_extent
    execute_on = 'timestep_end'
  [../]
  [./ASR_vstrain]
    type = MaterialRealAux
    block = 1
    variable = ASR_vstrain
    property = ASR_volumetric_strain
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_xx]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_yy]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]
  [./ASR_strain_zz]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_xy]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_xy
    index_i = 0
    index_j = 1
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_yz]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_yz
    index_i = 1
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./ASR_strain_zx]
    type = RankTwoAux
    block = 1
    rank_two_tensor = asr_expansion
    variable = ASR_strain_zx
    index_i = 0
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./total_strain_zz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = total_strain_zz
    index_i = 2
    index_j = 2
    execute_on = 'timestep_end'
  [../]

  [./total_strain_yy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = total_strain_yy
    index_i = 1
    index_j = 1
    execute_on = 'timestep_end'
  [../]

  [./total_strain_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = total_strain_xx
    index_i = 0
    index_j = 0
    execute_on = 'timestep_end'
  [../]
[]


[Materials]
  [./concreteTH]
    type = PorousMediaBase
 # setup thermal property models and parameters
 # options available: CONSTANT ASCE-1992 KODUR-2004 EUROCODE-2004 KIM-2003
    thermal_conductivity_model = CONSTANT
    thermal_capacity_model     = CONSTANT
#    aggregate_type = Siliceous               #options: Siliceous Carbonate

    ref_density_of_concrete = 2250.0         # in kg/m^3
    ref_specific_heat_of_concrete = 1100.0   # in J/(Kg.0C)
    ref_thermal_conductivity_of_concrete = 3 # in W/(m.0C)


# # setup moisture capacity and humidity diffusivity models
#     aggregate_pore_type = dense              #options: dense porous
#     aggregate_mass = 1877.0                  #mass of aggregate (kg) per m^3 of concrete
#     cement_type = 1                          #options: 1 2 3 4
#     cement_mass = 354.0                      #mass of cement (kg) per m^3 of concrete
#     water_to_cement_ratio       = 0.43
#     concrete_cure_time          = 23.0       #curing time in (days)

# #options available for humidity diffusivity:
#     moisture_diffusivity_model = Bazant      #options: Bazant Xi Mensi
#     D1 = 3.0e-12
#     aggregate_vol_fraction = 0.7             #used in Xi's moisture diffusivity model

#     coupled_moisture_diffusivity_factor = 1.0e-3  # factor for mositure diffusivity due to heat

# #coupled nonlinear variables
#     relative_humidity = rh
    temperature = T
    block = '1 2'
  [../]

  [elasticity_concrete]
    type = ComputeIsotropicElasticityTensor
    block = 1
    youngs_modulus = 37.3e9
    poissons_ratio = 0.22
#    residual_youngs_modulus_fraction = 0.5
  []

  [ASR_damage_concrete]
    type = ConcreteASRMicrocrackingDamage
    block = 1
    residual_youngs_modulus_fraction = 0.5
  []

  [thermal_strain_concrete]
    type = ComputeThermalExpansionEigenstrain
    block = 1
    temperature = T
    thermal_expansion_coeff = 1.0e-5
    stress_free_temperature = 35.0
    eigenstrain_name = thermal_expansion
  []

  [stress_concrete]
    type = ComputeDamageStress
    block = 1
    damage_model = ASR_damage_concrete
  []

  [ASR_expansion]
    type = ConcreteASREigenstrain
    block = 1
    expansion_type = Anisotropic

    reference_temperature  = 35.0
    temperature_unit = Celsius
    max_volumetric_expansion = 0.00262

    characteristic_time = 68.9
    latency_time = 111.0
    characteristic_activation_energy = 5400.0
    latency_activation_energy = 9400.0

    compressive_strength = 31.0e6
    expansion_stress_limit = 8.0e6
    tensile_strength = 3.2e6

    stress_latency_factor = 2.3333

    ASR_dependent_tensile_strength = true
    residual_tensile_strength_fraction = 0.5

    temperature = T
    relative_humidity = 0.0
    rh_exponent = 0.0
    eigenstrain_name = asr_expansion
  []

  [elasticity_steel]
    type = ComputeIsotropicElasticityTensor
    block = 2
    youngs_modulus = 193e9
    poissons_ratio = 0.3
  []

  [thermal_strain_steel]
    type = ComputeThermalExpansionEigenstrain
    block = 2
    temperature = T
    thermal_expansion_coeff = 1.0e-5
    stress_free_temperature = 35.0
    eigenstrain_name = thermal_expansion
  []

  [stress_steel]
    type = ComputeFiniteStrainElasticStress
    block = 2
  []
[]


[BCs]
  [./x_disp]
    type = DirichletBC
    variable = disp_x
    boundary = 1
    value    = 0.0
  [../]
  [./y_disp]
    type = DirichletBC
    variable = disp_y
    boundary = 3
    value    = 0.0
  [../]

  [./axial_load]
    type = NeumannBC
    variable = disp_y
    boundary = 4
    value    = -20e6
  [../]

[]

[Executioner]
  type       = Transient
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -snes_ls -pc_hypre_boomeramg_strong_threshold'
  petsc_options_value = 'hypre boomeramg 201 cubic 0.7'
#  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package -ksp_gmres_restart'
#  petsc_options_value = 'lu       superlu_dist                  101'
#  petsc_options_iname = '-pc_type -ksp_gmres_restart'
#  petsc_options_value = 'lu       101'

  dt = 100000
  num_steps = 5

  l_max_its  = 50
  l_tol      = 1e-6
  nl_max_its = 10
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-6
[]

[Outputs]
  file_base      = asr_confined_strip_out
  interval       = 1
  exodus         = true
  perf_graph     = true
  [./Console]
    type = Console
  [../]
[]
