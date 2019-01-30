# Template file for testing error messages.

[Mesh]
  type = GeneratedMesh
  nx = 1
  dim = 1
[]


[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[Kernels]
  [./DynamicTensorMechanics]
    displacements = 'disp_x'
  [../]
[]

[DiracKernels]
  [./SeismicSource]
    [./one_source]
      displacements = 'disp_x'
      point = '0.2'
      slip = y_force
      strike = 25.0 # strike angle =25
      dip = 45.0 # dip angle = pi/4, angle fault makes with horizontal
      rake = 90.0 # rake angle = pi/2, gives slip direction
      shear_modulus = 1.0
      area = 1.0
    [../]
  [../]
[]

[Materials]
  [./Elasticity_tensor_1]
    type = ComputeIsotropicElasticityTensor
    block = 0
    youngs_modulus = 2.e+2
    poissons_ratio = 0.4
  [../]

  [./strain_1]
    type = ComputeSmallStrain
    block = 0
    displacements = 'disp_x'
  [../]

  [./stress_1]
    type = ComputeLinearElasticStress
    block = 0
  [../]

  [./density_1]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'density'
    prop_values = '1.'
  [../]

[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  start_time = 0
  end_time = 5.0
  l_tol = 1e-10
  nl_abs_tol = 1e-10
  dt = 1.0
  timestep_tolerance = 1e-12
[]

[Functions]
  [./y_force]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0 3.0 4.0 5.0'
    y = '0.0 1.0 2.0 3.0 4.0 5.0'
  [../]
  [./pos]
    type = PiecewiseLinear
    x = '1    2  3'
    y = '0.1 0.2 0.3'
  [../]
[]
