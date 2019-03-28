#@requirement F8.1
[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 3
  ny = 3
[]

[MeshModifiers]
  [./add_bnd]
    type = AddExtraNodeset
    new_boundary = 'bnd'
    nodes = '2 10'
  [../]
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  [../]
[]

[AuxVariables]
  [./accel_x]
  [../]
  [./proc_id]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./accel_x]
    type = FunctionAux
    function = t*t
    variable = accel_x
    execute_on = 'initial timestep_end'
  [../]
  [./proc]
    type = ProcessorIDAux
    variable = proc_id
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 5
  dt = 1
[]

[Outputs]
  exodus = true
  csv = true
[]

[VectorPostprocessors]
  [./accel_nodes]
    type = ResponseHistoryBuilder
    variables = 'u accel_x'
    nodes = '2 10'
  [../]
  [./accel_bnd]
    type = ResponseHistoryBuilder
    variables = 'u accel_x'
    boundary = 'bnd'
  [../]
[]
