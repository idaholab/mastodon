#### Seismic force

In some cases, the ground excitation is measured at a rock outcrop (where rock is found at surface
level and there is no soil above it). To apply this to a location where rock is say $10$m deep and
there is soil above it, a sideset is created at $10$m depth and the ground excitation (converted into
a stress) is applied at this depth. To apply ground excitation as a stress, the input function should
be given as ground velocity.

To convert a velocity applied normal to the boundary into a normal stress, the normal stress equation above can be used. Using a similar argument as discussed in the section above, to
convert a velocity applied tangential to the boundary into a shear stress, Equation
[eqn:shear_stress] can be used.

\begin{equation}
\label{eqn:shear_stress}
\tau = \rho  V_s  \frac{du}{dt}
\end{equation}
where, $V_s$ is the shear wave speed and $\tau$ is the shear stress.

In some situations, the ground motion measured at a depth within the soil is available. This ground
motion is the summation of the wave that enters and exits the soil domain. MASTODON has the
capability to extract the incoming wave from the within soil ground motion. To calculate the incoming
wave velocity, an iterative procedure is used. The initial guess for the incoming wave velocity
($v_i$) at time t is taken to be the same as the within soil velocity measured at that location. The
velocity at this boundary obtained from MASTODON ($v_{mastodon}$) is now going to be different from
the measured within soil velocity ($v_{measured}$) at time t. Half the difference between
$v_{mastodon}$ and $v_{measured}$ is added to $v_o$ and the iterations are continued until $v_i$
converges (within a numerical tolerance).
