#Theory manual

## Introduction
Multi-hazard Analysis for STOchastic time-DOmaiN phenomena (MASTODON) is a finite element application that aims at analyzing the response of 3-D soil-structure systems to natural and man-made hazards such as earthquakes, floods and fire. MASTODON currently focuses on the simulation of seismic events and has the capability to perform extensive 'source-to-site' simulations including earthquake fault rupture, nonlinear wave propagation and nonlinear soil-structure interaction (NLSSI) analysis. MASTODON is being developed to be a dynamic probabilistic risk assessment framework that enables analysts to not only perform deterministic analyses, but also easily perform probabilistic or stochastic simulations for the purpose of risk assessment.

MASTODON is a MOOSE-based application and performs finite-element analysis of the dynamics of solids, mechanics of interfaces and porous media flow. It is equipped with numerical material models of dry and saturated soils including a nonlinear hysteretic soil model, and a u-P-U model for saturated soil, as well as structural materials such as reinforced concrete. It is also equipped with interface models that simulate gapping, sliding and uplift at the interfaces of solid media such as the foundation-soil interface of structures. MASTODON also includes absorbing boundary models for the simulation of infinite or semi-infinite domains, fault rupture model for seismic source simulation, and the domain reduction method for the input of complex, three-dimensional wave fields. Since MASTODON is a MOOSE-based application, it can efficiently solve problems using standard workstations or very large high-performance computers.

This document describes the theoretical and numerical foundations of MASTODON.

## Governing equations
The basic equation that MASTODON solves is the nonlinear wave equation:

\begin{eqnarray}
\label{eqn:governing_equation}
 \rho \mathbf{\ddot{u}} + \nabla \cdot \boldsymbol{\sigma} = \mathbf{F_{ext}}
\end{eqnarray}

where, $\rho$ is the density of the soil or structure that can vary with space, $\boldsymbol{\sigma}$ is the stress at any point in space and time, $\mathbf{F_{ext}}$ is the external force acting on the system that can be in the form of localized seismic sources or global body forces such as gravity, and $\mathbf{\ddot{u}}$ is the acceleration at any point within the soil-structure domain. The left side of the equation contains the internal forces acting on the system with first term being the contribution from the inertia, and the second term being the contribution from the stiffness of the system. Additional terms would be added to this equation when damping is present in the system. The material stress response ($\boldsymbol{\sigma}$) is described by the constitutive model, where the stress is determined as a function of the strain ($\boldsymbol{\epsilon}$), i.e. $\boldsymbol{\sigma}(\boldsymbol{\epsilon})$. Details about the material constitutive models available in MASTODON are presented in the section about [material models](#Material models).

The above equation is incomplete and ill-conditioned without the corresponding boundary conditions. There are two main types of boundary conditions - (i) Dirichlet boundary condition which is a kinematic boundary condition where the displacement, velocity, or acceleration at that boundary is specified; (ii) Neumann boundary condition where a force or traction is applied at the boundary. All the special boundary conditions such as absorbing boundary condition are specialized versions of these broad boundary condition types.

## Time integration
To solve Equation \eqref{eqn:governing_equation} for $\mathbf{u}$, an appropriate time integration scheme needs to be chosen. Newmark and Hilber-Hughes-Taylor (HHT) time integration schemes are two of the commonly used methods in solving wave propagation problems.

### Newmark time integration
In Newmark time integration \citep{newmark1959amethod}, the acceleration and velocity at $t+\Delta t$ are written in terms of the displacement ($\mathbf{u}$), velocity ($\mathbf{\dot{u}}$) and acceleration ($\mathbf{\ddot{u}}$) at time $t$ and the displacement at $t+\Delta t$.

\begin{eqnarray} \label{eqn:Newmark}
\mathbf{\ddot{u}}(t+\Delta t) &=& \frac{\mathbf{u}(t+\Delta t)-\mathbf{u}(t)}{\beta \Delta t^2}- \frac{\mathbf{\dot{u}}(t)}{\beta \Delta t}+\frac{\beta -0.5}{\beta}\mathbf{\ddot{u}}(t) \\
\mathbf{\dot{u}}(t+ \Delta t) &=& \mathbf{\dot{u}}(t)+ (1-\gamma)\Delta t \mathbf{\ddot{u}}(t) + \gamma \Delta t \mathbf{\ddot{u}}(t+\Delta t)
\end{eqnarray}

In the above equations, $\beta$ and $\gamma$ are Newmark time integration parameters. Substituting the above two equations into the equation of motion will result in a linear system of equations ($\mathbf{Au}(t+\Delta t) = \mathbf{b}$) from which $\mathbf{u}(t+\Delta t)$ can be estimated.

For $\beta = 0.25$ and $\gamma = 0.5$, the Newmark time integration scheme is the same as the trapezoidal rule. The trapezoidal rule is an unconditionally stable integration scheme, i.e., the solution does not diverge for any choice of $\Delta t$, and the solution obtained from this scheme is second order accurate. One disadvantage with using trapezoidal rule is the absence of numerical damping to damp out any high frequency numerical noise that is generated due to the discretization of the equation of motion in time.

The Newmark time integration scheme is unconditionally stable for $\gamma \ge \frac{1}{2}$ and $\beta \ge \frac{1}{4}\gamma$. For $\gamma > 0.5$, high frequency oscillations are damped out, but the solution accuracy decreases to first order.

### Hilber-Hughes-Taylor (HHT) time integration
The HHT time integration scheme \citep{hughes2000thefinite} is built upon Newmark time integration method to provide an unconditionally stable and second order accurate numerical scheme with the ability to damp out high frequency numerical noise. Here, in addition to the Newmark equations, the equation of motion is also altered resulting in:

\begin{eqnarray}
\rho\mathbf{\ddot{u}}(t+\Delta t) + \nabla \cdot [(1+\alpha) \boldsymbol{\sigma}(t+\Delta t) - \alpha \boldsymbol{\sigma}(t)] = \mathbf{F_{ext}}(t+\alpha\Delta t)
\end{eqnarray}

Here, $\alpha$ is the HHT parameter. The optimum parameter combination to use for this time integration scheme is $\beta = \frac{1}{4}(1-\alpha)^2$, $\gamma = \frac{1}{2} - \alpha$, and $-0.3 \le \alpha \le 0$.

## Small strain damping  
When the soil-structure system (including both soil and concrete) responds to earthquake excitation energy is dissipated in two primary ways: (1) elastic and/or in-elastic material response, and (2) gapping, sliding and uplift at the soil-foundation interface. Dissipation of energy due to item (1) is modeled (approximately) using two different methods: (i) small strain damping experienced at very small strain levels where the material behavior is largely linear elastic; (ii) hysteretic damping due to nonlinear behavior of the material. Dissipation of energy due to (2) is discussed in the section about [foundation-soil interface models](#Foundation-soil interface models). This section discusses the damping that is present at small strain levels.

### Rayleigh damping
Rayleigh damping is the most common form of classical damping used in modeling structural dynamic problems. The more generalized form of classical damping, Caughey Damping \citep{caughey1960classical}, is currently not in MASTODON. Rayleigh damping is a specific form of Caughey damping that uses only the first two terms of the series. In this method, the viscous damping is proportional to the inertial contribution and contribution from the stiffness. This implies that in the matrix form of the governing equation, the damping matrix ($\mathbf{C}$) is assumed to be a linear combination of the mass ($\mathbf{M}$) and stiffness ($\mathbf{K}$) matrices, i.e., $\mathbf{C} = \eta \mathbf{M} +\zeta\mathbf{K}$. Here, $\eta$ and $\zeta$ are the mass and stiffness dependent Rayleigh damping parameters, respectively.

The equation of motion (in the matrix form) in the presence of Rayleigh damping becomes:
\begin{eqnarray}
\mathbf{M}\mathbf{\ddot{u}}+ (\eta \mathbf{M} + \zeta \mathbf{K})\mathbf{\dot{u}} +\mathbf{K}\mathbf{u} = \mathbf{F_{ext}}
\end{eqnarray}

The same equation of motion at any point in space and time (in the non-matrix form) is given by:
\begin{eqnarray}
\rho\mathbf{\ddot{u}} + \eta \rho \mathbf{\dot{u}} + \zeta  \nabla \cdot \frac{d}{dt}\boldsymbol{\sigma} + \nabla \cdot \boldsymbol{\sigma} = \mathbf{F_{ext}}
\end{eqnarray}

The degree of damping in the system depends on the coefficients $\zeta$ and $\eta$ as follows:
\begin{equation}\label{eqn:general_rayleigh}
\xi (f) = \frac{\eta}{2} \frac{1}{f} + \frac{\zeta}{2} f
\end{equation}

where, $\xi(f)$ is the damping ratio of the system as a function of frequency $f$. The damping ratio as a function of frequency for $\zeta = 0.0035$ and $\eta = 0.09$ is presented in \ref{fig:rayleigh}.

!media doc/media/theory/rayleigh.png width=60% margin-left=150px float=center id=fig:rayleigh caption=Damping ratio as a function of frequency.

Because Rayleigh damping only uses two terms, a constant damping ratio can only be approximated between a certain frequency range. The user must consider the frequency range of interest to their application to achieve the desired viscous damping. For building structures it is desirable to have constant damping for the first few modes of response, this is typically between 2 to 10 percent (i.e., between 0.02-0.10). For soils, the small strain (elastic) viscous damping ratio is constant till a cut-off frequency and then it decreases with increase in frequency. MASTODON has the capability to calculate $\zeta$ and $\eta$ for both the above mentioned scenarios.

#### Constant damping ratio
For the constant damping ratio scenario, the aim is to find $\zeta$ and $\eta$ such that the $\xi(f)$ is close to the target damping ratio $\xi_t$, which is a constant value, between the frequency range $[f_1, f_2]$. This can be achieved by minimizing the difference between $\xi_t$ and $\xi(f)$ for all the frequencies between $f_1$ and $f_2$, i.e., if

\begin{eqnarray}
I = \int_{f_1}^{f_2} \xi_t - \left(\frac{\eta}{2}\frac{1}{f} + \frac{\zeta}{2}f\right) df
\end{eqnarray}

Then, $\frac{dI}{d \eta} = 0$ and $\frac{dI}{d \zeta}=0$ results in two equations that are linear in $\eta$ and $\zeta$. Solving these two linear equations simultaneously gives:

\begin{eqnarray}
\zeta = \frac{\xi_t}{2 \pi} \; \frac{3}{(\Delta f)^2} \; \left(f_1 + f_2 - 2 \frac{f_1 f_1}{\Delta f} \; ln \frac{f_2}{f_1}\right) \\
\eta = 2 \pi \xi_t \; \frac{f_1 f_2}{\Delta_f} \; \left[ln \frac{f_2}{f_1}\; \left(2 + 6 \frac{f_1 f_2}{(\Delta_f)^2}\right) - \frac{3(f_1 + f_2)}{\Delta_f}\right]
\end{eqnarray}

#### Damping ratio for soils
The damping seen in soils is usually of the form \citep{withers2015memory}:

\begin{equation}\label{eqn:non_constant_damping}
\xi (f) = \begin{cases}
           \xi_t, & \text{if}\ \; f \le f_T \\
           \xi_t \; \left(\frac{f_T}{f}\right)^\gamma, & \text{if}\ \;f > f_T
           \end{cases}
\end{equation}

where, $f_T$ is the frequency above which the damping ratio starts to deviate from the constant target value of $\xi_t$, and $\gamma$ is the exponent which lies between 0 and 1. Minimizing the difference between Equations \eqref{eqn:non_constant_damping} and \eqref{eqn:general_rayleigh} with respect to $\eta$ and $\zeta$ for all frequencies between $f_1$ and $f_2$ gives:

\begin{eqnarray}
\zeta = \frac{\xi_t}{2 \pi} \; \frac{6}{(\Delta f)^3} \; [b(f_1,f_2) - a(f_1, f_2) \; f_1 f_2] \\
\eta = 2 \pi \xi_t \; \frac{2 f_1 f_2}{(\Delta f)^3} \; [a(f_1, f_2)\; ({f_1}^2 + {f_2}^2 + f_1 f_2) - 3 b(f_1, f_2)]
\end{eqnarray}

where, the functions $a(f_1, f_2)$ and $b(f_1, f_2)$ are given by:

\begin{eqnarray}
a(f_1, f_2) = ln \frac{f_T}{f_1} + \frac{1}{\gamma} \; \left[1- \left(\frac{f_T}{f_2}\right)^\gamma\right] \\
b(f_1, f_2) = \frac{{f_T}^2 - {f_1}^2}{2} + \frac{{f_T}^\gamma}{2-\gamma} \; ({f_2}^{2-\gamma} - {f_T}^{2-\gamma})
\end{eqnarray}

Also, $\xi_t$ for soils is inversely proportional to the shear wave velocity ($V_s$) and a commonly used expression for $\xi_t$ of soil is:

\begin{eqnarray}
\xi_t = \frac{5}{V_s}
\end{eqnarray}

where, $V_s$ is in m/s.

### Frequency independent damping
As seen in the previous sub-section, the damping ratio using Rayleigh damping varies with frequency. Although the parameters $\eta$ and $\zeta$ can be tuned to arrive at a constant damping ratio for a short frequency range, as the frequency range increases, the damping ratio no longer remains constant. For scenarios like these, where a constant damping ratio is required over a large frequency range, frequency independent damping formulations work better. This formulations is under consideration for adding to MASTODON.

## Soil layers and meshing
Soil models in general are made up of different soil layers each with a different material behavior. These soil layers can either be horizontal or non-horizontal. For the horizontal soil layer scenario, the location of the interfaces can be provided as input and MASTODON will use that information to generate a set of soil layers, each with a unique identification number. These layer ids are later used to assign material properties to each soil layer. The same procedure can also be used for non-horizontal but planar soil layers by specifying the normal to the plane and the interface locations measured along the normal direction.

For scenarios where the soil layers are non-horizontal and non-planar, images (.jpg, .png, etc.) of the soil profile can be provided as input. The different soil layers are distinguished from the image by reading either the red, green or blue color value (as per user's directions) at each pixel. Gray scale images in which the red, green and blue values are all the same also work well for this purpose. For creating 3D soil layers, multiple 2D images with soil profiles at different 2-D cross-sections of the soil domain can be provided as input.

Once the soil layers have been distinguished, it is necessary to ensure that the different soil layers are meshed such that they can accurately transmit waves of the required frequency. The optimum element size for each soil layer depends on the type of element used for meshing, cut-off frequency (f) of the wave and the shear wave velocity ($V_s$) of the soil layer. A minimum of 10 points is required per wavelength of the wave to accurately represent the wave in space \citep{coleman2016time}. The minimum wavelength ($\lambda_{min}$) is calculated as:

\begin{equation}
\lambda_{min} = \frac{V_s}{f}
\end{equation}

If linear elements such as QUAD4 or HEX8 are used, then the optimum mesh size is $\lambda_{min}/10$. If quadratic elements such as QUAD9 or HEX27 are used, then the optimum mesh size is $\lambda_{min}/5$. Using the minimum element size information, MASTODON refines the mesh such that the element size criterion is met and at the same time the layers separations are visible. An example of this meshing scheme is presented in \ref{fig:adaptive_meshing} where a 2D soil domain is divided into 3 soil layers and these soil layers are meshed such that the element size criterion is satisfied. A denser mesh is created at the interface between different soil layers.

!media doc/media/theory/adaptive_mesh.png width=60% margin-left=150px float=center id=fig:adaptive_meshing caption=Auto-generated mesh for a soil domain with three non-horizontal non-planar soil layers.

## Material models
To model the mechanical behavior of a material, three components are need to be defined at every point in space and time - strain, elasticity tensor, stress.

1. **Strain**: Strain is a normalized measure of the deformation experienced by a material. In a 1-D scenario, say a truss is stretched along its axis, the axial strain is the elongation of the truss normalized by the length of the truss. In a 3D scenario, the strain is 3x3 tensor and there are three different ways to calculate strains from displacements - small linearized total strain, small linearized incremental strain, and finite incremental strain. Details about these methods can be found in http://mooseframework.org/docs/PRs/9050/site/documentation/modules/tensor_mechanics/index.html.

2. **Elasticity Tensor**: The elasticity tensor is a 4th order tensor with a maximum of 81 independent constants. For MASTODON applications, the soil and structure are usually assumed to behave isotropically, i.e., the material behaves the same in all directions. Under this assumption, the number of independent elastic constants reduces from 81 to 2. The two independent constants that are usually provided for the soil are the shear modulus and Poissons's ratio, and for the structure it is the Young's modulus and Poisson's ratio.

3. **Stress**: The stress at a point in space and time is a 3x3 tensor which is a function of the strain at that location.  The function that relates the stress tensor to the strain tensor is the constitutive model. Depending on the constitutive model, the material can behave elastically or plastically with an increment in strain.

Details about stress calculation for two different constitutive models are presented below.

### Linear elastic constitutive model
In scenarios where the material exhibits a linear relation between stress and strain, and does not retain any residual strain after unloading, is called a linear elastic material. In linear elasticity, the stress tensor ($\boldsymbol{\sigma}$) is calculated as $\boldsymbol{\sigma} = \mathcal{C}\boldsymbol{\epsilon}$, where $\mathcal{C}$ is the elasticity tensor, and $\boldsymbol{\epsilon}$ is the strain tensor. This material model is currently used for numerically modeling the behavior of concrete and other materials used for designing a structure in MASTODON.

### Nonlinear hysteretic constitutive model for soils (I-Soil)
<!-- Add stuff from I - Soil index.md -->
The I-soil material model is a nonlinear hysteretic soil model that is based on the distributed element models developed by \citet{iwan1967on} and \citet{chiang1994anew}. In 1-D, this model takes the backbone shear stress - shear strain curve and divides it into a set of elastic-perfectly plastic curves. The total stress then is the sum of the stresses from the individual elastic-perfectly plastic curves (\ref{fig:1D_representation}).

!media doc/media/theory/1D_isoil_representation.png width=80% margin-left=40px float=center id=fig:1D_representation caption=I-soil model details: (a) 1D representation by springs; (b) example monotonic and cyclic behavior of four nested component model (reprinted from \citet{baltaji2017nonlinear}).

The three-dimensional generalization of this model is achieved using von-Mises failure criteria for each elastic-perfectly plastic curve, resulting in an invariant yield surfaces in three-dimensional stress space like in \ref{fig:yield_surface}.

!media doc/media/yield_surface.png width=40% margin-left=200px float=center id=fig:yield_surface caption=Invariant yield surfaces of the individual elastic-perfectly curves \citep{chiang1994anew}.

The backbone stress-strain curves for each soil layer can be provided in three different ways:

1. User-defined backbone curve (soil_type = 0): The backbone curve can be provided in a .csv file where the first column is shear strain and the second column is shear stress. The number of elastic-perfectly plastic curves that will be generated from this backbone curve depends on the number of entries in the data file. When many soil layers are present, a vector of data files can be provided as input. The size of this vector should equal the number of soil layers. Also the number of entries in each data file should be the same.

2. Darendeli backbone curve (soil_type = 1): The backbone curve can be auto-generated based on empirical data for common soil types. \citet{darendeli2001development} presents a functional form that can be used to create the backbone shear stress - strain curves based on the experimental results obtained from resonant column and torsional shear tests. This functional form requires the initial shear modulus, initial bulk modulus, plasticity index, over consolidation ratio, reference mean confining pressure ($p_{ref}$) and number of points as input. Other than the number of points, all the other parameters can be provided as a vector for each soil layer. The number of points, which determines the number of elastic-perfectly plastic curves to be generated, is constant for all soil layers.

3. General Quadratic/Hyperbolic (GQ/H) backbone curve (soil_type = 2): \citet{darendeli2001development} study constructs the shear stress-strain curves based on experimentally obtained data. At small strains, the data is obtained using resonant column test, and towards the medium shear strain levels, the torsional shear test results are used. These values are extrapolated to the large strain levels. This extrapolation may underestimate or overestimate the shear strength at large strains. Therefore, shear strength correction is necessary to account for the correct shear strength at large strains. GQ/H model proposed by \citet{groholski2016simplified} has a curve fitting scheme that automatically corrects the reference curves provided by \citet{darendeli2001development} based on the specific shear strength at the large strains. This model requires taumax, theta_1 through 5, initial shear modulus, initial bulk modulus and number of points as input. The parameter taumax is the maximum shear stress that can be generated in the soil. The parameters theta_1 through 5 are the curve fitting parameters and can be obtained using DEEPSOIL \citep{hashash2016deepsoil}. Other than the number of points, all the other parameters can be given as a vector for the different soil layers. The number of points, which determines the number of elastic-perfectly plastic curves to be generated, is constant for all soil layers.

Once the backbone curve is generated, the data from this curve is used to calculate the shear modulus and yield stress for each the elastic-plastic curve in \ref{fig:1D_representation}. The stress calculation for each elastic-plastic curve follows two steps:

1. If $\Delta \boldsymbol{\epsilon}$ is the strain increment in the current time step, then it is first assumed that the material is still in the elastic region and the current stress is calculated as $\boldsymbol{\sigma} = \mathcal{C} \boldsymbol{\epsilon}$.

2. Next, the elasticity assumption is tested by checking whether the current stress state is above or below the yield surface. To check this yield criterion, the von-Mises stress ($\sigma_{eq}$) is calculated from the stress tensor $\boldsymbol{\sigma}$. If $\sigma_{eq}$ is less than or equal to the yield stress ($\sigma_y$) for that curve, the stress state calculated in step 1 is correct. If not, the stress tensor calculated in step 1 is brought back to the yield surface by linearly scaling it by $\frac{\sigma_y}{\sigma_{eq}}$.

Once the stress for all the elastic-plastic curves are evaluated using the above 2 step process, the total stress of the material is the summation of the stress tensors from all the elastic-plastic curves.

All the above backbone curves provide the behavior of the soil at a reference confining pressure ($p_{ref}$). When the confining pressure of the soil changes, the soil behavior also changes. The shear modulus ($G(p)$) of each elastic-plastic curve at a pressure $p$ is given by:

\begin{equation}
 G(p) = G_0 \left(\frac{p-p_0}{p_{ref}}\right)^{b_{exp}}
\end{equation}

where, $G_0$ is the initial shear modulus, $p_0$ is the tension pressure cut off and $b_{exp}$ is a parameter obtained from experiments. The shear modulus reduces to zero for any pressure lower than $p_0$ to model the failure of soil in tension. Note that compressive pressure is taken to be positive.  

Similarly, the yield stress ($\sigma_y(p)$) of the elastic-perfectly plastic curve also changes when the confining pressure changes. The yield stress ($\sigma_y(p)$) at a pressure $p$ is given as:
\begin{equation}
\sigma_y(p) = \sqrt{\frac{a_0 + a_1 (p-p_0) + a_2 (p-p_0)^2}{a_0 + a_1 p_{ref} + a_2 {p_{ref}}^2}} \sigma_y(p_{ref})
\end{equation}

where, $a_0$, $a_1$ and $a_2$ are parameters obtained from experiments.

This material model was developed in collaboration with Prof. Youssef Hashash and his students Ozgun Numanoglu and Omar Baltaji from University of Illinois at Urbana-Champaign.

## Foundation-soil interface models
The foundation-soil interface is an important aspect of NLSSI modeling. The foundation-soil interface simulates geometric nonlinearities in the soil-structure system: gapping (opening and closing of gaps between the soil and the foundation), sliding, and uplift.

### Thin-layer method
An efficient approach to modeling the foundation-soil interface is to create a thin layer of the I-Soil material at the interface, as illustrated in \ref{fig:thin_layer} below.

!media doc/media/theory/thin_layer.png width=60% margin-left=100px float=center id=fig:thin_layer caption=Modeling the foundation-soil interface as a thin layer for a sample surface foundation.

The red layer between the foundation (green) and soil (yellow) is the thin layer of I-Soil. The properties of this thin layer are then adjusted to simulate Coulomb friction between the surfaces. The Coulomb-friction-type behavior can be achieved by modeling the material of the thin soil layer as follows:

1. Define an I-Soil material with option 0, which corresponds to a user-defined stress-strain curve.

2. Calculate the shear strength of the thin layer as $\tau_{max}=\mu \sigma_N$ , where $\tau_{max}$ is the shear strength, $\mu$ is the friction coefficient, and $p_{ref}$ is the reference pressure for the material. The reference pressure can be set arbitrarily, or set to the normal stress at the interface from gravity loads.

\begin{equation}
\tau_{max} = \mu p_{ref}
\end{equation}

3. Define the stress-strain curve to be almost elastic-perfectly-plastic, and such that the shear modulus of the thin layer is equal to the shear modulus of the surrounding soil, in case of an embedded foundation. If the foundation is resting on the surface such as in \ref{fig:thin_layer} above, the shear modulus of the thin layer soil should be as high as possible, such that the linear horizontal foundation stiffness is not reduced due to the presence of the thin layer. A sample stress-strain curve is shown in \ref{fig:thin_layer_stress_strain} below. The sample curve in the figure shows an almost bilinear shear behavior with gradual yielding and strain hardening, both of which, are provided to reduce possible high-frequency response. High-frequency response is likely to occur if a pure Coulomb friction model (elastic-perfectly-plastic shear behavior at the interface) is employed, due to the sudden change in the interface shear stiffness to zero.

!media doc/media/theory/thin_layer_stress_strain.png width=60% margin-left=150px float=center id=fig:thin_layer_stress_strain caption=Sample shear-stress shear-strain curve for modeling the thin-layer interface using I-Soil.

4. 	Turn on pressure dependency of the soil stress-strain curve and set $a_0$, $a_1$ and $a_2$ to 0, 0 and 1, respectively. This ensures that the stress-strain curve scales linearly with the normal pressure on the interface, thereby also increasing the shear strength in the above equation linearly with the normal pressure, similar to Coulomb friction.

5. Use a large value for the Poisson’s ratio, in order to avoid sudden changes in the volume of the thin-layer elements after the yield point is reached. A suitable value for the Poisson’s ratio can be calculated by trial and error.

Following the above steps should enable the user to reasonably simulate geometric nonlinearities. These steps will be automated in MASTODON in the near future.

## Special boundary conditions
### Non-reflecting boundary
This boundary condition applies a Lsymer damper \citep{lysmer1969finite} on a given boundary to absorb the waves hitting the boundary. To understand Lsymer dampers, let us consider an uniform linear elastic soil column and say a 1D vertically propagating P wave is traveling through this soil column. Then the normal stress at any location in the soil column is given by:

\begin{equation}
\label{eqn:normal_stress}
\sigma = E \epsilon = E \frac{du}{dx} = \frac{E}{V_p} \frac{du}{dt}= \rho V_p \frac{du}{dt}
\end{equation}

where, $E$ is the Young's modulus, $\sigma$ is the normal stress, $\epsilon$ is the normal strain, $\rho$ is the density, $V_p = \sqrt{\frac{E}{\rho}}$ is the P-wave speed and $\frac{du}{dt}$ is the particle velocity. Note that for a 3D problem, the P-wave speed is $V_p = \sqrt{\frac{E(1-\    nu)}{(1+\nu)(1-2\nu)}}$.

The stress in the above equation is directly proportional to the particle velocity which makes this boundary condition analogous to a viscous damper with damping coefficient of $\rho V_p$. So truncating the soil domain and placing this damper at the end of the domain is equivalent to simulating wave propagation in an infinite soil column provided the soil is made of linear elastic material and the wave is vertically incident on the boundary.

If the soil is not linear elastic or if the wave is incident at an angle on the boundary, the waves are not completely absorbed by the Lsymer damper. However, if the non-reflecting boundary is placed sufficiently far from the region of interest, any reflected waves will get damped out by rayliegh damping or hysteretic material behavior before it reaches the region of interest.

### Seismic force
In some cases, the ground excitation is measured at a rock outcrop (where rock is found at surface level and there is no soil above it). To apply this to a location where rock is say $10$m deep and there is soil above it, a sideset is created at $10$m depth and the ground excitation (converted into a stress) is applied at this depth. To apply ground excitation as a stress, the input function should be given as ground velocity.

To convert a velocity applied normal to the boundary into a normal stress, Equation \eqref{eqn:normal_stress} can be used. Using a similar argument as discussed in the section above, to convert a velocity applied tangential to the boundary into a shear stress, Equation \eqref{eqn:shear_stress} can be used.

\begin{equation}
\label{eqn:shear_stress}
\tau = \rho  V_s  \frac{du}{dt}
\end{equation}

where, $V_s$ is the shear wave speed and $\tau$ is the shear stress.

In some situations, the ground motion measured at a depth within the soil is available. This ground motion is the summation of the wave that enters and exits the soil domain. MASTODON has the capability to extract the incoming wave from the within soil ground motion. To calculate the incoming wave velocity, an iterative procedure is used. The initial guess for the incoming wave velocity ($v_i$) at time t is taken to be the same as the within soil velocity measured at that location. The velocity at this boundary obtained from MASTODON ($v_{mastodon}$) is now going to be different from the measured within soil velocity ($v_{measured}$) at time t. Half the difference between $v_{mastodon}$ and $v_{measured}$ is added to $v_o$ and the iterations are continued until $v_i$ converges (within a numerical tolerance).

### Preset acceleration
If the ground excitation was measured at a depth within the soil by placing an accelerometer at that location, then it is termed as a within-soil input. This time history contains the wave that was generated by the earthquake (incoming wave) and the wave that is reflected off the free surface. This ground excitation time history is usually available in the form of a acceleration time history. Since MASTODON is a displacement controlled algorithm, i.e., displacements are the primary unknown variables, the acceleration time history is first converted to the corresponding displacement time history using Newmark time integration equation (Equation \eqref{eqn:Newmark}). This displacement time history is then prescribed to the boundary.

### Domain reduction method (DRM)
Earthquake 'source-to-site' simulations require simulating a huge soil domain (order of many kilometers) with a earthquake fault. The nuclear power plant structure, which is usually less than 100 m wide, is located very far from the earthquake fault, and the presence of the structure only affects the response of the soil in the vicinity of the structure. In most of these situations, where a localized feature such as  a structure is present in a huge soil domain, the problem can be divided into two parts: (i) a free-field 'source-to-site' simulation is run on the huge soil domain ( \ref{fig:DRM}(a)) that does not contain the localized feature, and (ii) the forces from the free-field simulation at one element layer, which is the element layer separating the bigger and smaller soil domain, can be transferred to a much smaller domain containing the localized feature ( \ref{fig:DRM}(b)). This method of reducing the domain is called the domain reduction method (DRM) \citep{bielak2003domain}.

!media doc/media/theory/DRM.png width=100% float=center id=fig:DRM caption=Domain reduction method summary: (a) Big soil domain containing the earthquake fault but not the localized feature. The displacements are obtained at the boundaries $\Gamma$ and $\Gamma_e$ and are converted to equivalent forces. (b) Smaller soil domain containing the localized feature but not the earthquake fault. The equivalent forced calculated in (a) are applied at the boundaries $\Gamma$ and $\Gamma_e$. This image is reprinted from \citet{bielak2003domain}.

To convert the displacements at $\Gamma$ and $\Gamma_e$ from part (i) to equivalent forces, a finite element model of the one element layer between $\Gamma$ and $\Gamma_e$ is simulated in two steps. First, the bounday $\Gamma_e$ is fixed and the boundary $\Gamma$ is moved with the displacements recorded at $\Gamma$. This step gives the equivalent forces at $\Gamma_e$. Second, the boundary $\Gamma$ is fixed and the boundary $\Gamma_e$ is moved with the displacements recorded at $\Gamma_e$. This steps gives the equivalent forces at $\Gamma$.

Note: The meshes for the bigger soil domain and smaller soil domain need not align between $\Gamma$ and $\Gamma_e$. The equivalent forces can be applied as point forces at the same coordinate location at which nodes are present in the bigger model, irrespective of whether these locations correspond to nodal locations in the smaller model.   

## Earthquake fault rupture
The orientation of an earthquake fault is described using three directions - strike ($\phi_s$), dip ($\delta$) and slip direction ($\lambda$) as shown in \ref{fig:fault_orientation}.

!media doc/media/fault_orientation.png width=80% margin-left=100px id=fig:fault_orientation caption= Definition of the fault-orientation parameters - strike $\phi_s$, dip $\delta$ and slip direction $\lambda$. The slip direction is measured clockwise around from north, with the fault dipping down to the right of the strike direction. Strike direction is also measured from the north. $\delta$ is measured down from the horizontal (image courtesy \citet{aki2012quantitative}).

In MASTODON, earthquake fault is modeled using a set of point sources. The seismic moment ($M_o$) of the earthquake point source in the fault specific coordinate system is:

\begin{equation}
 M_o(t) = \mu A \bar{u}(t)
 \end{equation}

where, $\mu$ is the shear modulus of the soil, $A$ is the area of fault rupture and $\bar{u}(t)$ is the spatially averaged slip rate of the fault.

When this seismic moment is converted into the global coordinate system (x, y and z) with the x direction oriented along the geographic north and z direction along the soil depth, the resulting moment can be written in a symmetric $3 \times 3$ matrix form whose components are as follows:

\begin{eqnarray}
 M_{xx}(t) = -M_o(t)(\sin \delta \cos \lambda \sin2 \phi_s + \sin 2\delta \sin\lambda \sin^2 \phi_s\\
M_{xy}(t) = M_o(t)(\sin\delta \cos \lambda \cos 2 \phi_s + \frac{1}{2} \sin 2\delta \sin \lambda \sin 2 \phi_s) = M_{yx}(t) \\
M_{xz}(t) = -M_o(t)(\cos \delta \cos \lambda \cos \phi_s + \cos 2\delta \sin \lambda \sin 2\phi_s = M_{zx}(t)\\
M_{yy}(t) = M_o(t)(\sin \delta \cos \lambda \sin 2 \phi_s - \sin 2 \delta \sin \lambda \cos^2 \phi_s \\
M_{yz}(t) = -M_o(t)(\cos \delta \cos \lambda \sin \phi_s - \cos 2\delta \sin\lambda \cos\phi_s) = M_{zy}(t) \\
M_{zz}(t) = M_o(t) \sin 2\delta \sin \lambda
\end{eqnarray}

Each component of the above matrix is a force couple with the first index representing the force direction and the second index representing the direction in which the forces are separated (see \ref{fig:source_direction}).

!media doc/media/source_direction.png width=60% margin-left=150px id=fig:source_direction caption=The nine different force couples required to model an earthquake source (image courtesy \citet{aki2012quantitative}).

The total force in global coordinate direction $i$ resulting from an earthquake source applied at point $\vec{\zeta}$ in space is then:

\begin{equation}
f_i(\vec{x}, t) = - \sum_{j=1}^{3} \frac{\partial M_{ij}(\vec{x}, t)}{\partial x_j} = \sum_{j=1}^{3} M_{ij}(t) \frac{\partial \delta (\vec{x} - \vec{\zeta})}{\partial x_j}
\end{equation}

where, $\delta(.)$ is the delta function in space.

When many earthquake sources are placed on the earthquake fault, and they rupture at the same time instant, then an approximation to a plane wave is generated. If one of the point sources is specified as the epicenter and the rupture speed ($V_r$) is provided, then the other point sources start rupturing at $d/V_r$, where $d$ is the distance between the epicenter and the other point source.

## Post-processing
This section presents some of the common post-processing tools available in MASTODON that help in understanding the wave propagation, and response of structures and soils to earthquake excitation.

### Time histories
In MASTODON, the time history of any nodal variable (displacement, velocity, acceleration) or elemental variable (stress, strain) can be requested as output. The nodal variable time histories can be requested at a set of nodes and this can help in visualizing the wave propagation.

### Response spectra
An important quantity that is used in understanding the response of a structure is the velocity/acceleration response spectra. This contains information about the frequency content of the velocity/acceleration at a particular location. The velocity/acceleration response spectra at a frequency $\omega$ is obtained by exciting a single degree of freedom (SDOF) system with natural frequency of $\omega$ with the velocity/acceleration time history recorded at that location, and obtaining the peak velocity/acceleration experienced by the SDOF. This exercise is repeated for multiple frequencies to obtain the full response spectra.

### Housner spectrum intensity
The response spectra is very useful for understanding the response of the system at one location. However, if the response at multiple locations have to be compared, a single value that can summarize the response at a location is much more useful. Housner spectrum intensity is the integral of the velocity response spectra between 0.25-2.5 s (or 0.4-4 Hz). This packs the information from the velocity response spectra at multiple frequencies into a single value and reasonably represents the response at a location.

## References
\bibliographystyle{unsrt}
\bibliography{doc/bib/mastodon.bib}
