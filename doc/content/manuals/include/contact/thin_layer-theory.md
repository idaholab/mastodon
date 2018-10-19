### Thin-layer method

An efficient approach to modeling the foundation-soil interface is to create a thin layer of the
I-Soil material at the interface, as illustrated in [fig:thin_layer] below.

!media media/theory/thin_layer.png
       style=width:60%;margin-left:100px;float:center;
       id=fig:thin_layer
       caption=Modeling the foundation-soil interface as a thin layer for a sample surface foundation.

The red layer between the foundation (green) and soil (yellow) is the thin layer of I-Soil. The
properties of this thin layer are then adjusted to simulate Coulomb friction between the
surfaces. The Coulomb-friction-type behavior can be achieved by modeling the material of the thin
soil layer as follows:

1. Define an I-Soil material with a user-defined stress-strain curve.

2. Calculate the shear strength of the thin layer as $\tau_{max}=\mu \sigma_N$ , where $\tau_{max}$
   is the shear strength, $\mu$ is the friction coefficient, and $\sigma_N$ is the normal stress on
   the contact surface. The shear strength of the thin layer is the stress at which sliding starts at
   the interface. Therefore, this shear strength should be proportional to the normal stress to
   simulate Coulomb friction. This can be achieved by setting the initial shear strength equal to the
   reference pressure, $p_{ref}$. The reference pressure can then be set to an arbitrary positive
   value, such as the average normal stress at the interface from gravity loads. The shear strength
   will then follow the equation

   \begin{equation}
   \tau_{max} = \mu p_{ref}
   \end{equation}

3. Define the stress-strain curve to be almost elastic-perfectly-plastic, and such that the shear
   modulus of the thin layer is equal to the shear modulus of the surrounding soil, in case of an
   embedded foundation. If the foundation is resting on the surface such as in [fig:thin_layer]
   above, the shear modulus of the thin layer soil should be as high as possible, such that the
   linear horizontal foundation stiffness is not reduced due to the presence of the thin layer. A
   sample stress-strain curve is shown in [fig:thin_layer_stress_strain] below. The sample curve in
   the figure shows an almost bilinear shear behavior with gradual yielding and strain hardening,
   both of which, are provided to reduce possible high-frequency response. High-frequency response is
   likely to occur if a pure Coulomb friction model (elastic-perfectly-plastic shear behavior at the
   interface) is employed, due to the sudden change in the interface shear stiffness to zero.

!media media/theory/thin_layer_stress_strain.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:thin_layer_stress_strain
       caption=Sample shear-stress shear-strain curve for modeling the thin-layer interface using I-Soil.

4. Turn on pressure dependency of the soil stress-strain curve and set $a_0$, $a_1$ and $a_2$ to 0, 0
   and 1, respectively. This ensures that the stress-strain curve scales linearly with the normal
   pressure on the interface, thereby also increasing the shear strength in the above equation
   linearly with the normal pressure, similar to Coulomb friction.

5. Use a large value for the Poisson’s ratio, in order to avoid sudden changes in the volume of the
   thin-layer elements after the yield point is reached. A suitable value for the Poisson’s ratio can
   be calculated by trial and error.

Following the above steps should enable the user to reasonably simulate geometric
nonlinearities. These steps will be automated in MASTODON in the near future.
