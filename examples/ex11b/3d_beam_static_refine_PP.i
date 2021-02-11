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
    elem_type = 'HEX20'
  []
  [nodeset_gen]
    type = ExtraNodesetGenerator
    new_boundary = 'center_node'
    coord = '5.0 0.0 0.5'
    input = 'mesh_gen'
  [../]
  [subdomain_near]
    type = SubdomainBoundingBoxGenerator
    bottom_left = '0.0 0.0 0.0'
    top_right = '1.0 1.0 1.0'
    location = INSIDE
    block_id = 1
    input = nodeset_gen
  []
  [subdomain_small]
    type = SubdomainBoundingBoxGenerator
    bottom_left = '0.0 0.0 0.0'
    top_right = '1.0 0.5 1.0'
    location = INSIDE
    block_id = 5
    input = subdomain_near
  []
[]

[Adaptivity]
  initial_marker = marker1
  initial_steps = 5
  [Markers]
    [marker1]
      type = MinimumElementSizeMarker
      element_size = 0.25
      block = 1
    []
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Modules/TensorMechanics/Master]
  [./strain_calculator]
    strain = FINITE
    generate_output = 'stress_xx'
  [../]
[]

[Variables]
  [./disp_x]
    order = SECOND
    family = LAGRANGE
  [../]
  [./disp_y]
    order = SECOND
    family = LAGRANGE
  [../]
  [./disp_z]
    order = SECOND
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
  [disp_y]
    type = PointValue
    point = '5.0 0.0 0.5'
    variable = disp_y
  []
  [moment_z]
    type = SidesetMoment
    stress_direction = '1 0 0'
    stress_tensor = stress
    boundary = 'left'
    reference_point = '0.0 0.0 0.0'
    moment_direction = '0 0 1'
  []
  [avg_stress_xx_side]
    type = SideAverageValue
    variable = stress_xx
    boundary = left
  []
  [stress_xx_side]
    type = PointValue
    variable = stress_xx
    point = '0.0 1.0 0.5'
  []
  [avg_stress_xx_block]
    type = ElementAverageValue
    variable = stress_xx
    block = 5
  []
[]

[Outputs]
  file_base = 11_b_out
  exodus = true
  csv = true
  perf_graph = true
[]
