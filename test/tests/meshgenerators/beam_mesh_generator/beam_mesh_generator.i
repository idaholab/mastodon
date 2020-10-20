[Mesh]
  [./amg]
    type = BeamMeshGenerator
    mesh_file = test_file.txt
  []
[]


[Outputs]
  exodus = true
[]
