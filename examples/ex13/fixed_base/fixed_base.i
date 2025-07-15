[Mesh]
  [mesh_gen]
    type = FileMeshGenerator
    file = fixed_base_mesh.e
  []
  [interface1]
    type = SideSetsBetweenSubdomainsGenerator
    input = mesh_gen
    primary_block = 'fluid_material'
    paired_block = 'RV'
    new_boundary = 'Interface'
  []
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [disp_x]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' #' # 99 98 97 96 95 94'
    order = FIRST
    family = LAGRANGE
  []
  [disp_y]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    order = FIRST
    family = LAGRANGE
  []
  [disp_z]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    order = FIRST
    family = LAGRANGE
  []
  [p]
    block = 'fluid_material'
  []
[]

[AuxVariables]
  [Wave1]
    block = 'fluid_material'
  []
  [vel_x]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
  [accel_x]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
  [vel_y]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
  [accel_y]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
  [vel_z]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
  [accel_z]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = 'p'
    block = 'fluid_material'
    use_displaced_mesh = false
  []
  [inertia]
    type = AcousticInertia
    variable = p
    block = 'fluid_material'
    use_displaced_mesh = false
  []
  [DynamicTensorMechanics]
    stiffness_damping_coefficient = 0.00006366
    displacements = 'disp_x disp_y disp_z'
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
  []
  [inertia_x]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = InertialForce
    variable = disp_x
    eta = 7.854
  []
  [inertia_y]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = InertialForce
    variable = disp_y
    eta = 7.854
  []
  [inertia_z]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = InertialForce
    variable = disp_z
    eta = 7.854
  []
[]

[AuxKernels]
  [accel_x]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_x]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    gamma = 0.5
    execute_on = timestep_end
  []
  [accel_y]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_y]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    gamma = 0.5
    execute_on = timestep_end
  []
  [accel_z]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    beta = 0.25
    execute_on = timestep_end
  []
  [vel_z]
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat RV SGs RV_slab' # 99 98 97 96 95 94'
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    gamma = 0.5
    execute_on = timestep_end
  []
  [waves]
    type = WaveHeightAuxKernel
    variable = 'Wave1'
    pressure = p
    density = 1e-6
    gravity = 9.81
    execute_on = timestep_end
    block = 'fluid_material'
  []
[]

[InterfaceKernels]
  [interface1]
    type =  StructureAcousticInterface
    variable = p
    neighbor_var = disp_x
    boundary = 'Interface'
    D = 1e-6
    component = 0
  []
  [interface2]
    type =  StructureAcousticInterface
    variable = p
    neighbor_var = disp_y
    boundary = 'Interface'
    D = 1e-6
    component = 1
  []
  [interface3]
    type =  StructureAcousticInterface
    variable = p
    neighbor_var = disp_z
    boundary = 'Interface'
    D = 1e-6
    component = 2
  []
[]

[Materials]
  [elasticity_1]
    type = ComputeIsotropicElasticityTensor
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat'
    youngs_modulus = 24.8
    poissons_ratio = 0.2
  []
  [strain_1]
    type = ComputeSmallStrain
    displacements = 'disp_x disp_y disp_z'
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat'
  []
  [stress_1]
    type = ComputeLinearElasticStress
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat'
  []
  [den_1]
    type = GenericConstantMaterial
    block = 'roof ext_buttresses ext_walls int_buttresses SG_bases int_wall int_slab RV_housing small_walls basemat'
    prop_names = density
    prop_values = 2.4e-6 #kg/m3
  []

  [elasticity_2]
    type = ComputeIsotropicElasticityTensor
    block = 'RV SGs RV_slab'
    youngs_modulus = 170
    poissons_ratio = 0.3
  []
  [strain_2]
    type = ComputeSmallStrain
    displacements = 'disp_x disp_y disp_z'
    block = 'RV SGs RV_slab'
  []
  [stress_2]
    type = ComputeLinearElasticStress
    block = 'RV SGs RV_slab'
  []

  [den_rv]
    type = GenericConstantMaterial
    block = 'RV RV_slab'
    prop_names = density
    prop_values = 7.85e-6 #kg/m3
  []
  [den_sg]
    type = GenericConstantMaterial
    block = 'SGs'
    prop_names = density
    prop_values = 7.85e-6 #kg/m3
  []

  [density_fluid]
    type = GenericConstantMaterial
    prop_names = inv_co_sq
    prop_values = 4.65e-7
    block = 'fluid_material'
  []
  [dens2]
    type = GenericConstantMaterial
    block = 'fluid_material'
    prop_names = density
    prop_values = 1e-6
  []
[]

[Functions]
  [input_motion_x]
    type = PiecewiseLinear
    data_file = 'fixed_base_input_motion.csv'
    format = columns
    xy_in_file_only = false
    y_index_in_file = 1
  []
  [input_motion_y]
    type = PiecewiseLinear
    data_file = 'fixed_base_input_motion.csv'
    format = columns
    xy_in_file_only = false
    y_index_in_file = 2
  []
  [input_motion_z]
    type = PiecewiseLinear
    data_file = 'fixed_base_input_motion.csv'
    format = columns
    xy_in_file_only = false
    y_index_in_file = 3
  []
[]

[BCs]
  [x_motion]
    type = PresetAcceleration
    acceleration = accel_x
    velocity = vel_x
    variable = disp_x
    beta = 0.25
    boundary = 'bottom_surface'
    function = 'input_motion_x'
  []
  [y_motion]
    type = PresetAcceleration
    acceleration = accel_y
    velocity = vel_y
    variable = disp_y
    beta = 0.25
    boundary = 'bottom_surface'
    function = 'input_motion_y'
  []
  [z_motion]
    type = PresetAcceleration
    acceleration = accel_z
    velocity = vel_z
    variable = disp_z
    beta = 0.25
    boundary = 'bottom_surface'
    function = 'input_motion_z'
  []

  [free]
    type = FluidFreeSurfaceBC
    variable = p
    boundary = 'fluid_top'
  []
[]

[Postprocessors]
[Wave1]
  type = PointValue
  point = '-17.235 0.0 6.655'
  variable = Wave1
[]
[Wave2]
  type = PointValue
  point = '-17.0 0.0 6.655'
  variable = Wave1
[]
[Wave3]
  type = PointValue
  point = '-16.75 0.0 6.655'
  variable = Wave1
[]
[Wave4]
  type = PointValue
  point = '-16.5 0.0 6.655'
  variable = Wave1
[]
[Wave5]
  type = PointValue
  point = '-16.25 0.0 6.655'
  variable = Wave1
[]
[Wave6]
  type = PointValue
  point = '-16.0 0.0 6.655'
  variable = Wave1
[]
[Wave7]
  type = PointValue
  point = '-15.75 0.0 6.655'
  variable = Wave1
[]
[Wave8]
  type = PointValue
  point = '-15.5 0.0 6.655'
  variable = Wave1
[]
[Wave9]
  type = PointValue
  point = '-15.25 0.0 6.655'
  variable = Wave1
[]
[Wave10]
  type = PointValue
  point = '-15.0 0.0 6.655'
  variable = Wave1
[]
[Wave11]
  type = PointValue
  point = '-14.75 0.0 6.655'
  variable = Wave1
[]
[Wave12]
  type = PointValue
  point = '-14.5 0.0 6.655'
  variable = Wave1
[]
[Wave13]
  type = PointValue
  point = '-14.25 0.0 6.655'
  variable = Wave1
[]
[Wave14]
  type = PointValue
  point = '-14.0 0.0 6.655'
  variable = Wave1
[]
[Wave15]
  type = PointValue
  point = '-13.75 0.0 6.655'
  variable = Wave1
[]
[Wave16]
  type = PointValue
  point = '-13.5 0.0 6.655'
  variable = Wave1
[]
[Wave17]
  type = PointValue
  point = '-13.25 0.0 6.655'
  variable = Wave1
[]
[Wave18]
  type = PointValue
  point = '-13.0 0.0 6.655'
  variable = Wave1
[]
[Wave19]
  type = PointValue
  point = '-12.765 0.0 6.655'
  variable = Wave1
[]

[p1_n]
  type = PointValue
  point = '-17.48 0.0 6.655'
  variable = p
[]
[p2_n]
  type = PointValue
  point = '-17.48 0.0 6.3'
  variable = p
[]
[p3_n]
  type = PointValue
  point = '-17.48 0.0 6.0'
  variable = p
[]
[p4_n]
  type = PointValue
  point = '-17.48 0.0 5.7'
  variable = p
[]
[p5_n]
  type = PointValue
  point = '-17.48 0.0 5.4'
  variable = p
[]
[p6_n]
  type = PointValue
  point = '-17.48 0.0 5.1'
  variable = p
[]
[p7_n]
  type = PointValue
  point = '-17.48 0.0 4.8'
  variable = p
[]
[p8_n]
  type = PointValue
  point = '-17.48 0.0 4.5'
  variable = p
[]
[p9_n]
  type = PointValue
  point = '-17.48 0.0 4.2'
  variable = p
[]
[p10_n]
  type = PointValue
  point = '-17.48 0.0 3.9'
  variable = p
[]
[p11_n]
  type = PointValue
  point = '-17.48 0.0 3.6'
  variable = p
[]
[p12_n]
  type = PointValue
  point = '-17.48 0.0 3.3'
  variable = p
[]
[p13_n]
  type = PointValue
  point = '-17.48 0.0 3.0'
  variable = p
[]
[p14_n]
  type = PointValue
  point = '-17.48 0.0 2.855'
  variable = p
[]

[p1_p]
  type = PointValue
  point = '-12.52 0.0 6.655'
  variable = p
[]
[p2_p]
  type = PointValue
  point = '-12.52 0.0 6.3'
  variable = p
[]
[p3_p]
  type = PointValue
  point = '-12.52 0.0 6.0'
  variable = p
[]
[p4_p]
  type = PointValue
  point = '-12.52 0.0 5.7'
  variable = p
[]
[p5_p]
  type = PointValue
  point = '-12.52 0.0 5.4'
  variable = p
[]
[p6_p]
  type = PointValue
  point = '-12.52 0.0 5.1'
  variable = p
[]
[p7_p]
  type = PointValue
  point = '-12.52 0.0 4.8'
  variable = p
[]
[p8_p]
  type = PointValue
  point = '-12.52 0.0 4.5'
  variable = p
[]
[p9_p]
  type = PointValue
  point = '-12.52 0.0 4.2'
  variable = p
[]
[p10_p]
  type = PointValue
  point = '-12.52 0.0 3.9'
  variable = p
[]
[p11_p]
  type = PointValue
  point = '-12.52 0.0 3.6'
  variable = p
[]
[p12_p]
  type = PointValue
  point = '-12.52 0.0 3.3'
  variable = p
[]
[p13_p]
  type = PointValue
  point = '-12.52 0.0 3.0'
  variable = p
[]
[p14_p]
  type = PointValue
  point = '-12.52 0.0 2.855'
  variable = p
[]
[moment_x]
  type = SidesetMoment
  boundary = 'fluid_interface'
  reference_point = '-15.0 0.0 7.625'
  moment_direction = '1 0 0'
  pressure = p
[]
[moment_y]
  type = SidesetMoment
  boundary = 'fluid_interface'
  reference_point = '-15.0 0.0 7.625'
  moment_direction = '0 1 0'
  pressure = p
[]
[moment_z]
  type = SidesetMoment
  boundary = 'fluid_interface'
  reference_point = '-15.0 0.0 7.625'
  moment_direction = '0 0 1'
  pressure = p
[]
[]


[VectorPostprocessors]
  [accel_hist_x]
    type = ResponseHistoryBuilder
    variables = 'accel_x'
    boundary = 'rv_head_nodes'
    outputs = out1
  []
  [accel_spec_x]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist_x
    regularize_dt = 0.002
    damping_ratio = 0.05
    start_frequency = 0.1
    end_frequency = 1000
    outputs = out1
  []

  [accel_hist_y]
    type = ResponseHistoryBuilder
    variables = 'accel_y'
    boundary = 'rv_head_nodes'
    outputs = out1
  []
  [accel_spec_y]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist_y
    regularize_dt = 0.002
    damping_ratio = 0.05
    start_frequency = 0.1
    end_frequency = 1000
    outputs = out1
  []

  [accel_hist_z]
    type = ResponseHistoryBuilder
    variables = 'accel_z'
    boundary = 'rv_head_nodes'
    outputs = out1
  []
  [accel_spec_z]
    type = ResponseSpectraCalculator
    vectorpostprocessor = accel_hist_z
    regularize_dt = 0.002
    damping_ratio = 0.05
    start_frequency = 0.1
    end_frequency = 1000
    outputs = out1
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
  petsc_options = '-ksp_snes_ew'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  solve_type = 'NEWTON'
  nl_rel_tol = 1e-7
  nl_abs_tol = 1e-12
  dt = 0.01
  end_time = 28.0
  timestep_tolerance = 1e-6
  [TimeIntegrator]
    type = NewmarkBeta
    beta = 0.25
    gamma = 0.5
  []
[]

[Outputs]
  exodus = true
  perf_graph = true
  csv = true
  [out1]
    type = CSV
    execute_on = 'final'
  []
[]
