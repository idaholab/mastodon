[Mesh]
  type = FileMesh
  file = FluidOnly.e
  # uniform_refine = 1
[]

[GlobalParams]
[]

[Variables]
  [./p]
  [../]
[]

[AuxVariables]
  [./disp_z]
  [../]
[]

[Kernels]
  [./diffusion]
    type = Diffusion
    variable = 'p'
  [../]
  [./inertia]
    type = InertialForce
    variable = p
  [../]
[]

[AuxKernels]
  [./waves]
    type = WaveHeightAuxKernel
    variable = 'disp_z'
    pressure = p
    dens = 1e-6
    grav = 9.81
    execute_on = timestep_end
  [../]
[]

[BCs]
  [./bottom_accel1]
    type = FunctionDirichletBC
    variable = p
    boundary = 'Bottom'
    function = accel_bottom
    # value = 0
  [../]
  [./free]
    type = FluidFreeSurfaceBC
    variable = p
    boundary = 'Top'
    alpha = '0.1'
  []
[]

[Functions]
  [./accel_bottom]
    type = PiecewiseLinear
    data_file = Pressure.csv # Input_1Peak_highF.csv # Input_1Peak.csv
    scale_factor = 1.0
    format = 'columns'
  [../]
[]

[Materials]
  [./density]
    type = GenericConstantMaterial
    prop_names = density
    prop_values = 4.44e-7
  [../]
[]

[Preconditioning]
  [./andy]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  # petsc_options = '-snes_ksp_ew'
  # petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  # petsc_options_value = '201                hypre    boomeramg      4'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  start_time = 0.0
  end_time = 3.0
  dt = 0.05
  dtmin = 0.00001
  nl_abs_tol = 1e-12 # 1e-3
  nl_rel_tol = 1e-12 # 1e-3
  l_tol = 1e-12 # 1e-3
  l_max_its = 25
  timestep_tolerance = 1e-8
  automatic_scaling = true
  [TimeIntegrator]
    type = NewmarkBeta
  []
[]

[Postprocessors]
  [./wave1]
    type = PointValue
    point = '-6.1 0.0 6.0'
    variable = disp_z
  [../]
  [./wave2]
    type = PointValue
    point = '-5.0833 0.0 6.0'
    variable = disp_z
  [../]
  [./wave3]
    type = PointValue
    point = '-4.0667 0.0 6.0'
    variable = disp_z
  [../]
  [./wave4]
    type = PointValue
    point = '-3.05 0.0 6.0'
    variable = disp_z
  [../]
  [./wave5]
    type = PointValue
    point = '-2.0333 0.0 6.0'
    variable = disp_z
  [../]
  [./wave6]
    type = PointValue
    point = '-1.01667 0.0 6.0'
    variable = disp_z
  [../]
  [./wave7]
    type = PointValue
    point = '0.0 0.0 6.0'
    variable = disp_z
  [../]
  [./wave8]
    type = PointValue
    point = '1.01667 0.0 6.0'
    variable = disp_z
  [../]
  [./wave9]
    type = PointValue
    point = '2.0333 0.0 6.0'
    variable = disp_z
  [../]
  [./wave10]
    type = PointValue
    point = '3.05 0.0 6.0'
    variable = disp_z
  [../]
  [./wave11]
    type = PointValue
    point = '4.0667 0.0 6.0'
    variable = disp_z
  [../]
  [./wave12]
    type = PointValue
    point = '5.0833 0.0 6.0'
    variable = disp_z
  [../]
  [./wave13]
    type = PointValue
    point = '6.1 0.0 6.0'
    variable = disp_z
  [../]
  [./P1]
    type = PointValue
    point = '6.1 0.0 6.0'
    variable = p
  [../]
  [./P2]
    type = PointValue
    point = '6.1 0.0 5.5'
    variable = p
  [../]
  [./P3]
    type = PointValue
    point = '6.1 0.0 5.0'
    variable = p
  [../]
  [./P4]
    type = PointValue
    point = '6.1 0.0 4.5'
    variable = p
  [../]
  [./P5]
    type = PointValue
    point = '6.1 0.0 4.0'
    variable = p
  [../]
  [./P6]
    type = PointValue
    point = '6.1 0.0 3.5'
    variable = p
  [../]
  [./P7]
    type = PointValue
    point = '6.1 0.0 3.0'
    variable = p
  [../]
  [./P8]
    type = PointValue
    point = '6.1 0.0 2.5'
    variable = p
  [../]
  [./P9]
    type = PointValue
    point = '6.1 0.0 2.0'
    variable = p
  [../]
  [./P10]
    type = PointValue
    point = '6.1 0.0 1.5'
    variable = p
  [../]
  [./P11]
    type = PointValue
    point = '6.1 0.0 1.0'
    variable = p
  [../]
  [./P12]
    type = PointValue
    point = '6.1 0.0 0.5'
    variable = p
  [../]
  [./P13]
    type = PointValue
    point = '6.1 0.0 0.0'
    variable = p
  [../]
  [./P14]
    type = PointValue
    point = '6.1 0.0 -0.5'
    variable = p
  [../]
  [./P15]
    type = PointValue
    point = '6.1 0.0 -1.0'
    variable = p
  [../]
  [./P16]
    type = PointValue
    point = '6.1 0.0 -1.5'
    variable = p
  [../]
  [./P17]
    type = PointValue
    point = '6.1 0.0 -2.0'
    variable = p
  [../]
  [./P18]
    type = PointValue
    point = '6.1 0.0 -2.5'
    variable = p
  [../]
  [./P19]
    type = PointValue
    point = '6.1 0.0 -3.0'
    variable = p
  [../]
  [./P20]
    type = PointValue
    point = '6.1 0.0 -3.5'
    variable = p
  [../]
  [./P21]
    type = PointValue
    point = '6.1 0.0 -4.0'
    variable = p
  [../]
  [./P22]
    type = PointValue
    point = '6.1 0.0 -4.5'
    variable = p
  [../]
  [./P23]
    type = PointValue
    point = '6.1 0.0 -5.0'
    variable = p
  [../]
  [./P24]
    type = PointValue
    point = '6.1 0.0 -5.5'
    variable = p
  [../]
[]

[Outputs]
  csv = true
  exodus = true
  perf_graph = true
  print_linear_residuals = true
  file_base = Ex_FluidOnly
  [./out]
    execute_on = 'TIMESTEP_BEGIN'
    type = CSV
    file_base = FluidOnly
  [../]
[]
