## Earthquake fault rupture

The orientation of an earthquake fault is described using three directions - strike ($\phi_s$), dip
($\delta$) and slip direction ($\lambda$) as shown in [fig:fault_orientation], which is courtesy of
[!citet](aki2012quantitative).

!media media/fault_orientation.png
       style=width:80%;margin-left:100px;
       id=fig:fault_orientation
       caption=Definition of the fault-orientation parameters - strike $\phi_s$, dip $\delta$ and
               slip direction $\lambda$. The slip direction is measured clockwise around from north,
               with the fault dipping down to the right of the strike direction. Strike direction is
               also measured from the north. $\delta$ is measured down from the horizontal.

In MASTODON, earthquake fault is modeled using a set of point sources. The seismic moment ($M_o$) of
the earthquake point source in the fault specific coordinate system is:

\begin{equation}
M_o(t) = \mu A \bar{u}(t)
\end{equation}

where, $\mu$ is the shear modulus of the soil, $A$ is the area of fault rupture and $\bar{u}(t)$ is
the spatially averaged slip rate of the fault.

When this seismic moment is converted into the global coordinate system (x, y and z) with the x
direction oriented along the geographic north and z direction along the soil depth, the resulting
moment can be written in a symmetric $3 \times 3$ matrix form whose components are as follows:

\begin{equation}
\begin{aligned}
M_{xx}(t) &= -M_o(t)(\sin \delta \cos \lambda \sin2 \phi_s + \sin 2\delta \sin\lambda \sin^2 \phi_s) \\
M_{xy}(t) &= M_{yx}(t) = M_o(t)(\sin\delta \cos \lambda \cos 2 \phi_s + \frac{1}{2} \sin 2\delta \sin \lambda \sin 2 \phi_s) \\
M_{xz}(t) &= M_{zx}(t) = -M_o(t)(\cos \delta \cos \lambda \cos \phi_s + \cos 2\delta \sin \lambda \sin 2\phi_s) \\
M_{yy}(t) &= M_o(t)(\sin \delta \cos \lambda \sin 2 \phi_s - \sin 2 \delta \sin \lambda \cos^2 \phi_s) \\
M_{yz}(t) &= M_{zy}(t) = -M_o(t)(\cos \delta \cos \lambda \sin \phi_s - \cos 2\delta \sin\lambda \cos\phi_s) \\
M_{zz}(t) &= M_o(t) \sin 2\delta \sin \lambda
\end{aligned}
\end{equation}

Each component of the above matrix is a force couple with the first index representing the force
direction and the second index representing the direction in which the forces are separated (see
[fig:source_direction]; 
[!citet](aki2012quantitative)).

!media media/source_direction.png
       style=width:60%;margin-left:150px;
       id=fig:source_direction
       caption=The nine different force couples required to model an earthquake source.

The total force in global coordinate direction $i$ resulting from an earthquake source applied at
point $\vec{\zeta}$ in space is then:

\begin{equation}
f_i(\vec{x}, t) = - \sum_{j=1}^{3} \frac{\partial M_{ij}(\vec{x}, t)}{\partial x_j} = \sum_{j=1}^{3} M_{ij}(t) \frac{\partial \delta (\vec{x} - \vec{\zeta})}{\partial x_j}
\end{equation}
where, $\delta(.)$ is the delta function in space.

When many earthquake sources are placed on the earthquake fault, and they rupture at the same time
instant, then an approximation to a plane wave is generated. If one of the point sources is specified
as the epicenter and the rupture speed ($V_r$) is provided, then the other point sources start
rupturing at $d/V_r$, where $d$ is the distance between the epicenter and the other point source.
