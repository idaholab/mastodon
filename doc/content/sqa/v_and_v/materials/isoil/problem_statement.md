## Problem Statement id=problem-statement

A single hexahedral element with dimensions of 1 meter *x* 1 meter *x* 1 meter was used for all verifications presented in this report. The back surface ($z=0$) of the element was fixed in all three principal directions and special boundary conditions were enforced to ensure the element deformed in perfect shear (see [Periodic Boundary Conditions](manuals/user/index.md#PeriodicBC)). The ramp displacement shown in [applied-displacement] was applied to the front surface ($z=1$) in the X direction.

!plot scatter filename=test/tests/materials/isoil/gold/Displacement2.csv
              data=[{'x':'time', 'y':'displacement', 'name':'Applied Displacement vs. Time'}]
              layout={'xaxis':{'title':'Time (s)'},
                      'yaxis':{'title':'Displacement (m)'},
                      'title':'Applied Displacement vs. Time'}
              id=applied-displacement
              caption=Plot of the ramp displacement boundary condition applied to the element's front surface.

The Newmark-beta time integration scheme, with $\beta = 0.25$ and $\gamma = 0.5$, was used to solve the equation of motion for the element. The theory behind the Newmark-beta method is described [here](manuals/theory/index.md#time-integration). A graphical rendering of an example displacement of the element computed using Newmark's method with I-Soil is shown in [disp-example].

!media media/materials/isoil/isoil_disp_example.png
       style=width:50%;margin-left:auto;margin-right:auto;
       id=disp-example
       caption=Sample deflected shape of the 1 meter *x* 1 meter *x* 1 meter element used in all tests.

The resulting strains and stresses were assumed to be constant throughout the element. Material properties and other parameters which are specific to a given test are described in their respective sections of this report.
