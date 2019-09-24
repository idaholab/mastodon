## Gravity Loading

Gravity loading can be applied on the domain to achieve the stress state due to gravity. MOOSE
includes built-in commands to apply the gravity as a body force throughout the transient
analysis. The following command can be used to apply the gravitational field on the modeled domain:

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=gravity
         end=AuxKernels

The above command should be embedded inside the [Kernels](manuals/user/index.md) command
block. variable = disp_z activates the gravity in z direction and value = -9.81 is assigned to
specify the magnitude and direction of the gravitational field.

Gravity command applies an external body force along the specified direction on the domain of
interest. In order to bring the system to equilibrium, a transient analysis with some viscous damping
is necessary. This approach causes fluctuations on the stress and strains at the beginning of the
analysis because of the elements being initially at zero stress state. Viscous damping removes the
fluctuations and brings the system to equilibrium. Once the system equilibrates, the gravity stresses
are obtained along with the displacements due to the gravity loading. "Initial Stress" command is
available in MASTODON framework to eliminate the need for a separate transient analysis. In addition,
no displacements, or strains due to gravity result.
