### Friction Pendulum^TM^ Isolator

Friction Pendulum^TM^ (FP) bearings are used to seismically isolate structures, including critical facilities such as nuclear power plants. The single concave FP bearing consists of a spherical sliding surface, a slider coated with PTFE-type composite material, and a housing plate. [fig:fp_components] and [fig:fp_sectionalview] shows components and part sectional view of a single concave Friction Pendulum^TM^ bearing, respectively.

!row!

!media media/materials/fp_isolator/fp_components.png
      style=width:34%;margin-left:4%;margin-right:0%;float:left;
      id=fig:fp_components
      caption=Components of a single Friction Pendulum^TM^ bearing [!citep](EPS)

!media media/materials/fp_isolator/fp_sectionalview.png
      style=width:50.39%;margin-left:1%;margin-right:4%;float:right;
      id=fig:fp_sectionalview
      caption=Sectional view of a single concave Friction Pendulum^TM^ bearing [!citep](manishkumarmceer2015b)

!row-end!

Continuum modeling of FP bearings is computationally expensive and impractical for applications involving structures with tens to hundreds of bearings. Hence a two-node model developed by [!citet](manishkumarmceer2015b) is adopted here. The bottom node represents the concave sliding surface, and the top node represents the articulated slider. The two-node model has six degrees of freedom (3 translations and 3 rotations), at each node and is capable of simulating nonlinear hysteric behavior in the FP bearing. The physical model and spring representation of the two-node FP isolator element is similar to that of the lead-rubber isolator as shown in [fig](manuals/include/materials/lr_isolator-theory.md[fig:physicalmodelofbearing]).

#### Force-displacement relationship

Friction Pendulum^TM^ bearings dissipate the energy induced during an earthquake through nonlinear hysteretic behavior in shear. The lateral force-displacement relationship of a single concave FP bearing is characterized by two parameters: yield strength and post elastic stiffness, and can be represented by the bi-linear curve of [fig:fp_forcedisplacement].

!media media/materials/fp_isolator/fp_forcedisplacement.png
        style=width:40%;margin-left:2%;float:right;
        id=fig:fp_forcedisplacement
        caption=Lateral force-displacement relationship of a single concave FP bearing

The yield strength and the post-elastic stiffness can be calculated as,

\begin{equation}
\label{fp_eqn1}
\begin{aligned}
Q=\mu(t) * N
\end{aligned}
\end{equation}

\begin{equation}
\label{fp_eqn2}
\begin{aligned}
K=\frac{N}{r_{eff}}
\end{aligned}
\end{equation}

where, \\
$Q$ is the yield strength \\
$K$ is the post elastic stiffness \\
$N$ is the instantaneous axial load, and \\
$r_{eff}$ is the effective radius of curvature of the sliding surface.

Parameters $Q$ and $K$ depends on the coefficient of sliding friction and the axial load on the bearing. For detailed information on the numerical implementation of the nonlinear hysteretic behavior of the FP bearing in shear, refer to [!citet](manishkumarmceer2015a) (page:106-107). Additionally, the FP bearing exhibits very large stiffness in axial, torsional and rotational directions and is therefore modeled as a linear elastic material with large stiffness in these degrees-of-freedom.

#### Coefficient of sliding friction

The lateral force-displacement relationship of the FP bearing is governed by the coefficient of sliding friction. At a given instant of time the coefficient of sliding friction depends on the instantaneous sliding velocity, instantaneous axial pressure, and the temperature of the sliding interface.

\begin{equation}
\label{fp_eqn3}
\begin{aligned}
\mu(t)=f(T,v,p)
\end{aligned}
\end{equation}

where, \\
$\mu(t)$ is the coefficient of sliding friction at given time $t$\\
$v$ is the instantaneous sliding velocity\\
$p$ is the instantaneous axial pressure, and \\
$T$ is the instantaneous temperature of the sliding surface.

[!citet](manishkumarmceer2015b) developed a framework to account for the variation in $\mu$ due to the combined effect of velocity, pressure, and temperature and it is adopted here. The sliding velocity and the axial pressure on the bearing are governed by the response of the superstructure. The temperature at the sliding surface is a function of the coefficient of friction, sliding velocity, instantaneous axial pressure and, thermal and heat flux parameters of the material (typically stainless steel):

\begin{equation}
\label{fp_eqn4}
\begin{aligned}
T(t)=f(\mu,v,p,D,k)
\end{aligned}
\end{equation}

where, \\
k is the thermal conductivity of the sliding interface material, and \\
D is the thermal diffusivity of the sliding interface material.

It is evident from [fp_eqn3] and [fp_eqn4], that $T$ and $\mu$ are interdependent. This complex interdependence of the coefficient of sliding friction, sliding velocity, instantaneous axial pressure, and the temperature at the sliding surface is described using the flowchart of [fig:fp_interdependence].

!media media/materials/fp_isolator/fp_interdependence.png
       style=width:75%;margin-left:180px;float:center;
       id=fig:fp_interdependence
       caption=Interdependence of parameters governing the lateral force-displacement relationship of a FP bearing [!citep](manishkumarmceer2015b).

##### Dependence of the coefficient of friction on the sliding velocity

[!citet](mokha1988) proposed an expression to account for the dependence of the coefficient of friction on the instantaneous sliding velocity as

\begin{equation}
\label{fp_eqn5}
\begin{aligned}
\mu(v)=\mu_{max}\left[1-\left(1-\frac{\mu_{min}}{\mu_{max}}\right)e^{-av}\right]
\end{aligned}
\end{equation}

where,\\
$\mu_{min}$ is the friction coefficient at a very small sliding velocity\\
$\mu_{max}$ is the friction coefficient at a very high sliding velocity, and \\
$a$ is a rate parameter.

Based on the experimental data, the ratio $\frac{\mu_{min}}{\mu_{max}}$ is assumed to be a constant value of 0.5 [!citep](manishkumarmceer2015b). The modified expression for coefficient of friction as a function of instantaneous sliding velocity, at constant axial load and temperature, can be written as:

\begin{equation}
\label{fp_eqn6}
\begin{aligned}
\mu(v)=\mu_{max}\left[1-0.5e^{-av}\right]
\end{aligned}
\end{equation}

##### Dependence of the coefficient of friction on the axial pressure

[!citet](manishkumarmceer2015b) proposed an expression for the coefficient of sliding friction as a function of axial pressure at very high sliding velocity, and is given by

\begin{equation}
\label{fp_eqn7}
\begin{aligned}
\mu(p)=\mu_{p=p_o} * \alpha^{\beta(p-p_o)}
\end{aligned}
\end{equation}

where,\\
$p_o$ is the reference axial pressure \\
$\mu_{p=p_o}$ is the co-efficent of sliding friction when the axial pressure on the bearing ($p$) is equal to the reference pressure ($p_o$), and \\
$\alpha$ and $\beta$ are constants.

The assumption of $\mu_{min} =0.5 \mu_{max}$ for all values of axial pressure allows the coefficient of sliding friction to be expressed as the product of the coefficient of friction measured at a reference pressure and a term that depends on the instantaneous axial pressure. Based on the experiments, $\alpha$ and $\beta$ can be set equal to 0.7 and 0.02 respectively [!citep](manishkumarmceer2015b). Rewriting the expression for $\mu$ as a function of $p$:

\begin{equation}
\label{fp_eqn8}
\begin{aligned}
\mu(p)=\mu_{p=p_o} * 0.70^{0.02(p-p_o)}
\end{aligned}
\end{equation}

##### Dependence of the coefficient of friction on the temperature at sliding interface

The coefficient of sliding friction decreases as the number of cycles of loading increase. [!citet](manishkumarmceer2015b) proposed an expression to define the coefficient of friction as a function of the instantaneous temperature, at a reference axial pressure and very high sliding velocity as

\begin{equation}
\label{fp_eqn9}
\begin{aligned}
\mu(T)=\mu_{T=T_o} * 0.79\left[0.7^\frac{T}{50}+0.40\right]
\end{aligned}
\end{equation}

where,\\
$\mu(T)$ is the coefficient of friction at a temperature $T$\\
$\mu_{T=T_o}$ is the coefficient of friction measured at the reference temperature $T_o$, and \\
$T$ is the instantaneous temperature.

During the analysis, it is difficult to monitor the temperature over the entire sliding surface. To address the heating, [!citet](manishkumarmceer2015b) proposed an alternate approach namely, to monitor the temperature at the center of sliding surface only. When the slider is above the center of sliding surface, the temperature increases, and drops as the slider moves away. This increase in temperature at the monitoring location is governed by the heat flux parameters and the path of the slider, and is given by [fp_eqn10]

\begin{equation}
\label{fp_eqn10}
\begin{aligned}
\Delta{T}(x,t)=\frac{\sqrt{D}}{k\sqrt{\pi}}\int_0^\infty \mathrm{q(t-\tau)e^{\frac{-x^2}{4D\tau}}}\,\mathrm{\frac{d\tau}{\sqrt{\tau}}}
\end{aligned}
\end{equation}

where,\\
$\Delta{T}(x,t)$ is the increase in temperature\\
$x$ is the depth measured from the sliding surface,\\
$D$ is the thermal diffusivity of the sliding interface material,\\
$k$ is the thermal conductivity of the sliding interface material, and \\
$q$ is the heat flux.

At a given instant of time, the instantaneous heat flux, at the monitoring location can be expressed as

\begin{equation}
\label{fp_eqn11}
\begin{aligned}
q(t)=\mu(t) * p * v
\end{aligned}
\end{equation}

##### Dependence of the coefficient of friction on the combined effects of temperature, axial pressure and sliding velocity

During an earthquake, the sliding velocity, axial pressure and the temperature at the sliding surface of a FP bearing changes continuously. The combined effect of all the three has to be considered to determine the coefficient of sliding friction. From [fp_eqn6], [fp_eqn8],and [fp_eqn9], the factors accounting for friction dependence on the velocity, pressure, and the temperature ($k_T$) can be written as

\begin{equation}
\label{fp_eqn12}
\begin{aligned}
k_v=\left[1-0.5e^{-av}\right]
\end{aligned}
\end{equation}

\begin{equation}
\label{fp_eqn13}
\begin{aligned}
k_p=0.70^{0.02(p-p_o)}
\end{aligned}
\end{equation}

\begin{equation}
\label{fp_eqn14}
\begin{aligned}
k_T=0.79\left[0.7^\frac{T}{50}+0.40\right]
\end{aligned}
\end{equation}

The coefficient of sliding friction as a function of the instantaneous sliding velocity, axial pressure on the bearing and the temperature at the center of the sliding surface is updated as:

\begin{equation}
\label{fp_eqn15}
\begin{aligned}
\mu(t)=\mu_{ref}k_vk_pk_T
\end{aligned}
\end{equation}

where,\\
$k_v$ is the factor accounting for friction dependence on the sliding velocity\\
$k_p$ is the factor accounting for friction dependence on the axial pressure on the bearing\\
$k_T$ is the factor accounting for friction dependence on the temperature at center of the sliding surface, and \\
$\mu_{ref}$ is the coefficient of sliding friction at refernce pressure $p_o$, measured at high velocity of sliding with the temperature at the center of the sliding surface being $T_o$.
