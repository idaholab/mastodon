# Newmark Time Integration Verification

## Problem Statement and Results

In this one element test, the base of the 2D element is fixed in both x and y directions. The top of
the block is also fixed in the y direction and a sawtooth acceleration is applied in the x direction.

The prescribed acceleration is integrated in time using Newmark time integration.
For $\beta = 0.25$ and $\gamma = 0.5$ and timestep of 1.0 sec., the displacement and velocity
are as given in [newmark-disp-vel-accel]. The interactive results shown in
[verification_figure] compare the analytical result ("previous") with the result computed
from MASTODON for this problem ("current").

!table id=newmark-disp-vel-accel caption=Analytical displacement, acceleration, and velocity in the x-direction.
| t | Displacement | Velocity | Acceleration |
| - | - | - | - |
| 0 | 0    | 0   |  0 |
| 1 | 0.25 | 0.5 |  1 |
| 2 | 1    | 1   |  0 |
| 3 | 1.75 | 0.5 | -1 |

!plot scatter filename=test/tests/kernels/time_integration/gold/newmark_out.csv
              data=[{'x':'time', 'y':'accel_x', 'name':'Acceleration'},
                    {'x':'time', 'y':'disp_x', 'name':'Displacement'},
                    {'x':'time', 'y':'vel_x', 'name':'Velocity'}]
              layout={'xaxis':{'title':'Time (sec)'},
                      'yaxis':{'title':'Acceleration (g)'},
                      'title':'Acceleration history'}
              caption=Acceleration, velocity and displacement values computed using NewmarkBeta integration in MASTODON
              id=verification_figure

## Complete Input File

!listing test/tests/kernels/time_integration/newmark.i
         start=[Mesh]
