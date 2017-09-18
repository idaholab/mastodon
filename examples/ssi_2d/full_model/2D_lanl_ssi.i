[Mesh]
  type = FileMesh
  file = fine_mesh_lanl_ssi.e
[]

[GlobalParams]
   alpha = -0.05
   beta = 0.275625
   gamma = 0.55
#   beta = 0.25
#   gamma = 0.5
   displacements = 'disp_x disp_y'
[]

#[Controls]
#   [./time_period]
#      type = TimePeriod
#      disable_objects = 'Kernels::inertia_x_1 Kernels::inertia_x_2 Kernels::inertia_y_1 Kernels::inertia_y_2 AuxKernels::vel_x AuxKernels::vel_y AuxKernels::accel_x AuxKernels::accel_y'
#      start_time = 0.0
#      end_time = 0.01
#   [../]
#[]

[MeshModifiers]
  [./extra_bc]
    type = ParsedAddSideset
    combinatorial_geometry = 'y=28'
    normal = '0 1 0'
    new_sideset_name = interior
  [../]
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[AuxVariables]
  [./vel_x]
  [../]
  [./vel_y]
  [../]
  [./accel_x]
  [../]
  [./accel_y]
  [../]
  [./layer_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    execute_on = timestep_end
  [../]
  [./vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    execute_on = timestep_end
  [../]
  [./accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    execute_on = timestep_end
  [../]
  [./vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    execute_on = timestep_end
  [../]
  [./layer_id]
     type = UniformLayerAuxKernel
     variable = layer_id
     interfaces = '28 64 163 192 245 264 284 284.8333 324.8333'
     layer_ids =  ' 0  1   2   3   4   5  6   7        8'
     direction = '0.0 1.0 0.0'
     block = '1 2 3 4'
     execute_on = initial
  [../]
[]

[Kernels]
  [./stress_x_1]
    type = DynamicStressDivergenceTensors
    variable = disp_x
    component = 0
    zeta = 1.5725e-05
    block = '3 4'
#    static_initialization = true
  [../]
  [./stress_y_1]
    type = DynamicStressDivergenceTensors
    variable = disp_y
    component = 1
    zeta = 1.5725e-05
    block = '3 4'
#    static_initialization = true
  [../]
  [./stress_x_3]
    type = DynamicStressDivergenceTensors
    variable = disp_x
    component = 0
    zeta = 1.5725e-05
    block = '2'
#    static_initialization = true
#    volumetric_locking_correction = true
  [../]
  [./stress_y_3]
    type = DynamicStressDivergenceTensors
    variable = disp_y
    component = 1
    zeta = 1.5725e-05
    block = '2'
#    static_initialization = true
#    volumetric_locking_correction = true
  [../]
  [./inertia_x_1]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    eta = 0.075175
    block = '2 3 4'
  [../]
  [./inertia_y_1]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    eta = 0.075175
    block = '2 3 4'
  [../]
  [./stress_x_2]
    type = DynamicStressDivergenceTensors
    variable = disp_x
    component = 0
    zeta = 8.084e-04
    block = '1'
#    static_initialization = true
  [../]
  [./stress_y_2]
    type = DynamicStressDivergenceTensors
    variable = disp_y
    component = 1
    zeta = 8.084e-04
    block = '1'
#    static_initialization = true
  [../]
  [./inertia_x_2]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    eta = 1.772
    block = '1'
  [../]
  [./inertia_y_2]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
    eta = 1.772
    block = '1'
  [../]
  [./gravity]
    type = Gravity
    variable = disp_y
    value = -32.174
    block = '1 2 3 4'
  [../]
[]

[BCs]
  [./SeismicForce]
    [./bottom_stress]
       input_components = '0 1'
       boundary = interior
       velocity_functions = 'vel_func_x vel_func_z'
    [../]
  [../]
  [./NonReflectingBC]
    [./back]
      velocities = 'vel_x vel_y'
      accelerations = 'accel_x accel_y'
      boundary = '4'
    [../]
  [../]
  [./Periodic]
    [./x_dir]
      variable = 'disp_x disp_y'
      primary = '2'
      secondary = '3'
      translation = '568.0 0.0 0.0'
    [../]
  [../]
  [./Pressure]
    [./bottom]
       disp_x = disp_x
       disp_y = disp_y
       factor = 29.4508116
       boundary = 4
    [../]
  [../]
#   [./fixed_x]
#     type = DirichletBC
#     variable = disp_x
#     boundary = 4
#     value = 0.0
#   [../]
#   [./fixed_y]
#     type = DirichletBC
#     variable = disp_y
#     boundary = 4
#     value = 0.0
#   [../]
[]

[Materials]
   [./I_Soil]
      [./nonlinear_soil_layers]
         block = '3'
         layer_variable = layer_id
         layer_ids = '0 1 2 3 4 5 6 7 8'
         soil_type = 0
         data_file = 'layer_6.csv layer_6.csv layer_5.csv layer_4.csv layer_3.csv layer_2.csv layer_1.csv layer_1.csv layer_1.csv'
         poissons_ratio = '0.21875 0.21875 0.20995973 0.38369694 0.312558895 0.3140337 0.27683564 0.2768354 0.2768354'
         initial_stress = 'initial_stress 0 0 0 initial_stress 0 0 0 initial_stress'
         density = '0.0031045 0.0031045 0.0032985 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045'

         #### pressure settings  #####
         pressure_dependency = true
         b_exp = 0.0
         p_ref = '24.3906 24.3906 17.33937 10.63772 6.54245 2.9466 0.99885 0.99885 0.99885'
         tension_pressure_cut_off = -1e-09
         a0 = 0
         a1 = 0
         a2 = 1
      [../]
      [./contact_layer]
         block = 2
         layer_variable = layer_id
         layer_ids = '0 1 2 3 4 5 6 7 8'

         soil_type = 0
         data_file = 'layer_0.csv layer_0.csv layer_0.csv layer_0.csv layer_0.csv layer_0.csv layer_0.csv layer_0.csv layer_0.csv'
#         poissons_ratio = '0.49518603 0.49518603 0.49518603 0.49518603 0.49518603 0.49518603 0.49518603 0.49518603 0.49518603'
         poissons_ratio = '0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3 0.3'
         density = '0.0031045 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045 0.0031045'
         #### pressure settings #####
         pressure_dependency = true
         b_exp = 0.0
         p_ref = '0.88992277 0.88992277  0.88992277  0.88992277  0.88992277  0.88992277 0.88992277  0.88992277  0.88992277 '
         tension_pressure_cut_off = -1e-09
         a0 = 0
         a1 = 0
         a2 = 1
  #       volumetric_locking_correction = true
      [../]
   [../]
   [./elastic_layer_7]
      type = ComputeIsotropicElasticityTensorSoil
      block = '4'
      layer_variable = layer_id
      layer_ids = '0 1 2 3 4 5 6 7 8'
      shear_modulus = '23868.598 2745.57861081911 2745.57861081911 8972.04121507413 6635.7216826688 3166.91323304399 18371.5738056901 9399.2221393628 23868.598'
      poissons_ratio = '0.0871644 0.0871644 0.0871644 0.0871644 0.0871644 0.0871644 0.0871644 0.0871644 0.0871644'
      density = '0.0032985 0.0032985 0.0032985 0.0032985 0.0032985 0.0032985 0.0032985 0.0032985 0.0032985'
   [../]
   [./density_structure]
     type = GenericConstantMaterial
     block ='1'
     prop_names = 'density'
     prop_values = '0.002'
   [../]
   [./elastic_structure]
     type = ComputeIsotropicElasticityTensor
     block = '1'
     youngs_modulus = 2.5e6
     poissons_ratio = 0.2
   [../]
   [./stress_0_7]
      block = '1 4'
      type = ComputeFiniteStrainElasticStress
      store_stress_old = true
   [../]
   [./strain_elastic_block]
      block = '4'
      type = ComputeIncrementalSmallStrain
   [../]
   [./strain_structure]
     block = 1
     type = ComputeIncrementalSmallStrain
   [../]
[]

[Functions]
  [./initial_stress]
    type = PiecewiseLinear
    x = '0	28	64	163	192	245	264	284'
    y = '28.98504182	26.18828469	22.5924541	12.08598614	9.189344836	3.895483137	1.99768366	0'
    scale_factor = -1.0
    axis = 2
  [../]
  [./vel_func_x]
    type = PiecewiseLinear
    data_file = 'vel_1100.csv'
    format = 'columns'
    scale_factor = -1.0
  [../]
  [./vel_func_z]
    type = PiecewiseLinear
    data_file = 'vel_1103.csv'
    format = 'columns'
    scale_factor = -1.0
  [../]
[]

[Preconditioning]
#  [./fdp]
#    type = FDP
#    full = true
#  [../]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
   type = Transient
   solve_type = 'PJFNK'
   start_time = 0
   end_time = 50.95
   l_tol = 1e-6
   l_max_its = 200
   nl_rel_tol = 1e-6
   nl_abs_tol = 1e-6
   dt = 0.002
#   petsc_options = '-snes_ksp_ew'
#   petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_type_boomeramg_max_iter'
#   petsc_options_value = '201                 hypre    boomeramg       4'
#   petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
#   petsc_options_value = 'lu superlu_dist'
    petsc_options = '-sub_pc_factor_nonzeros_along_diagonal'
    petsc_options_iname = '-pc_type -sub_pc_type'
    petsc_options_value = 'asm sor'
#   line_search = 'none'
   timestep_tolerance = 1e-12
[]

[VectorPostprocessors]
   [./left_structure]
     type = NodalValueSampler
     variable = 'disp_x disp_y vel_x vel_y accel_x accel_y'
     boundary = 5
     sort_by = y
     execute_on = 'timestep_end'
   [../]
   [./right_structure]
     type = NodalValueSampler
     variable = 'disp_x disp_y vel_x vel_y accel_x accel_y'
     boundary = 6
     sort_by = y
     execute_on = 'timestep_end'
   [../]
[]

[Outputs]
   file_base = 'output/friction_06_out'
   csv = true
   print_linear_residuals = true
   print_perf_log = true
   [./exodus]
     type = Exodus
     interval = 50
   [../]
  # [./out]
  #    type = Console
  #    max_rows = 1
  #    interval = 1000
  #[../]
[]
