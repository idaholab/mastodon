
[Mesh]
  [mesh_gen]
    type = GeneratedMeshGenerator
    dim = 3
    ny = 2
    ymin = 0.0
    ymax = 0.5
    zmin = 0.0
    zmax = 0.5
    nz = 2
    nx = 10
    xmin = 0.0
    xmax = 5.0
  []
  [nodeset_gen]
    type = ExtraNodesetGenerator
    new_boundary = 'corner_node'
    coord = '5.0 0.25 0.25'
    input = 'mesh_gen'
  []
  displacements = 'disp_x disp_y disp_z'
  second_order = true
[]

[Variables]
  [disp_x]
    order = SECOND
    family = LAGRANGE
  []
  [disp_y]
    order = SECOND
    family = LAGRANGE
  []
  [disp_z]
    order = SECOND
    family = LAGRANGE
  []
[]

[Kernels]
  [DynamicTensorMechanics]
    displacements = 'disp_x disp_y disp_z'
  []
[]

[BCs]
  [fixx1]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  []
  [fixy1]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  []
  [fixz1]
    type = DirichletBC
    variable = disp_z
    boundary = left
    value = 0.0
  []
[]

[NodalKernels]
  [force_y2]
    type = ConstantRate
    variable = disp_y
    boundary = corner_node
    rate = 1.0e-4
  []
[]

[Materials]
  [elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 2.1e2
    poissons_ratio = 0.3
  []
  [strain]
    type = ComputeFiniteStrain
    displacements = 'disp_x disp_y disp_z'
  []
  [./stress]
    type =  ComputeFiniteStrainElasticStress
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
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  solve_type = NEWTON
  line_search = 'none'
  nl_max_its = 15
  nl_rel_tol = 1e-7
  nl_abs_tol = 1e-7
  dt = 1
  dtmin = 1
  end_time = 1
[]

[Postprocessors]
  [./moment_z_bot]
    type = SidesetMoment
    stress_direction = '1 0 0'
    stress_tensor = stress
    boundary = 'left'
    reference_point = '0.0 0.0 0.25'
    moment_direction = '0 0 -1'
  [../]
[]

[Outputs]
  exodus = true
[]
