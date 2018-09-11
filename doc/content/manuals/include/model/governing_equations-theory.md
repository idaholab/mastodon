## Governing equations

The basic equation that MASTODON solves is the nonlinear wave equation:

\begin{equation}
\label{eqn:governing_equation}
 \rho \mathbf{\ddot{u}} + \nabla \cdot \sigma = \mathbf{F_{ext}}
\end{equation}

where, $\rho$ is the density of the soil or structure that can vary with space, $\sigma$ is the stress at any point in space and time, $\mathbf{F_{ext}}$ is the external force acting on the system that can be in the form of localized seismic sources or global body forces such as gravity, and $\mathbf{\ddot{u}}$ is the acceleration at any point within the soil-structure domain. The left side of the equation contains the internal forces acting on the system with first term being the contribution from the inertia, and the second term being the contribution from the stiffness of the system. Additional terms would be added to this equation when damping is present in the system. The material stress response ($\sigma$) is described by the constitutive model, where the stress is determined as a function of the strain ($\epsilon$), i.e. $\sigma(\epsilon)$. Details about the material constitutive models available in MASTODON are presented in the section about [material models](#material-models).

The above equation is incomplete and ill-conditioned without the corresponding boundary conditions. There are two main types of boundary conditions: (i) Dirichlet boundary condition which is a kinematic boundary condition where the displacement, velocity, or acceleration at that boundary is specified; (ii) Neumann boundary condition where a force or traction is applied at the boundary. All the special boundary conditions such as absorbing boundary condition are specialized versions of these broad boundary condition types.
