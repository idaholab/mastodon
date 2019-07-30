### Nonlinear Fluid Viscous Damper

[`ComputeFVDamperElasticity`](source/materials/ComputeFVDamperElasticity.md) material is used to simulate the hysteretic response of a nonlinear FVD element in MASTODON. Nonlinear fluid viscous dampers (FVDs) are seismic protective devices used to mitigate the effects of intense earthquake shaking on engineered structures such as nuclear power plants. Fluid viscous dampers operate on the principle of fluid flow through the orifice creating a differential pressure across the piston head and develops an internal resisting force in the damper. [!citet](constantinou1995) proposed a simplified expression for the force in the damper as a function of the fractional power of relative velocity as shown in [fv_eqn1]

\begin{equation}
\label{fv_eqn1}
F_D(t)\;=\;C_d|\dot{u}(t)|^{\alpha}sgn(\dot{u}(t))
\end{equation}

where,
$C_d$ is the damping coefficient,
$\alpha$ is the velocity exponent, and
$u\dot(t)$ is the instantaneous relative velocity in the damper.

The response of the FVD is characterized by two parameters, $C_d$, and $\alpha$. For $\alpha$ = 1, [fv_eqn1] models a linear damper and the typical range for $\alpha$ is between 0.3 and 1.0 for seismic applications. Fluid viscous dampers are typically installed in the structure with an in-line brace, which provides stiffness to the damper assembly. [!citet](constantinou1995) observed that such stiffness had to be addressed for response calculations and therefore modeled explicitly. The  brace can be modeled as a spring in series with the nonlinear dashpot, and the combined behavior of spring and dashpot is best represented using the Maxwell formulation, as shown in [fig:fv_maxwellmodel].

!media media/materials/fv_damper/fv_maxwellmodel.png
       style=width:85%;margin-left:7.5%;float:center;
       id=fig:fv_maxwellmodel
       caption=Mathematical model for a nonlinear FVD element [!citep](akcelyan).

The Maxwell model of [fig:fv_maxwellmodel] also includes the flexibility of the gussets, brackets and clevises. The overall stiffness of the damper assembly is calculated as  

\begin{equation}
\label{fv_eqn2}
\frac{1}{K_s} =\frac{1}{K_d}+\frac{1}{K_b}+\frac{2}{K_{cl}}+\frac{2}{K_{gus}}
\end{equation}

In the Maxwell model, the nonlinear dashpot and the spring are in series and therefore the force in the dashpot and spring are equal. This force is given by,

\begin{equation}
\label{fv_eqn3}
F_d(t)=F_S(t)=K_su_s(t)
\end{equation}

and the expressions for the total displacement and the total velocity of the damper are

\begin{equation}
\label{fv_eqn4}
u_m(t) = u_s(t) + u_d(t)
\end{equation}

\begin{equation}
\label{fv_eqn5}
\dot{u}_m(t) = \dot{u}_s(t) + \dot{u}_d(t)
\end{equation}

where,
$u_s(t)$ and $\dot{u}_s(t)$ are the relative displacement and the relative velocity in the spring, and
$u_d(t)$ and $\dot{u}_d(t)$ are the relative displacement and the relative velocity in the dashpot.

From [fv_eqn3], the rate of change of force can be written as

\begin{equation}
\label{fv_eqn6}
\dot{F}_d(t)=K_s\dot{u}_s(t)
\end{equation}

Using [fv_eqn4] & [fv_eqn5], and rewriting the expression for the rate of change of force

\begin{equation}
\label{fv_eqn7}
\dot{F}_d(t)=\left(\dot{u}_m(t)-sgn(F_d(t))\left(\frac{|F_d(t)|}{C_d}\right)\right) \; F_d(t_0)=F_0
\end{equation}

[fv_eqn7] is a first-order differential equation in the form of an initial value problem (IVP)

\begin{equation}
\label{fv_eqn8}
y^{\prime}=f(t_n,y_n) \; and \;  y(t_o)=y_o
\end{equation}

[!citet](dormandprince) proposed an iterative integration scheme known as Dormand Prince (DP54), to solve a generalized initial value problem of the form shown in [fv_eqn8] above. Iterative algorithms such as DP54 are computationally more efficient than traditional integration schemes for numerical solution of initial value problems. [!citet](akcelyan) proposed a framework to obtain the numerical solution of nonlinear FVD based on DP54 algorithm and that has been adopted here. For detailed information on the numerical implementation, specific to the FVD element, refer to [!citet](akcelyan).
