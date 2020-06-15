[Mesh]
  type = FileMesh
  file = Thick_tank.e
  # uniform_refine = 1
[]
[Variables]
  [u]
  []
  [p]
  []
[]
[Kernels]
  [u]
    type = NullKernel
    variable = u
  []
  [p]
    type = NullKernel
    variable = p
  []
[]
[InterfaceKernels/ik]
  type = InterfaceReaction
  variable = u
  neighbor_var = p
  kf = 1
  kb = 1
  boundary = Interface
[]
[Executioner]
  type = Steady
[]
