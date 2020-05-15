# NonReflectingBC

!syntax description /BCs/NonReflectingBC

## Description

This BC models a Lysmer damper [!citep](lysmer1969finite) that absorbs the waves hitting a boundary. To understand Lysmer dampers, let us consider a uniform, linear elastic soil column and say a 1D vertically propagating P wave is traveling through this soil column. Then the normal stress at any location in the soil column is given by:

\begin{equation}
\label{eqn:wave}
\sigma = E \epsilon = E \frac{du}{dx} = \frac{E}{V_p} \frac{du}{dt}= \rho V_p \frac{du}{dt}
\end{equation}

where $E$ is the Young's modulus, $\sigma$ is the normal stress, $\epsilon$ is the normal strain, $\rho$ is the density, $V_p = \sqrt{\frac{E}{\rho}}$ is the P-wave speed and $\frac{du}{dt}$ is the particle velocity. Note that for a 2D or a 3D problem, the P-wave speed is

\begin{equation}
\label{eqn:vp}
V_p = \sqrt{\frac{E(1-\nu)}{(1+\nu)(1-2\nu)}}
\end{equation}

Note that stress in the above equation is directly proportional to the particle velocity, which makes this boundary condition analogous to a viscous damper with damping coefficient of $\rho V_p$. Therefore, truncating the soil domain and placing this damper at the end of the domain is equivalent to simulating wave propagation in an infinite soil column provided the soil is made of linear elastic material and the wave is normally incident on the boundary.

If the soil is not linear elastic or if the wave is incident at an angle, the waves are not completely absorbed by the Lysmer dampers. However, if this non-reflecting boundary is placed sufficiently far from the source, the reflected waves will be dissipated through Rayliegh damping or material hysteresis (provided the soil material models hysteresis) before it reaches the region of interest again.

!syntax parameters /BCs/NonReflectingBC

!syntax inputs /BCs/NonReflectingBC

!syntax children /BCs/NonReflectingBC

!bibtex bibliography
