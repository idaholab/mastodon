# Theory manual

## Introduction

Multi-hazard Analysis for STOchastic time-DOmaiN phenomena (MASTODON) is
a finite element application that aims at analyzing the response of 3-D
soil-structure systems to natural and man-made hazards such as
earthquakes, floods and fire. MASTODON currently focuses on the
simulation of seismic events and has the capability to perform extensive
'source-to-site' simulations including earthquake fault rupture,
nonlinear wave propagation and nonlinear soil-structure interaction
(NLSSI) analysis. MASTODON is being developed to be a dynamic
probabilistic risk assessment framework that enables analysts to not
only perform deterministic analyses, but also easily perform
probabilistic or stochastic simulations for the purpose of risk
assessment.

MASTODON is a <a href="http://mooseframework.org/"> MOOSE </a>-based application and performs finite-element
analysis of the dynamics of solids, mechanics of interfaces and porous
media flow. It is equipped with effective stress space nonlinear hysteretic soil constitutive  model (I-soil), and a <a href= "https://lwrs.inl.gov/RisckInformed%20Safety%20Margin%20Characterization/INL-EXT-15-36735%20Advanced%20Seismic%20Fragility%20Modeling.pdf"> u-p-U </a> formulation to couple solid and fluid, as well as structural materials such as reinforced concrete. It includes interface models that
simulate gapping, sliding and uplift at the interfaces of solid media
such as the foundation-soil interface of structures. Absorbing boundary models for the simulation of infinite or
semi-infinite domains, fault rupture model for seismic source
simulation, and the domain reduction method for the input of complex,
three-dimensional wave fields are incorporated. Since MASTODON is a <a href="http://mooseframework.org/"> MOOSE </a>-based
application, it can efficiently solve problems using standard
workstations or very large high-performance computers.

This document describes the theoretical and numerical foundations of
MASTODON.

## Governing equations

The basic equation that MASTODON solves is the nonlinear wave equation:

\begin{equation}
\label{eqn:governing_equation}
 \rho \mathbf{\ddot{u}} + \nabla \cdot \sigma = \mathbf{F_{ext}}
\end{equation}

where, $\rho$ is the density of the soil or structure that can vary with space, $\sigma$ is the stress at any point in space and time, $\mathbf{F_{ext}}$ is the external force acting on the system that can be in the form of localized seismic sources or global body forces such as gravity, and $\mathbf{\ddot{u}}$ is the acceleration at any point within the soil-structure domain. The left side of the equation contains the internal forces acting on the system with first term being the contribution from the inertia, and the second term being the contribution from the stiffness of the system. Additional terms would be added to this equation when damping is present in the system. The material stress response ($\sigma$) is described by the constitutive model, where the stress is determined as a function of the strain ($\epsilon$), i.e. $\sigma(\epsilon)$. Details about the material constitutive models available in MASTODON are presented in the section about [material models](#material-models).

The above equation is incomplete and ill-conditioned without the corresponding boundary conditions. There are two main types of boundary conditions: (i) Dirichlet boundary condition which is a kinematic boundary condition where the displacement, velocity, or acceleration at that boundary is specified; (ii) Neumann boundary condition where a force or traction is applied at the boundary. All the special boundary conditions such as absorbing boundary condition are specialized versions of these broad boundary condition types.

## Time integration

To solve Equation [eqn:governing_equation] for $\mathbf{u}$, an appropriate time integration scheme needs to be chosen. Newmark and Hilber-Hughes-Taylor (HHT) time integration schemes are two of the commonly used methods in solving wave propagation problems.

### Newmark time integration

In Newmark time integration [citep:newmark1959amethod], the acceleration and velocity at $t+\Delta t$ are written in terms of the displacement ($\mathbf{u}$), velocity ($\mathbf{\dot{u}}$) and acceleration ($\mathbf{\ddot{u}}$) at time $t$ and the displacement at $t+\Delta t$.

\begin{equation} \label{eqn:Newmark}
\begin{aligned}
\mathbf{\ddot{u}}(t+\Delta t) &= \frac{\mathbf{u}(t+\Delta t)-\mathbf{u}(t)}{\beta \Delta t^2} - \frac{\mathbf{\dot{u}}(t)}{\beta \Delta t}+\frac{\beta -0.5}{\beta}\mathbf{\ddot{u}}(t) \\
\mathbf{\dot{u}}(t+ \Delta t) &= \mathbf{\dot{u}}(t)+ (1-\gamma)\Delta t \mathbf{\ddot{u}}(t) + \gamma \Delta t \mathbf{\ddot{u}}(t+\Delta t)
\end{aligned}
\end{equation}

In the above equations, $\beta$ and $\gamma$ are Newmark time integration parameters. Substituting the above two equations into the equation of motion will result in a linear system of equations ($\mathbf{Au}(t+\Delta t) = \mathbf{b}$) from which $\mathbf{u}(t+\Delta t)$ can be estimated.

For $\beta = 0.25$ and $\gamma = 0.5$, the Newmark time integration scheme is the same as the trapezoidal rule. The trapezoidal rule is an unconditionally stable integration scheme, i.e., the solution does not diverge for any choice of $\Delta t$, and the solution obtained from this scheme is second order accurate. One disadvantage with using trapezoidal rule is the absence of numerical damping to damp out any high frequency numerical noise that is generated due to the discretization of the equation of motion in time.

The Newmark time integration scheme is unconditionally stable for $\gamma \ge \frac{1}{2}$ and $\beta \ge \frac{1}{4}\gamma$. For $\gamma > 0.5$, high frequency oscillations are damped out, but the solution accuracy decreases to first order.

### Hilber-Hughes-Taylor (HHT) time integration

The HHT time integration scheme [citep:hughes2000thefinite] is built upon Newmark time integration method to provide an unconditionally stable and second order accurate numerical scheme with the ability to damp out high frequency numerical noise. Here, in addition to the Newmark equations, the equation of motion is also altered resulting in:

\begin{equation}
\rho\mathbf{\ddot{u}}(t+\Delta t) + \nabla \cdot [(1+\alpha) \sigma(t+\Delta t) - \alpha \sigma(t)] = \mathbf{F_{ext}}(t+\alpha\Delta t)
\end{equation}

Here, $\alpha$ is the HHT parameter. The optimum parameter combination to use for this time integration scheme is $\beta = \frac{1}{4}(1-\alpha)^2$, $\gamma = \frac{1}{2} - \alpha$, and $-0.3 \le \alpha \le 0$.

## Small strain damping

When the soil-structure system (including both soil and concrete)
responds to an earthquake excitation, energy is dissipated in two primary
ways: (1)small-strain and hysteretic material damping, and (2) damping due to gapping,
sliding and uplift at the soil-foundation interface. Dissipation of
energy due to item (1) is modeled (approximately) using following methods: (i) viscous damping for small strain damping experienced at very small strain
levels ($\gamma$ $\leq 0.001 \%$) where the material behavior is largely linear viscoelastic; (ii)
hysteretic damping due to nonlinear hysteretic behavior of the material.
Dissipation of energy due to (2) is discussed in [foundation-soil interface models](#Foundation-soil interface models). This section discusses the damping that is present at small strain levels.


### Rayleigh damping

Rayleigh damping is the most common form of classical damping used in modeling structural dynamic problems. The more generalized form of classical damping, Caughey Damping [citep:caughey1960classical], is currently not implemented in MASTODON. Rayleigh damping is a specific form of Caughey damping that uses only the first two terms of the series. In this method, the viscous damping is proportional to the inertial contribution and contribution from the stiffness. This implies that in the matrix form of the governing equation, the damping matrix ($\mathbf{C}$) is assumed to be a linear combination of the mass ($\mathbf{M}$) and stiffness ($\mathbf{K}$) matrices, i.e., $\mathbf{C} = \eta \mathbf{M} +\zeta\mathbf{K}$. Here, $\eta$ and $\zeta$ are the mass and stiffness dependent Rayleigh damping parameters, respectively.

The equation of motion (in the matrix form) in the presence of Rayleigh damping becomes:
\begin{equation}
\mathbf{M}\mathbf{\ddot{u}}+ (\eta \mathbf{M} + \zeta \mathbf{K})\mathbf{\dot{u}} +\mathbf{K}\mathbf{u} = \mathbf{F_{ext}}
\end{equation}

The same equation of motion at any point in space and time (in the non-matrix form) is given by:
\begin{equation}
\rho\mathbf{\ddot{u}} + \eta \rho \mathbf{\dot{u}} + \zeta  \nabla \cdot \frac{d}{dt}\sigma + \nabla \cdot \sigma = \mathbf{F_{ext}}
\end{equation}

The degree of damping in the system depends on the coefficients $\zeta$ and $\eta$ as follows:
\begin{equation}\label{eqn:general_rayleigh}
\xi (f) = \frac{\eta}{2} \frac{1}{f} + \frac{\zeta}{2} f
\end{equation}

where, $\xi(f)$ is the damping ratio of the system as a function of frequency $f$. The damping ratio as a function of frequency for $\zeta = 0.0035$ and $\eta = 0.09$ is presented in [fig:rayleigh].

!media media/theory/rayleigh.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:rayleigh
       caption=Damping ratio as a function of frequency.


#### Constant damping ratio

For the constant damping ratio scenario, the aim is to find $\zeta$ and $\eta$ such that the $\xi(f)$ is close to the target damping ratio $\xi_t$, which is a constant value, between the frequency range $[f_1, f_2]$. This can be achieved by minimizing the difference between $\xi_t$ and $\xi(f)$ for all the frequencies between $f_1$ and $f_2$, i.e., if

\begin{equation}
I = \int_{f_1}^{f_2} \xi_t - \left(\frac{\eta}{2}\frac{1}{f} + \frac{\zeta}{2}f\right) df
\end{equation}

Then, $\frac{dI}{d \eta} = 0$ and $\frac{dI}{d \zeta}=0$ results in two equations that are linear in $\eta$ and $\zeta$. Solving these two linear equations simultaneously gives:

\begin{equation}
\begin{aligned}
\zeta &= \frac{\xi_t}{2 \pi} \; \frac{3}{(\Delta f)^2} \; \left(f_1 + f_2 - 2 \frac{f_1 f_1}{\Delta f} \; ln \frac{f_2}{f_1}\right) \\
\eta &= 2 \pi \xi_t \; \frac{f_1 f_2}{\Delta_f} \; \left[ln \frac{f_2}{f_1}\; \left(2 + 6 \frac{f_1 f_2}{(\Delta_f)^2}\right) - \frac{3(f_1 + f_2)}{\Delta_f}\right]
\end{aligned}
\end{equation}

#### Damping ratio for soils

Small strain material damping of soils is independent of loading frequency in frequency band of 0.01 Hz - 10 Hz ([cite:menq2003], [cite:shibuya2000damp],[cite:lopresti1997damp], and [cite:marmureanu2000damp]). The two mode Rayleigh damping is frequency dependent and can only achieve the specified damping at two frequencies while underestimating within and overestimating outside of these frequencies. The parameters $\eta$ and $\zeta$ for a given damping ratio can be calculated as follows:

\begin{equation}
  \begin{bmatrix}
    \xi_i \\
    \xi_j
  \end{bmatrix}
  =
  \frac{1}{4\pi}
  \begin{bmatrix}
    \frac {1}{f_i} &  f_i \\
    \frac {1}{f_j} &  f_j
  \end{bmatrix}
  \begin{bmatrix}
    \eta \\
    \zeta
  \end{bmatrix}
\end{equation}

In case of two mode Rayleigh damping, [cite:kwok2007damp] suggests to use natural frequency and five times of it for the soil column of interest. In addition, selecting first mode frequency of soil column and higher frequency that corresponds to predominant period of the input ground motion is a common practice.

Heterogeneities of the wave travel path may introduce scattering effect which leads to frequency dependent damping ([cite:campbell2009damp]). This type of damping is of the form ([cite:withers2015memory]):

\begin{equation}\label{eqn:non_constant_damping}
\xi (f) = \begin{cases}
           \xi_t, & \text{if}\ \; f \le f_T \\
           \xi_t \; \left(\frac{f_T}{f}\right)^\gamma, & \text{if}\ \;f > f_T
           \end{cases}
\end{equation}

where, $f_T$ is the frequency above which the damping ratio starts to
deviate from the constant target value of $\xi_t$, and $\gamma$ is
the exponent which lies between 0 and 1. Minimizing the difference
between [eqn:non_constant_damping] and
[eqn:general_rayleigh] with respect to $\eta$ and $\zeta$ for
all frequencies between $f_1$ and $f_2$ gives:

\begin{equation}
\begin{aligned}
\zeta &= \frac{\xi_t}{2 \pi} \; \frac{6}{(\Delta f)^3} \; [b(f_1,f_2) - a(f_1, f_2) \; f_1 f_2] \\
\eta &= 2 \pi \xi_t \; \frac{2 f_1 f_2}{(\Delta f)^3} \; [a(f_1, f_2)\; ({f_1}^2 + {f_2}^2 + f_1 f_2) - 3 b(f_1, f_2)]
\end{aligned}
\end{equation}

where, the functions $a(f_1, f_2)$ and $b(f_1, f_2)$ are given by:

\begin{equation}
\begin{aligned}
a(f_1, f_2) &= ln \frac{f_T}{f_1} + \frac{1}{\gamma} \; \left[1- \left(\frac{f_T}{f_2}\right)^\gamma\right] \\
b(f_1, f_2) &= \frac{{f_T}^2 - {f_1}^2}{2} + \frac{{f_T}^\gamma}{2-\gamma} \; ({f_2}^{2-\gamma} - {f_T}^{2-\gamma})
\end{aligned}
\end{equation}

Also, $\xi_t$ for soils is inversely proportional to the shear wave
velocity ($V_s$) and a commonly used expression for $\xi_t$ of soil
is:

\begin{equation}
\xi_t = \frac{5}{V_s}
\end{equation}

where, $V_s$ is in m/s.

### Frequency independent damping

As seen in the previous sub-section, the damping ratio using Rayleigh damping varies with frequency. Although the parameters $\eta$ and $\zeta$ can be tuned to arrive at a constant damping ratio for a short frequency range, as the frequency range increases, the damping ratio no longer remains constant. For scenarios like these, where a constant damping ratio is required over a large frequency range, frequency independent damping formulations work better. This formulations is under consideration for adding to MASTODON.

## Soil layers and meshing

Small strain properties (shear wave velocity, small strain modulus etc.) as well as mobilized shear strength of soils change with depth. Thus, in numerical models, soil profile (layers) is constructed to incorporate the depth dependent properties. The ground surface as well as layers that define the soil domain can be horizontal
or non-horizontal. For the horizontal ground surface and layering scenario, the location
of the interfaces can be provided as input and MASTODON will use that
information to generate a set of soil layers, each with a unique
identification number. These layer ids are later used to assign material
properties to each soil layer. The same procedure can also be used for
non-horizontal but planar soil layers by specifying the normal to the
plane and the interface locations measured along the normal direction.

For scenarios where the soil layers are non-horizontal and non-planar, images (.jpg, .png, etc.) of the soil profile can be provided as input. The different soil layers are distinguished from the image by reading either the red, green or blue color value (as per user's directions) at each pixel. Gray scale images in which the red, green and blue values are all the same also work well for this purpose. For creating 3D soil layers, multiple 2D images with soil profiles at different 2-D cross-sections of the soil domain can be provided as input.

Once the soil layers have been distinguished, it is necessary to ensure that the different soil layers are meshed such that they can accurately transmit waves of the required frequency. The optimum element size for each soil layer depends on the type of element used for meshing, cut-off frequency (f) of the wave and the shear wave velocity ($V_s$) of the soil layer. A minimum of 10 points is required per wavelength of the wave to accurately represent the wave in space [citep:coleman2016time]. The minimum wavelength ($\lambda_{min}$) is calculated as:

\begin{equation}
\lambda_{min} = \frac{V_s}{f}
\end{equation}

If linear elements such as QUAD4 or HEX8 are used, then the optimum mesh size is $\lambda_{min}/10$. If quadratic elements such as QUAD9 or HEX27 are used, then the optimum mesh size is $\lambda_{min}/5$. Using the minimum element size information, MASTODON refines the mesh such that the element size criterion is met and at the same time the layers separations are visible. An example of this meshing scheme is presented in [fig:adaptive_meshing] where a 2D soil domain is divided into 3 soil layers and these soil layers are meshed such that the element size criterion is satisfied. A denser mesh is created at the interface between different soil layers.

!media media/theory/adaptive_mesh.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:adaptive_meshing
       caption=Auto-generated mesh for a soil domain with three non-horizontal non-planar soil layers.

## Material models

To model the mechanical behavior of a material, three components need to be defined at every point in space and time - strain, elasticity tensor, stress.

1. **Strain**: Strain is a normalized measure of the deformation experienced by a material. In a 1-D
   scenario, say a truss is stretched along its axis, the axial strain is the elongation of the truss
   normalized by the length of the truss. In a 3D scenario, the strain is 3x3 tensor and there are
   three different ways to calculate strains from displacements - small linearized total strain,
   small linearized incremental strain, and finite incremental strain. Details about these methods
   can be found in [modules/tensor_mechanics/index.md].

2. **Elasticity Tensor**: The elasticity tensor is a 4th order tensor with a maximum of 81
   independent constants. For MASTODON applications, the soil and structure are usually assumed to
   behave isotropically, i.e., the material behaves the same in all directions. Under this
   assumption, the number of independent elastic constants reduces from 81 to 2. The two independent
   constants that are usually provided for the soil are the shear modulus and Poissons's ratio, and
   for the structure it is the Young's modulus and Poisson's ratio.

3. **Stress**: The stress at a point in space and time is a 3x3 tensor which is a function of the
   strain at that location.  The function that relates the stress tensor to the strain tensor is the
   constitutive model. Depending on the constitutive model, the material can behave elastically or
   plastically with an increment in strain.

Details about stress calculation for two different constitutive models are presented below.

### Linear elastic constitutive model

In scenarios where the material exhibits a linear relation between stress and strain, and does not
retain any residual strain after unloading, is called a linear elastic material. In linear
elasticity, the stress tensor ($\sigma$) is calculated as $\sigma = \mathcal{C}\epsilon$, where
$\mathcal{C}$ is the elasticity tensor, and $\epsilon$ is the strain tensor. This material model is
currently used for numerically modeling the behavior of concrete and other materials used for
designing a structure in MASTODON.

### Nonlinear hysteretic constitutive model for soils (I-soil)

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
\tau = \sum_{k=1}^{i} G_{k}*\gamma +  \sum_{k=i+1}^{n} {\tau_{y}}^k
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

## Foundation-soil interface models

The foundation-soil interface is an important aspect of NLSSI modeling. The foundation-soil interface
simulates geometric nonlinearities in the soil-structure system: gapping (opening and closing of gaps
between the soil and the foundation), sliding, and uplift.

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

## Special boundary conditions

### Non-reflecting boundary

This boundary condition applies a Lsymer damper [citep:lysmer1969finite] on a given boundary to
absorb the waves hitting the boundary. To understand Lsymer dampers, let us consider an uniform
linear elastic soil column and say a 1D vertically propagating P wave is traveling through this soil
column. Then the normal stress at any location in the soil column is given by:

\begin{equation}
\label{eqn:normal_stress}
\sigma = E \epsilon = E \frac{du}{dx} = \frac{E}{V_p} \frac{du}{dt}= \rho V_p \frac{du}{dt}
\end{equation}

where, $E$ is the Young's modulus, $\sigma$ is the normal stress, $\epsilon$ is the normal strain,
$\rho$ is the density, $V_p = \sqrt{\frac{E}{\rho}}$ is the P-wave speed and $\frac{du}{dt}$ is the
particle velocity. Note that for a 3D problem, the P-wave speed is $V_p = \sqrt{\frac{E(1-\
nu)}{(1+\nu)(1-2\nu)}}$.

The stress in the above equation is directly proportional to the particle velocity which makes this
boundary condition analogous to a viscous damper with damping coefficient of $\rho V_p$. So
truncating the soil domain and placing this damper at the end of the domain is equivalent to
simulating wave propagation in an infinite soil column provided the soil is made of linear elastic
material and the wave is vertically incident on the boundary.

If the soil is not linear elastic or if the wave is incident at an angle on the boundary, the waves
are not completely absorbed by the Lsymer damper. However, if the non-reflecting boundary is placed
sufficiently far from the region of interest, any reflected waves will get damped out by rayliegh
damping or hysteretic material behavior before it reaches the region of interest.

### Seismic force

In some cases, the ground excitation is measured at a rock outcrop (where rock is found at surface
level and there is no soil above it). To apply this to a location where rock is say $10$m deep and
there is soil above it, a sideset is created at $10$m depth and the ground excitation (converted into
a stress) is applied at this depth. To apply ground excitation as a stress, the input function should
be given as ground velocity.

To convert a velocity applied normal to the boundary into a normal stress, Equation
[eqn:normal_stress] can be used. Using a similar argument as discussed in the section above, to
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

### Preset acceleration

If the ground excitation was measured at a depth within the soil by placing an accelerometer at that
location, then it is termed as a within-soil input. This time history contains the wave that was
generated by the earthquake (incoming wave) and the wave that is reflected off the free surface. This
ground excitation time history is usually available in the form of a acceleration time history. Since
MASTODON is a displacement controlled algorithm, i.e., displacements are the primary unknown
variables, the acceleration time history is first converted to the corresponding displacement time
history using Newmark time integration equation (Equation [eqn:Newmark]). This displacement time
history is then prescribed to the boundary.

### Domain reduction method (DRM)

Earthquake 'source-to-site' simulations require simulating a huge soil domain (order of many
kilometers) with a earthquake fault. The nuclear power plant structure, which is usually less than
100 m wide, is located very far from the earthquake fault, and the presence of the structure only
affects the response of the soil in the vicinity of the structure. In most of these situations, where
a localized feature such as a structure is present in a huge soil domain, the problem can be divided
into two parts: (i) a free-field 'source-to-site' simulation is run on the huge soil domain (
[fig:DRM](a)) that does not contain the localized feature, and (ii) the forces from the free-field
simulation at one element layer, which is the element layer separating the bigger and smaller soil
domain, can be transferred to a much smaller domain containing the localized feature (
[fig:DRM](b)). This method of reducing the domain is called the domain reduction method (DRM)
[citep:bielak2003domain]. [fig:DRM] is reproduced from [citet:bielak2003domain].

!media media/theory/DRM.png
       style=width:100%;float:center;
       id=fig:DRM
       caption=Domain reduction method summary: (a) Big soil domain containing the earthquake fault
               but not the localized feature. The displacements are obtained at the boundaries
               $\Gamma$ and $\Gamma_e$ and are converted to equivalent forces. (b) Smaller soil
               domain containing the localized feature but not the earthquake fault. The equivalent
               forced calculated in (a) are applied at the boundaries $\Gamma$ and $\Gamma_e$.

To convert the displacements at $\Gamma$ and $\Gamma_e$ from part (i) to equivalent forces, a finite
element model of the one element layer between $\Gamma$ and $\Gamma_e$ is simulated in two
steps. First, the boundary $\Gamma_e$ is fixed and the boundary $\Gamma$ is moved with the
displacements recorded at $\Gamma$. This step gives the equivalent forces at $\Gamma_e$. Second, the
boundary $\Gamma$ is fixed and the boundary $\Gamma_e$ is moved with the displacements recorded at
$\Gamma_e$. This steps gives the equivalent forces at $\Gamma$.

Note: The meshes for the bigger soil domain and smaller soil domain need not align between $\Gamma$
and $\Gamma_e$. The equivalent forces can be applied as point forces at the same coordinate location
at which nodes are present in the bigger model, irrespective of whether these locations correspond to
nodal locations in the smaller model.

## Earthquake fault rupture

The orientation of an earthquake fault is described using three directions - strike ($\phi_s$), dip
($\delta$) and slip direction ($\lambda$) as shown in [fig:fault_orientation], which is courtesy of
[citet:aki2012quantitative].

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
[fig:source_direction]; [citet:aki2012quantitative]).

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

## Post-processing

This section presents some of the common post-processing tools available in MASTODON that help in
understanding the wave propagation, and response of structures and soils to earthquake excitation.

### Time histories

In MASTODON, the time history of any nodal variable (displacement, velocity, acceleration) or
elemental variable (stress, strain) can be requested as output. The nodal variable time histories can
be requested at a set of nodes and this can help in visualizing the wave propagation.

### Response spectra

An important quantity that is used in understanding the response of a structure is the
velocity/acceleration response spectra. This contains information about the frequency content of the
velocity/acceleration at a particular location. The velocity/acceleration response spectra at a
frequency $\omega$ is obtained by exciting a single degree of freedom (SDOF) system with natural
frequency of $\omega$ with the velocity/acceleration time history recorded at that location, and
obtaining the peak velocity/acceleration experienced by the SDOF. This exercise is repeated for
multiple frequencies to obtain the full response spectra.

### Housner spectrum intensity

The response spectra is very useful for understanding the response of the system at one
location. However, if the response at multiple locations have to be compared, a single value that can
summarize the response at a location is much more useful. Housner spectrum intensity is the integral
of the velocity response spectra between 0.25-2.5 s (or 0.4-4 Hz). This packs the information from
the velocity response spectra at multiple frequencies into a single value and reasonably represents
the response at a location.

!bibtex bibliography
