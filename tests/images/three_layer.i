#@requirement F3.3
#@requirement DI1.4
[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 30
  ny = 30
[]

[AuxVariables]
  [./id]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Functions]
  [./image]
    type = ImageFunction
    file = three_layer.png
    component = 0
  [../]
[]

[ICs]
  [./id_ic]
    type = FunctionIC
    function = image
    variable = id
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]
