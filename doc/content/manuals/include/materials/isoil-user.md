### The I_Soil System

The I-soil material model is a nonlinear hysteretic soil model that is based on the distributed
element models developed by [!citet](iwan1967on) and [!citet](chiang1994anew). In 1-D, this model takes
the backbone stress-strain curve and divides it into a set of elastic-perfectly plastic curves. The
total stress then is the sum of the stresses from the individual elastic-perfectly plastic curves.

The three dimensional generalization of this model is achieved using von-Mises failure criteria for
each elastic-perfectly plastic curve resulting in an invariant yield surfaces in three-dimensional
stress space like in [fig:yieldsurface] (after [!citet](chiang1994anew)).

!media media/yield_surface.png
       style=width:40%;float:right;margin:20px;padding:20px;
       id=fig:yieldsurface
       caption=Invariant yield surfaces of the individual elastic-perfectly curves.

The following options are available for an automatic creation of the backbone curve:

1. User-defined backbone curve (soil_type = 'user_defined'): The backbone curve can be provided in a
   csv file where the first column is shear strain and the second column is shear stress. The number
   of elastic-perfectly plastic curves that will be generated from this backbone curve depends on the
   number of entries in the data file. When many soil layers are present, a vector of data files can
   be provided as input. The size of this vector should equal the number of soil layers. Also the
   number of entries in each data file should be the same.

2. Darendeli backbone curve (soil_type = 'darendeli'): The backbone curve can be auto-generated based
   on empirical data for common soil types. [!citet](darendeli2001development) presents a functional
   form that can be used to create the backbone shear stress - strain curves based on the
   experimental results obtained from resonant column and torsional shear tests. This functional form
   requires the initial shear modulus, initial bulk modulus, plasticity index, over consolidation
   ratio, reference mean confining pressure (p_ref) and number of points as input. Other than the
   number of points, all the other parameters can be provided as a vector for each soil layer. The
   number of points, which determines the number of elastic-perfectly plastic curves to be generated,
   is constant for all soil layers.

3. General Quadratic/Hyperbolic (GQ/H) backbone curve (soil_type = 'gqh'):
   [!citet](darendeli2001development) study constructs the shear stress-strain curves based on
   experimentally obtained data. At small strains the data is obtained using resonant column test,
   and towards the medium shear strain levels the torsional shear test results are used. The values
   are extrapolated at the large strain levels. This extrapolation may underestimate or overestimate
   the shear strength at large strains. Therefore, shear strength correction is necessary to account
   for the correct shear strength at large strains. GQ/H model proposed by
   [!citet](groholski2016simplified) has a curve fitting scheme that automatically corrects the
   reference curves provided by [!citet](darendeli2001development) based on the specific shear strength
   at the large strains. This model requires taumax, theta_1 through 5, initial shear modulus,
   initial bulk modulus and number of points as input. The parameter taumax is the maximum shear
   stress that can be generated in the soil. The parameters theta_1 through 5 are the curve fitting
   parameters and can be obtained using DEEPSOIL [!citep](hashash2016deepsoil). Other than the number
   of points, all the other parameters can be given as a vector for the different soil layers. The
   number of points, which determines the number of elastic-perfectly plastic curves to be generated,
   is constant for all soil layers.

4. Thin-layer friction backbone curve (soil_type = 'thin_layer'): This backbone curve is should be used for
   the simulation of the foundation-soil interfaces using a formulation similar to Coulomb
   friction. This option should be used for a thin layer of I-soil elements along the foundation-soil
   interface. Using the 'thin_layer' option creates a bilinear backbone curve with a pre-yield shear
   modulus equal to the the initial_shear_modulus, and a post-yield shear modulus equal to
   initial_shear_modulus * hardening_ratio. The shear stress at yield is set to be equal to
   friction_coefficient * instantaneous pressure. Using the 'thin_layer' option for the I-Soil
   backbone curve automatically sets the values pressure_dependency = 'true', $a_0 = 0$, $a_1 = 0$
   and $a_2 = 1$. This ensures that the shear strength dependence is turned on, and the strength of
   the thin layer increases linearly with instantaneous pressure. Note that since the strength is
   directly proportional to pressure (and not normal stress), this is not identical to Coulomb
   formulation.

All the above backbone curves provide the behavior of the soil at a reference confining pressure
($p_{ref}$). When the confining pressure of the soil changes, the soil behavior also changes (if
pressure dependency is turned on). The shear modulus ($G(p)$) at a pressure $p$ is given by:

\begin{equation}
G(p) = G_0 (\frac{p-p_0}{p_{ref}})^{b_{exp}},
\end{equation}

where $G_0$ is the initial shear modulus, $p_0$ is the tension pressure cut off and $b_{exp}$ is a
parameter obtained from experiments. The shear modulus reduces to zero for any pressure lower than
$p_0$ to model the failure of soil in tension. Note that compressive pressure is taken to be
positive.

Similarly, the yield stress ($\sigma_y(p)$) of the elastic-perfectly plastic curve also changes when
the confining pressure changes. The yield stress ($\sigma_y(p)$) at a pressure $p$ is given as:

\begin{equation}
\sigma_y(p) = \sqrt{\frac{a_0 + a_1 (p-p_0) + a_2 (p-p_0)^2}{a_0 + a_1 p_{ref} + a_2 {p_{ref}}^2}} \sigma_y(p_{ref}) ,
\end{equation}

where $a_0$, $a_1$ and $a_2$ are parameters obtained from experiments.

To include pressure-dependent stiffness and yield strength calculation, pressure_dependency should be
set to true and b_exp, a_0, a_1, a_2, tension_pressure_cut_off and p_ref need to be provided as
input. Other than p_ref, all the other parameters are the same for all the soil layers. p_ref can be
provided as vector with information about each soil layer.
