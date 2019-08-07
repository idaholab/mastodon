# Example 6c: Force-controlled frictional contact problem.

## Model Description

This example demonstrates frictional contact between a solid block and elastic soil in MASTODON. A 6 inch cubic block is centered on top of a 4 foot cubic block of soil. The top surface of the soil is free and the remaining surfaces are fixed in all directions. A uniform normal pressure of 5 psi is applied to the top of the smaller solid block and force is applied to the block in direction parallel to the contact surface. The resulting normal and frictional forces at the interface of the two materials are then obtained and compared with analytical results.

## Modeling in MASTODON

The simulation is carried out in two steps.

## Step 1: Stabilizing the system under the gravity and pressure forces

!listing examples/ex06c/stabilize_lc.i

 A static analysis is carried out in the mesh shown in [fig:ex06input] in the first step to stabilize the system under the action of constant gravitational force and pressure force as explained in [Example 6b](examples/example6b)

!media media/examples/ex06/ex06_input.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:ex06input
       caption=Input model in MASTODON.

## Step 2: Performing the dynamic simulation using the stabilized system

!listing examples/ex06c/actual_simulation_lc.i

Advanced restart is used to continue the simulation as discussed in [Example 6b](examples/example6b). A linearly ramping force from 0 lbf to 155 lbf is applied to the solid block.

## Results

The graph for the frictional force as a function of displacement obtained from MASTODON is shown in [frictional_force].

!plot scatter filename=examples/ex06c/results55.csv
              data=[{'x':'force2x', 'y':'tang_forc_x'}]
              layout={'xaxis':{'title':'Applied Force (lbf)'},'yaxis':{'title':'Frictional Force (lbf)'}}
              caption=Graph showing the frictional force as a function of applied force for Columb Friction Model
              id=frictional_force


## Theoretical Solution

Normal Force = density x volume x gravity + pressure x area = 198.7473 lbf

Maximum Frictional Force = coefficient of friction x Normal Force = 139.123 lbf

The frictional force increases linearly from 0 to 139.123 and remains constant after the block starts sliding. The same behavior is seen in the [frictional_force] obtained from MASTODON.
