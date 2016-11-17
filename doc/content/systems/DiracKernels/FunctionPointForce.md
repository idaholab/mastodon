#FunctionPointForce
!description /DiracKernels/FunctionPointForce

## Description
The points at which the force needs to be applied can be supplied in two ways: (i) a single point can be specified as a vector using "point" and (ii) multiple points can be specified by providing the "x_position", "y_position" and "z_position" as three seperate functions and providing the total number of points in "number".

This class then distributes this point force to the nodes of the element that this point belongs to.

!parameters /DiracKernels/FunctionPointForce

!inputfiles /DiracKernels/FunctionPointForce

!childobjects /DiracKernels/FunctionPointForce
