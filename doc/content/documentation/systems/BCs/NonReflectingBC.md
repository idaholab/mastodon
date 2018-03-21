# NonReflectingBC

!syntax description /BCs/NonReflectingBC

## Description

This boundary condition applies a Lsymer damper [citep:lysmer1969finite] on a given boundary to absorb the waves hitting the boundary. To understand Lsymer dampers, let us consider an uniform linear elastic soil column and say a 1D vertically propagating P wave is traveling through this soil column. Then the normal stress at any location in the soil column is given by:

$$
\sigma = E \epsilon = E \frac{du}{dx} = \frac{E}{V_p} \frac{du}{dt}= \rho V_p \frac{du}{dt}, $$
where $E$ is the Young's modulus, $\sigma$ is the normal stress, $\epsilon$ is the normal strain, $\rho$ is the density, $V_p = \sqrt{\frac{E}{\rho}}$ is the P-wave speed and $\frac{du}{dt}$ is the particle velocity. Note that for a 3D problem, the P-wave speed is $V_p = \sqrt{\frac{E(1-\    nu)}{(1+\nu)(1-2\nu)}$.

The stress in the above equation is directly proportional to the particle velocity which makes this boundary condition analogous to a viscous damper with damping coefficient of $\rho V_p$. So truncating the soil domain and placing this damper at the end of the domain is equivalent to simulating wave propagation in an infinite soil column provided the soil is made of linear elastic material and the wave is vertically incident on the boundary.

If the soil is not linear elastic or if the wave is incident at an angle on the boundary, the waves are not completely absorbed by the Lsymer damper. However, if the non-reflecting boundary is placed sufficiently far from the region of interest, any reflected waves will get damped out by rayliegh damping or hysteretic material behavior before it reaches the region of interest.

!syntax parameters /BCs/NonReflectingBC

!syntax inputs /BCs/NonReflectingBC

!syntax children /BCs/NonReflectingBC



!bibtex bibliography
