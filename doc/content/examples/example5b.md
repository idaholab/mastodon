# Example 5b: Dynamic analysis of a stick model of a nuclear power plant with mesh generated using text input

## Model

This example demonstrates the [BeamMeshGenerator.md] to create a stick model of a nuclear power plant taken from the SASSI2000 user manual [!citet](sassiuser2006), and described in [Example 5a](examples/example5a). Unlike in [Example 5a](examples/example5a), the basemat is not modeled. The rotations of the stick model are restrained at the base and the same input acceleration history shown in [Example 5a](examples/example5a) is applied at the base of the structure. The dynamic responses thus obtained are compared with the dynamic responses obtained from Example 5a, which imports a mesh file generated using [Cubit](https://cubit.sandia.gov/).

!listing examples/ex05b/beam_constraint.i

The mesh is generated by using the [BeamMeshGenerator.md], which generates a beam mesh shown in [fig:stickmodel5bnew] from a text file. Each row of the text file following `# Nodes` corresponds to a node and contains the node id, x, y and z coordinates of the node, and nodeset IDs that this node belongs to. Each row following the `# Element Linear` line corresponds to one beam element and contains the element id, node ids of the two nodes that this element connects, and the block id that this element belongs to. The text file defining the beam mesh, and an illustration of the generated mesh are below.

!listing examples/ex05b/stick_model.txt

!media media/examples/5b/stickmodel5bnew.png
       style=width:50%;margin-left:200px;float:center;
       id=fig:stickmodel5bnew
       caption=1D stick model using BeamMeshGenerator.

The other sections of the input file are identical to those in [Example 5a](examples/example5a) and are explained there.

## Results

The response spectra at the top of the internal and containment structures obtained from this example and Example 5a are presented here. `FileMesh` corresponds to the results calculated when the mesh is imported from Cubit, and `BeamMeshGenerator` corresponds to the results calculated in this example.

!row!

!col! small=12 medium=6 large=6
!media media/examples/5b/internalx.png
       style=width:400px id=fig:internalx
       caption=Comparison of the response spectra at at an elevation of 22 ft at internal structure in X direction
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/5b/internaly.png
       style=width:400px id=fig:internaly
       caption=Comparison of the response spectra at at an elevation of 22 ft at internal structure in Y direction
!col-end!

!row-end!

!row!

!col! small=12 medium=6 large=6
!media media/examples/5b/internalz.png
       style=width:400px id=fig:intrnalz
       caption=Comparison of the response spectra at at an elevation of 22 ft at internal structure in Z direction
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/5b/containmentx.png
       style=width:400px id=fig:containmentx
       caption=Comparison of the response spectra at at an elevation of 61 ft at internal structure in X direction
!col-end!

!row-end!

!row!

!col! small=12 medium=6 large=6
!media media/examples/5b/containmenty.png
       style=width:400px id=fig:containmenty
       caption=Comparison of the response spectra at at an elevation of 61 ft at internal structure in Y direction
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/5b/containmentz.png
       style=width:400px id=fig:containmentz
       caption=Comparison of the response spectra at at an elevation of 61 ft at internal structure in Z direction
!col-end!

!row-end!

!bibtex bibliography
