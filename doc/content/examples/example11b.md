# Example 11b: Fluid-structure interaction analysis of a tank subjected to uni-directional earthquake acceleration

!alert note title=Units of this example
Giga kilograms, Giga Newtons, meters, and seconds were used for the modeling. However, the
post-processed results maybe presented in different units.

## Modeling

This example uses the same model and kernels as discussed in [example 11a](examples/example11a.md).
 The major difference is from [example 11a](examples/example11a.md) is, instead of
using a harmonic acceleration, an earthquake acceleration corresponding to the
ElCentro earthquake is used. The input file is presented below.

!listing examples/ex11b/FSI_ex11b.i

## Input motion

The bottom of the tank is subjected to an ElCentro earthquake acceleration with
peak amplitude of 0.2 $g$. The length of the input motion is about 7 $s$. [fig:input] presents
 a plot of the input motion.

!media media/examples/ex11b_input.png
 style=width:50%;float:center;
 id=fig:input
 caption=Input harmonic motion to the tank.

## Results: Pressures on the tank and wave heights over the fluid

Pressures acting on the tank wall as a function of time are recorded at the circular
red dot as illustrated in [example 10a](examples/example10a.md). Wave heights as the function of time are also
recorded at the triangular red dot as illustrated in [example 10a](examples/example10a.md). [fig:pressure] and [fig:wave]
present the pressures and wave heights, respectively.

!row!

!media media/examples/ex11b_pressure.png
 style=width:40%;float:left;padding-top:2.5%;
 id=fig:pressure
 caption=Pressures recorded at the circular dot as illustrated in [example 10a](examples/example10a.md).

!media media/examples/ex11b_wave.png
 style=width:40%;float:right;padding-top:2.5%;
 id=fig:wave
 caption=Wave heights recorded at the triangular dot as illustrated in [example 10a](examples/example10a.md).

!row-end!

## Results: Comparison to a reference solution

[fig:comp] presents the peak pressure and peak wave height in comparison to a reference solution.
It is observed that the modeled results match quite well with the reference solution.

!media media/examples/ex11b_comp.png
 style=width:50%;float:center;
 id=fig:comp
 caption=Comparison of the modeled results to a reference solution.
