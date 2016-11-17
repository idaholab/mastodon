#SeismicSource

!description /DiracKernels/SeismicSource

## Description
The orientation of an earthquake fault is described using three directions - strike ($\phi_s$), dip ($\delta$) and slip direction ($\lambda$) as shown in Fig. 1.

!image media/fault_orientation.png width=40% float=right margin=20px padding=20px caption=Fig. 1 Definition of the fault-orientation parameters - strike $\phi_s$, dip $\delta$ and slip direction $\lambda$. The slip direction is measured clockwise around from north, with the fault dipping down to the right of the strike direction. Strike direction is also measured from the north. $\delta$ is measured down from the horizontal (image courtesy \citet{aki2012quantitative}).

The seismic moment ($M_o$) of the earthquake point source in the fault specific coordinate system is:

$$ M_o(t) = \mu A \bar{u}(t),$$

where $\mu$ is the shear modulus of the soil, $A$ is the area of fault rupture and $\bar{u}(t)$ is the spatially averaged slip rate of the fault.

When this seismic moment is converted into the global coordinate system (x, y and z) with the x direction oriented along the geographic north and z direction along the soil depth, the resulting moment can be written in a symmetric $3 \times 3$ matrix form whose components are as follows:
$$ M_{xx}(t) = -M_o(t)(\sin \delta \cos \lambda \sin2 \phi_s + \sin 2\delta \sin\lambda \sin^2 \phi_s\\
M_{xy}(t) = M_o(t)(\sin\delta \cos \lambda \cos 2 \phi_s + \frac{1}{2} \sin 2\delta \sin \lambda \sin 2 \phi_s) = M_{yx}(t) \\
M_{xz}(t) = -M_o(t)(\cos \delta \cos \lambda \cos \phi_s + \cos 2\delta \sin \lambda \sin 2\phi_s = M_{zx}(t)\\
M_{yy}(t) = M_o(t)(\sin \delta \cos \lambda \sin 2 \phi_s - \sin 2 \delta \sin \lambda \cos^2 \phi_s \\
M_{yz}(t) = -M_o(t)(\cos \delta \cos \lambda \sin \phi_s - \cos 2\delta \sin\lambda \cos\phi_s) = M_{zy}(t) \\
M_{zz}(t) = M_o(t) \sin 2\delta \sin \lambda $$

Each component of the above matrix is a force couple with the first index representing the force direction and the second index representing the direction in which the forces are separated (see Fig. 2).

!image media/source_direction.png width=40% float=right margin=20px padding=20px caption=Fig. 2 The nine different force couples required to model an earthquake source (image courtesy \citet{aki2012quantitative}).

The total force in global coordinate direction $i$ resulting from an earthquake source applied at point $\vec{\zeta}$ in space is then:
$$f_i(\vec{x}, t) = - \sum_{j=1}^{3} \frac{\partial M_{ij}(\vec{x}, t)}{\partial x_j} = \sum_{j=1}^{3} M_{ij}(t) \frac{\partial \delta (\vec{x} - \vec{\zeta})}{\partial x_j}, $$
where $\delta(.)$ is the delta function in space.

!parameters /DiracKernels/SeismicSource

!inputfiles /DiracKernels/SeismicSource

!childobjects /DiracKernels/SeismicSource

## References
\bibliographystyle{unsrt}
\bibliography{bib/mastodon.bib}
