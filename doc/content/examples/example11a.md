# Example 11a: Effect of Mesh Refinement and Element Type

This example demonstrates a 3D cantilever beam subject to static and dynamic loading. This example was created to show some modeling best practices and things to be aware of when simulating structures or other 3D geometry in dynamic, high-bending type scenarios - particularly when continuum elements are used in lieu of shell elements.

This example contains two models. The first is a static simulation where a single point load is applied at the free end of a cantilever beam and the displacement is measured. The other utilizes the same beam, but an Ormsby wavelet is applied at the fixed end in order to determine the natural frequency of the beam.

Varied levels of mesh refinement as well as linear and quadratic element types are explored for each model. Results from both are then compared to analytical solutions.

## Static Loading

In this example, a 3D cantilever beam is constructed using the [GeneratedMeshGenerator](meshgenerators/GeneratedMeshGenerator.md). The 5 x 1 x 1 $m$ beam is fixed in all directions at one end where x = 0. A nodeset is generated at the bottom center of the free end of the beam using the [ExtraNodesetGenerator](meshgenerators/ExtraNodesetGenerator.md), which is necessary for application of the point load.

!listing examples/ex11a/static/3d_beam_static_first_order.i
         start=Mesh
         end=GlobalParams

The mesh is constructed such that the beam contains 10 elements along the length of the beam (x) and two elements through the thickness in the out of plane direction (z), which is necessary in order to get a node inline with the neutral axis, thus avoiding torquing the beam when loading. The loading is applied in the y direction, so the number of y elements will be varied for this demonstration in order to determine the number of elements necessary to fully capture the bending.

[fig:static_beam] shows the coarsest and finest mesh with magnified displacements for the beam with HEX8 elements.

!media media/examples/ex11a/static_beam.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:static_beam
       caption=Static beam models with varied level of mesh refinement.

The analytical expression for maximum deflection of a cantilever beam is given below:
\begin{equation}
\Delta = \frac {PL^3}{3EI} \; (1 + \frac {3}{alpha})
\end{equation}
For this beam, the dimensionless parameter alpha is given by:
\begin{equation}
alpha = \frac {kAGL^2}{EI} = 98.039
\end{equation}
The value of alpha is not high  enough for the beam to behave like a thin beam where shear effects are not significant. Hence, the shear effects are considered and the small deformation analytical deflection of a cantilever beam is given by:
\begin{equation}
\Delta = 5.15 \times 10^{-5} m
\end{equation}

This model was setup and run for multiple levels of mesh refinement, also with both HEX8 (linear) and HEX20 (quadratic) element types. Refinement is only added in the y direction, so as to improve the measure of bending in the beam. For HEX8 elements, 1,2,4 and 8 elements are used in the thickness, and for HEX20 elements, 1 and 2 elements are used. The results of the mesh refinement study are shown in the table below:

!table id=static_results_linear caption= HEX8 (Linear) elements refinement study.
| Elements | Displacement (m) | Error (%) | Run time (s) |
| - | - | - | - |
| 1 | 4.1956e-05 | 18.58 | 0.1 |
| 2 | 4.4655e-05 | 13.34 | 0.11 |
| 4 | 4.5735e-05 | 11.25 | 0.141 |
| 8 | 4.6060e-05 | 10.62 | 0.188 |


!table id=static_results_quadratic caption= HEX20 (Quadratic) elements refinement study.
| Elements | Displacement (m) | Error (%) | Run time (s) |
| - | - | - | - |
| 1 | 5.1020e-05 | 0.99 | 0.207 |
| 2 | 5.1696e-05 | 0.32 | 0.339 |

It is worth noting that HEX27 elements were also tested and they produced nearly identical results to those of HEX20 elements. The differences between the first order and second order element models are subtle but important, specifically the element type (first order shown near the start of this example) and order of the displacement variables (second order shown below).

!listing examples/ex11a/static/3d_beam_static_second_order.i
         start=Variables
         end=BCs

## Dynamic Loading

Similar to the example shown above, this model utilizes the same beam, and follows the same mesh refinement strategy, the difference is that this beam is subject to an Ormsby Wavelet at one end and free at the other end. The response is measured at the free end in order to determine the natural frequency of the beam, which will appear as distinct peaks in the plotted response spectrum. The first natural frequency of the beam can be computed using the analytical expression shown below:

\begin{equation}
f_{n_f} = \frac {1.875^2}{2\pi}\sqrt{\frac {EI}{\rho AL^4}} = 18.65Hz
\end{equation}

The Ormsby wavelet is created using the [OrmsbyWavelet](functions/OrmsbyWavelet.md) function type.

!listing examples/ex11a/dynamic/3d_beam_dynamic_first_order.i
         start=Functions
         end=Materials

The wavelet is applied as an acceleration at the fixed end of the beam using the [PresetAcceleration](bcs/PresetAcceleration.md) boundary condition.

!listing examples/ex11a/dynamic/3d_beam_dynamic_first_order.i
         start=BCs
         end=Functions

The simulation results are captured using [VectorPostprocessors](VectorPostprocessors/index.md), specifically the [ResponseHistoryBuilder](vectorpostprocessors/ResponseHistoryBuilder.md) and [ResponseSpectraCalculator](vectorpostprocessors/ResponseSpectraCalculator.md)

!listing examples/ex11a/dynamic/3d_beam_dynamic_first_order.i
         start=VectorPostprocessors
         end=Outputs

!media media/examples/ex11a/dynamic_beam_plot.png
         style=width:100%;margin-right:0px;float:center;
         id=fig:dynamic_beam
         caption=Acceleration response spectrum plot for dynamic beam simulation

Simulation time for each of the models was recorded as well as the resulting first natural frequencies.

!table id=static_results_linear caption= HEX8 (Linear) elements refinement study.
| Elements | Mode 1 (Hz) | Run time (s)|
| - | - | - |
| 1 | 20.24 | 3.206 |
| 2 | 19.73 | 5.376 |
| 4 | 19.40 | 9.788 |
| 8 | 19.39 | 18.282 |


!table id=static_results_quadratic caption= HEX20 (Quadratic) elements refinement study.
| Elements | Mode 1 (Hz) | Run time (s) |
| - | - | - |
| 1 | 18.41 | 23.333 |
| 2 | 18.42 | 46.538 |

This example was created to provide some insight into how bending is captured using continuum elements. For both of these static and dynamic simulations, the second order elements provide a result that is more consistent with the analytical solutions, although the computation cost is much higher. The input file with second order elements is listed below, all previous listings show the first order element model.

!listing examples/ex11a/dynamic/3d_beam_dynamic_second_order.i
