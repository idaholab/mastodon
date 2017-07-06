#SeismicForce
!syntax description /BCs/SeismicForce

## Description
This boundary condition takes the ground velocity in a particular coordinate direction as input and converts it to a normal/tangential stress and applies this stress at the given boundary. The ground velocity is given as input using the "velocity_function" and the direction in which this velocity has to be applied is given by "vel_component".

To convert the velocity into a stress, let us a consider a uniform infinite linear elastic soil column and let us consider a 1D vertically propagating P wave traveling through this column. The normal stress at any location on the soil column is:

$$
\sigma = E  \epsilon = E  \frac{du}{dx} = \frac{E}{V_p}  \frac{du}{dt} = \rho  V_p  \frac{du}{dt},
$$
where $E$ is the Young's modulus, $\sigma$ is the normal stress, $\epsilon$ is the normal strain, $\rho$ is the density, $V_p = \sqrt{\frac{E}{\rho}}$ is the P-wave speed, $\frac{du}{dt}$ is the particle velocity (or the ground velocity in this case). Note that for a 3D problem, the P-wave speed is $V_p = \sqrt{\frac{E(1-\nu)}{(1+\nu)(1-2\nu)}$.

Similarly, if a vertically propagating shear wave is traveling through the infinite linear elastic soil column, then, the shear stress at any location within the soil column is:

$$
\tau = \rho  V_s  \frac{du}{dt},
$$
where $V_s$ is the shear wave speed and $\tau$ is the shear stress.

If the normal to boundary is not along "vel_component", then the given velocity is split into two components, one normal and one tangential to the boundary and the normal and shear stresses are then obtained using the above equations. The direction of these stresses are dependent on the orientation of the boundary. So, these stress have to be brought back to the global coordinate system using coordinate transformation and the component of the stress in the global direction given by "component" is assembled to the corresponding location in the residual vector.

!syntax parameters /BCs/SeismicForce actions=0 syntax=0

!syntax inputs /BCs/SeismicForce actions=0 syntax=0

!syntax children /BCs/SeismicForce actions=0 syntax=0
