# Example 10b: Cantilever example demonstrating central difference time integration using HEX20 elements

## Model Description

This example demonstrates the central difference time integration using HEX20 elements in MASTODON. A 2 $ft$ x 2 $ft$ x 20 $ft$ cantilever is modeled with the bottom surface fixed along x, y & z directions. A uniform normal pressure of 1 $kip/ft^2$ is applied to the top surface along the axis of the model. The resulting stress time-history and displacement time-history are then obtained at co-ordinate locations (0, 0, 10) and (0, 0, 20), respectively and compared with results from ABAQUS/Standard (since HEX20 elements are unavailable in ABAQUS/Explicit).

[fig:ex10b_input] demonstrates the overall dimensions and boundary conditions of the model.

!media media/examples/MASTODON_ex10_model.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex10b_input
       caption=Input model in MASTODON

## Modeling in MASTODON

!listing examples/ex10b/MASTODON_Explicit_HEX20.i

The automated meshing option provided by MASTODON is used to create the geometry and specify the element type (see the documentation of [GeneratedMeshGenerator](meshgenerators/GeneratedMeshGenerator.md) in syntax for detailed information).

!listing examples/ex10b/MASTODON_Explicit_HEX20.i
         start=Mesh
         end=Variables

The displacement variables are defined in the `Variables` block. The accelerations, velocities and stresses are defined as auxiliary variables in the `AuxVariables` block. HEX20 elements use second order polynomials as shape functions and hence it is necessary to specify the `order=SECOND` for the displacements, accelerations and velocities. Stresses are specified to be of `order=FIRST` since they are one order lower than displacement.

!listing examples/ex10b/MASTODON_Explicit_HEX20.i
         start=Variables
         end=Kernels

The rest of the input file is identical to that of `Example10a`.

## Results

The stress time-history and displacement time-history are obtained at locations (0, 0, 10) and (0, 0, 20) respectively and compared with results from ABAQUS/Standard (as shown in [fig:ex10b_stresszz_0_0_10] and [fig:ex10b_Dispz_0_0_20]).

!media media/examples/Example10b_Stresszz_0_0_10.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex10b_stresszz_0_0_10
       caption=Stress-zz Time-history at (0, 0, 10)

!media media/examples/Example10b_Dispz_0_0_20.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex10b_Dispz_0_0_20
       caption=Disp-z Time-history at (0, 0, 20)
