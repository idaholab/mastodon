[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[]

[Variables]
  [p]
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = p
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = p
    boundary = left
    value = 0
  []
  [right]
    type = DirichletBC
    variable = p
    boundary = right
    value = 1
  []
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Postprocessors]
  [moment]
    type = SidesetMoment
    boundary = 'right'
    ref_point = '1.0 1.0 0.0'
    leverarm_direction = '0 1 0'
    p = p
  []
[]

[Outputs]
  exodus = true
[]
