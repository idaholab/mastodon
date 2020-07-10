# A test of the BaselineCorrection function output applied as a BC. The input
# acceleration time history is twenty cycles of `-250 \pi^{2} \sin(50 \pi t)`

[GlobalParams]
 displacements = 'disp_x disp_y disp_z'
 gamma = 0.5
 beta = 0.25
[]

[Mesh]
  # generate a 1 m * 1 m * 1 m block
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 1
  nz = 1
  xmax = 1
  ymax = 1
  zmax = 1
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
  [vel_x]
  []
  [accel_x]
  []
  [vel_y]
  []
  [accel_y]
  []
  [vel_z]
  []
  [accel_z]
  []
[]

[Kernels]
  [stress_x]
    type = DynamicStressDivergenceTensors
    variable = disp_x
    component = 0
    use_displaced_mesh = true
  []
  [stress_y]
    type = DynamicStressDivergenceTensors
    variable = disp_y
    component = 1
    use_displaced_mesh = true
  []
  [stress_z]
    type = DynamicStressDivergenceTensors
    variable = disp_z
    component = 2
    use_displaced_mesh = true
  []
  [inertia_x]
    type = InertialForce
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
  []
  [inertia_y]
    type = InertialForce
    variable = disp_y
    velocity = vel_y
    acceleration = accel_y
  []
  [inertia_z]
    type = InertialForce
    variable = disp_z
    velocity = vel_z
    acceleration = accel_z
  []
[]

[AuxKernels]
  [accel_x]
    type = NewmarkAccelAux
    variable = accel_x
    displacement = disp_x
    velocity = vel_x
    execute_on = timestep_end
  []
  [accel_y]
    type = NewmarkAccelAux
    variable = accel_y
    displacement = disp_y
    velocity = vel_y
    execute_on = timestep_end
  []
  [accel_z]
    type = NewmarkAccelAux
    variable = accel_z
    displacement = disp_z
    velocity = vel_z
    execute_on = timestep_end
  []
  [vel_x]
    type = NewmarkVelAux
    variable = vel_x
    acceleration = accel_x
    execute_on = timestep_end
  []
  [vel_y]
    type = NewmarkVelAux
    variable = vel_y
    acceleration = accel_y
    execute_on = timestep_end
  []
  [vel_z]
    type = NewmarkVelAux
    variable = vel_z
    acceleration = accel_z
    execute_on = timestep_end
  []
[]

[Materials]
  [elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 200e09 # Pa, young's modulus of steel
    poissons_ratio = 0.3 # poisson's ratio of steel
    use_displaced_mesh = true
  []
  [strain_tensor]
    type = ComputeFiniteStrain
  []
  [stress_tensor]
    type = ComputeFiniteStrainElasticStress
  []
  [density]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 8050 # kg / m^3, density of steel
    use_displaced_mesh = true
  []
[]

[Functions]
  [corrected_accel_func]
    type = BaselineCorrection
    data_file = twenty_sines_accel.csv
    time_name = time
    acceleration_name = acceleration
    accel_fit_order = 9
    vel_fit_order = 9
    disp_fit_order = 7
  []
[]

[ICs]
  # The initial value of the corrected accel may be non-zero, so the AuxKernel
  # value must be enforced for PresetAcceleration to integrate properly
  #
  # TODO: Create action system that handles the BaselineCorrection function,
  # FunctionIC, and PresetAcceleration for a single set of inputs
  #
  [initial_ground_accel]
    # applies corrected acceleration function value at initial time step
    type = FunctionIC
    variable = accel_x
    boundary = 5
    function = corrected_accel_func
  []
[]

[BCs]
  [fixx]
    type = DirichletBC
    variable = disp_x
    boundary = 0
    value = 0
  []
  [fixy]
    type = DirichletBC
    variable = disp_y
    boundary = 0
    value = 0
  []
  [fixz]
    type = DirichletBC
    variable = disp_z
    boundary = 0
    value = 0
  []
  [Periodic]
    [x_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 4
      secondary = 2
      translation = '1.0 0.0 0.0'
    []
    [y_dir]
      variable = 'disp_x disp_y disp_z'
      primary = 1
      secondary = 3
      translation = '0.0 1.0 0.0'
    []
  []
  [apply_corrected_accel]
    type = PresetAcceleration
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    boundary = 5
    function = corrected_accel_func
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
  nl_rel_tol = 1e-06
  nl_abs_tol = 1e-09
  start_time = 0.0
  end_time = 0.8
  dt = 1e-03
  timestep_tolerance = 1e-06
  line_search = none
  petsc_options = '-ksp_snes_ew'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = ' 201                hypre    boomeramg      4'
[]

[Postprocessors]
  [check_func_value]
    type = FunctionValuePostprocessor
    function = corrected_accel_func
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [disp_top]
    type = NodalMaxValue
    variable = disp_x
    boundary = 5
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [vel_top]
    type = NodalMaxValue
    variable = vel_x
    boundary = 5
    execute_on = 'INITIAL TIMESTEP_END'
  []
  [accel_top]
    type = NodalMaxValue
    variable = accel_x
    boundary = 5
    execute_on = 'INITIAL TIMESTEP_END'
  []
[]

[Outputs]
  csv = true
[]
