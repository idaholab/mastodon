[Mesh]
  type = FileMesh
  file = contact_modified.e
  patch_update_strategy = iteration
  patch_size = 40
  partitioner = centroid
  centroid_partitioner_direction = y
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  volumetric_locking_correction = true
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
  [disp_z]
  []
[]

[AuxVariables]
  [nor_forc]
    order = FIRST
    family = LAGRANGE
  []
  [tang_forc_x]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [TensorMechanics]
    use_displaced_mesh = true
    displacements = 'disp_x disp_y disp_z'
  []
  [gravity]
    type = Gravity
    variable = disp_z
    value = -386.09 #in/s2
  []
[]

[AuxKernels]
  [nor_forc]
    type = PenetrationAux
    variable = nor_forc
    quantity = normal_force_magnitude
    boundary = 102
    paired_boundary = 103
  []
  [tang_forc_x]
    type = PenetrationAux
    variable = tang_forc_x
    quantity = tangential_force_x
    boundary = 102
    paired_boundary = 103
  []
[]

[BCs]
  [fix_x_soil]
    type = DirichletBC
    variable = disp_x
    boundary = 100
    value = 0.0
  []
  [fix_y_soil]
    type = DirichletBC
    variable = disp_y
    boundary = 100
    value = 0.0
  []
  [fix_z_soil]
    type = DirichletBC
    variable = disp_z
    boundary = 100
    value = 0.0
  []
  [concrete_pressure]
    type = Pressure
    boundary = 101
    variable = disp_z
    component = 2
    factor = 5 #psi
  []
[]

[Materials]
  [elasticity_tensor_block]
    youngs_modulus = 4e6 #psi
    poissons_ratio = 0.25
    type = ComputeIsotropicElasticityTensor
    block = 2
  []
  [strain_block]
    type = ComputeFiniteStrain
    block = 2
    displacements = 'disp_x disp_y disp_z'
  []
  [stress_block]
    type = ComputeFiniteStrainElasticStress
    block = 2
  []
  [den_block]
    type = GenericConstantMaterial
    block = 2
    prop_names = density
    prop_values = 0.0002248 #slug/in^3
  []
  [elasticity_tensor_soil]
    youngs_modulus = 1.3983e+05 #psi
    poissons_ratio = 0.3
    type = ComputeIsotropicElasticityTensor
    block = 1
  []
  [strain_soil]
    type = ComputeFiniteStrain
    block = 1
    displacements = 'disp_x disp_y disp_z'
  []
  [stress_soil]
    type = ComputeFiniteStrainElasticStress
    block = 1
  []
  [den_soil]
    type = GenericConstantMaterial
    block = 1
    prop_names = density
    prop_values = 0.0001356 #slug/in^3
  []
[]

[Contact]
  [leftright]
    secondary = 102
    primary = 103
    model = coulomb
    formulation = penalty
    normalize_penalty = true
    friction_coefficient = 0.7
    penalty = 1e5
    displacements = 'disp_x disp_y disp_z'
  []
[]

[Preconditioning]
  [andy]
    type = SMP
    full = true
  []
[]

[Postprocessors]
  [nor_forc]
    type = NodalSum
    variable = nor_forc
    boundary = 102
  []
  [tang_forc_x]
    type = NodalSum
    variable = tang_forc_x
    boundary = 102
  []
  [dispx]
    type = NodalExtremeValue
    value_type = max
    block = '2'
    variable = disp_x
  []
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu     superlu_dist'
  line_search = 'none'
  end_time = 2.0
  dt = 0.005
  dtmin = 0.001
  nl_abs_tol = 1e-1
  nl_rel_tol = 1e-4
  l_tol = 1e-2
  l_max_its = 20
  timestep_tolerance = 1e-3
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  [console]
    type = Console
    max_rows = 1
  []
  [out1]
    type = Checkpoint
    time_step_interval = 1
    num_files = 2
  []
[]
