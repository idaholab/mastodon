[Mesh]
  [mesh_gen]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 10
    ny = 2
    nz = 2
    xmin = 0.0
    xmax = 5.0
    ymin = 0.0
    ymax = 1.0
    zmin = 0.0
    zmax = 1.0
    elem_type = 'HEX8'
  []
  [nodeset_gen]
    type = ExtraNodesetGenerator
    new_boundary = 'center_node'
    coord = '5.0 0.0 0.5'
    input = 'mesh_gen'
  [../]
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Physics/SolidMechanics/QuasiStatic]
  [./strain_calculator]
    strain = FINITE
  [../]
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[BCs]
  [./fixx0]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./fixy1]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  [../]
  [./fixy2]
    type = DirichletBC
    variable = disp_z
    boundary = left
    value = 0.0
  [../]
[]

[NodalKernels]
  [./force_y2]
    type = ConstantRate
    variable = disp_y
    boundary = 'center_node'
    rate = -1000
  [../]
[]


[Materials]
  [./elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 10e9
    poissons_ratio = 0.3
  [../]
  [./stress]
    type = ComputeFiniteStrainElasticStress
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  petsc_options = '-ksp_snes_ew'
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  line_search = 'none'
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-10
[]

[Postprocessors]
  [./disp_y]
    type = PointValue
    point = '5.0 0.0 0.5'
    variable = disp_y
  [../]
[]

[Outputs]
  file_base = 2_2_out
  exodus = true
  csv = true
  perf_graph = true
[]
