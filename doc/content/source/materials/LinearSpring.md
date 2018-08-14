# Linear Spring Material

## Description

The Linear Spring Material simulates linear two-noded spring with a diagonal stiffness
matrix including three elongation and three rotation stiffnesses. The material takes
the six stiffnesses as input, and these stiffnesses can be provided either as
constants or as functions of space. When provided as functions of space, MASTODON will
evaluate the stiffnesses of the springs by evaluating these functions at the coordinates
of the midpoint of the spring element.

## Limitations

The linear spring element is currently limited to simulations involving small
rigid body rotations of the spring. The inertia of the spring element can be
simulated by modeling equivalent nodal masses using the `NodalTranslationalInertia` kernel.
Providing density in the Materials block is not sufficient for inertia to be included.

## Usage

The Materials block in the input file shown below demonstrates the usage of the
linear spring material. In the input file, sawtooth-type force and moment histories
are applied at one node of the spring, while the other node is fixed. Given the
stiffnesses of the spring, the same saw-tooth shape displacement and rotation histories
are calculated in the output.

!listing test/tests/materials/linear_spring/spring_static.i start=NodalKernels end=Preconditioning

!syntax children /Materials/LinearSpring

!syntax parameters /Materials/LinearSpring

!syntax inputs /Materials/LinearSpring
