#### Nonlinear hysteretic constitutive model for soils (I-soil)

I-soil ([citet:numanoglu2017phd]) is a three dimensional, physically motivated, piecewise linearized
nonlinear hysteretic material model for soils. The model can be represented by shear type
parallel-series distributed nested components (springs and sliders) in one dimensional shear stress
space and its framework is analogous to the distributed element modeling concept developed by
[citet:iwan1967on]. The model behavior is obtained by superimposing the stress-strain response of
nested components. Three dimensional generalization follows [citet:chiang1994anew] and uses von Mises
(independent of effective mean stress) and/or Drucker-Prager (effective mean stress dependent) type
shear yield surfaces depending on user's choice. The yield surfaces are invariant in the stress space
[fig:yield_surface] . Thus, the model does not require kinematic hardening rule to model un/reloading
stress-strain response and preserves mathematical simplicity.

!media media/theory/yield_surface.png
       style=width:40%;margin-left:200px;float:center;
       id=fig:yield_surface
       caption=Invariant yield surfaces of the individual elastic-perfectly curves
               (after Chiang and Beck, 1994).

The current version of I-soil implemented in MASTODON utilizes Masing type un/reloading behavior and
is analogous to MAT79 (LS-DYNA) material model but does not exhibit numerical instability observed in
MAT79 ([citet:numanoglu2017conf]). Masing type un/reloading is inherently achieved by the model
because upon un/reloading the yielded nested components regain stiffness and strength. The cyclic
response obtained from current version of the model is presented in
[fig:1D_isoil_representation]. Reduction factor type modification on un/reloading behavior
([citet:phillips2009damping]; [citet:numanoglu2017nonmasing]) is an ongoing study within MASTODON
framework.

!media media/theory/1D_isoil_representation.png
       style=width:60%;margin-left:200px;float:center;
       id=fig:1D_isoil_representation
       caption=I-soil model details: (a) 1D representation by springs; (b) example monotonic and
               cyclic behavior of four nested component model (reprinted from Baltaji et al., 2017).

Main input for the current version of I-soil in MASTODON is a backbone curve at a given reference pressure. MASTODON provides variety of options to built backbone curve for a given soil type and reference pressure using following methods :

1. User-defined backbone curve (soil\_type = 'user_defined'): The backbone curve can be provided in a
   .csv file where the first column is shear strain points and the second column is shear stress
   points. The number of nested components that will be generated from this backbone curve depends on
   the number of discrete shear strain - shear stress pairs defined in the .csv file. When layered
   soil profile is present, .csv file for each reference pressure can be provided to the
   corresponding elements in the mesh.

2. Darendeli backbone curve (soil\_type = 'darandeli'): The backbone curve can be auto-generated
   based on empirical relations obtained from laboratory tests.  [citet:darendeli2001development]
   presents a functional form for normalized modulus reduction curves obtained from resonant column -
   torsional shear test for variety of soils. MASTODON utilizes this study and auto-generates the
   backbone shear stress - strain curves. The inputs for this option are (1) small strain shear
   modulus, (2) bulk modulus, (3) plasticity index, (4) overconsolidation ratio, (5) reference
   effective mean stress ($p_{ref}$) at which the backbone is constructed, and (6) number of shear
   stress - shear strain points preferred by user to construct piecewise linear backbone curve. All
   the other parameters except the number of shear stress - shear strain points can be provided as a
   vector for each soil layer.

   Darendeli (2001) study extrapolates the normalized modulus reduction curves after 0.1% shear
   strains. This extrapolation causes significant over/under estimation of the shear strength implied
   at large strains for different type of soils at different reference effective mean stresses
   [citep:hashash2010recent]. Thus user should be cautious about implied shear strength when
   utilizing this option.

3. General Quadratic/Hyperbolic (GQ/H) backbone curve (soil\_type = 'gqh'):
   [citet:darendeli2001development] study presented in previous item constructs the shear stress -
   shear strain curves based on experimentally obtained data. At small strains, the data is obtained
   using resonant column test, and towards the moderate shear strain levels, torsional shear test
   results are used. Large strain data are extrapolation of the small to medium shear strain
   data. This extrapolation underestimates or overestimates the shear strength mobilized at large
   shear strains. Therefore, implied shear strength correction is necessary to account for the
   correct shear strength at large strains. GQ/H constitutive model proposed by
   [citet:groholski2016simplified] has a unique curve fitting scheme embedded into the constitutive
   model that accounts for mobilized strength at large shear strains by controlling the shear
   strength. This model uses taumax, theta\_1 through 5, small strain shear modulus, bulk modulus and
   number of shear stress - shear strain points preferred by user to construct piecewise linear
   backbone curve. The parameter taumax is the maximum shear strength that can be mobilized by the
   soil at large strains. The parameters theta\_1 through 5 are the curve fitting parameters and can
   be obtained using DEEPSOIL [citep:hashash2016deepsoil]. Other than the number of points, all the
   other parameters can be given as a vector for the different soil layers. The number of points,
   which determines the number of elastic-perfectly plastic curves to be generated, is constant for
   all soil layers.

Once the backbone curve is provided to I-soil, the model determines the properties for nested
components presented in [fig:1D_isoil_representation]. The stress integration for each nested
component follows classic elastic predictor - plastic corrector type radial return algorithm
([citet:simo2006computein]) and model stress is obtained by summing the stresses from each nested
component:

\begin{equation}
\tau = \sum_{k=1}^{i} G_{k} * \gamma +  \sum_{k=i+1}^{n} {\tau_{y}}^k
\end{equation}

where $\tau$ is the total shear stress, $G_{k}$ is the shear modulus of the $k^{th}$ nested
component, $\gamma$ is the shear strain, ${\tau_{y}}^k$ is the yield stress of the $k^{th}$ nested
component, and $i$ represents the number of components that have not yet yielded out of the $n$ total
nested components.

Small strain shear modulus can be varied with effective mean stress via:

\begin{equation}
 G(p) = G_0 \left(\frac{p-p_0}{p_{ref}}\right)^{b_{exp}}
\end{equation}

where, $G_0$ is the initial shear modulus, $p_0$ is the tension pressure cut off and $b_{exp}$ is
mean effective stress dependency parameter obtained from experiments. The shear modulus reduces to
zero for any mean effective stress lower than $p_0$ to model the failure of soil in tension. Note
that the mean effective stress is positive for compressive loading. Thus, $p_0$ should be inputted as
negative.

Yield criteria of the material can also be varied with effective mean stress dependent behavior as:

\begin{equation}
\tau_y(p) = \sqrt{\frac{a_0 + a_1 (p-p_0) + a_2 (p-p_0)^2}{a_0 + a_1 p_{ref} + a_2 {p_{ref}}^2}} \tau_y(p_{ref})
\end{equation}

where, $a_0$, $a_1$ and $a_2$ are parameters that define how the yield stress varies with pressure.
