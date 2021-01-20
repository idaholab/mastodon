[Mesh]
  [mesh_gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 20
    ny = 2
    nz = 2
    xmin = 0.0
    xmax = 5.0
    ymin = 0.0
    ymax = 1.0
    zmin = 0.0
    zmax = 1.0
    # elem_type = 'HEX8'
    elem_type = 'HEX20'
  []
  [nodeset_gen]
    type = ExtraNodesetGenerator
    new_boundary = 'base_node end_node'
    coord = '0.0 0.0 0.5; 5.0 0.0 0.5'
    input = 'mesh_gen'
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
    # order = FIRST
    order = SECOND
    family = LAGRANGE
  [../]
  [./disp_y]
    # order = FIRST
    order = SECOND
    family = LAGRANGE
  [../]
  [./disp_z]
    # order = FIRST
    order = SECOND
    family = LAGRANGE
  [../]
[]

[Kernels]
  [DynamicTensorMechanics]
  []
  [inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    beta = 0.25
    gamma = 0.5
  []
  [inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    beta = 0.25
    gamma = 0.5
  []
  [inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
    beta = 0.25
    gamma = 0.5
  []
[]

[AuxVariables]
  [vel_x]
    # order = FIRST
    order = SECOND
  []
  [accel_x]
    # order = FIRST
    order = SECOND
  []
  [vel_y]
    order = SECOND
  []
  [accel_y]
    order = SECOND
  []
  [vel_z]
    order = SECOND
  []
  [accel_z]
    order = SECOND
  []
[]

[AuxKernels]
  [accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = timestep_end
  []
  [accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = timestep_end
  []
  [accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = timestep_end
  []
[]

[BCs]
  [fix_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left'
    value = 0.0
  []
  [fix_z]
    type = DirichletBC
    variable = disp_z
    boundary = 'left'
    value = 0.0
  []
  [y_motion]
    type = PresetAcceleration
    acceleration = accel_y
    velocity = vel_y
    variable = disp_y
    beta = 0.25
    boundary = 'left'
    function = 'ormsby'
  []
[]

[Functions]
  [ormsby]
    type = OrmsbyWavelet
    f1 = 0.0
    f2 = 0.1
    f3 = 45.0
    f4 = 50.0
    ts = 1.5
    scale_factor = 0.5
  []
[]


[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 10e9
    poissons_ratio = 0.3
  []
  [strain_1]
    type = ComputeSmallStrain
  []
  [stress_1]
    type = ComputeLinearElasticStress
  []
  [den_1]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 1200
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  petsc_options = '-ksp_snes_ew'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  line_search = 'none'
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  dt = 0.01
  end_time = 3.0
  timestep_tolerance = 1e-6
[]

[VectorPostprocessors]
  [accel_hist]
    type = ResponseHistoryBuilder
    variables = 'accel_y'
    boundary = 'base_node end_node'
  []
  [accel_spec]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist
    regularize_dt = 0.01
    damping_ratio = 0.05
    start_frequency = 0.1
    end_frequency = 100
    outputs = 2_2
  []
[]

[Outputs]
  perf_graph = true
  [./2_2]
    type = CSV
    execute_on = 'final'
  [../]
[]
