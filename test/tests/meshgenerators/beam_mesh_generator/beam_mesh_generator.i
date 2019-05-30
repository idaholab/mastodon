[MeshGenerators]
  [./amg]
    type = BeamMeshGenerator
    mesh_file = test_file.txt
  []
[]

[Mesh]
  type = MeshGeneratorMesh
[]

[Outputs]
  exodus = true
[]
