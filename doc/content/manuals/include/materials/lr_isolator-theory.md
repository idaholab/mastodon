### Lead-Rubber Isolator id=lrisolator

Lead-rubber (LR) seismic isolators are composed of alternating layers of natural rubber and steel shims with
steel flange plates at top and bottom, and a central cylindrical lead core. Continuum modeling of LR
bearings is computationally expensive and impractical for applications involving large structures
with tens to hundreds of isolators. Hence, the two-node model developed by [!citet](manishkumarmceer2015a) is adopted here.
The isolator model has six degrees of freedom (3 translations and 3 rotations) at each node and is capable of
simulating nonlinear behavior in both axial and shear directions. The physical model of the
two-node isolator element is shown in [fig:physicalmodelofbearing] below. For a detailed description on the implementation of numerical model, refer to [!citep](manishkumarmceer2015a).

!media media/materials/lr_isolator/physicalmodel.png
       style=width:70%;margin-left:150px;float:center;
       id=fig:physicalmodelofbearing
       caption=Discrete, two-noded model of an LR bearing: (a) degrees of freedom, and (b) discrete
       spring representation [!citep](manishkumarEESD2014).

This model captures the following physical behaviors:

- Axial

  - Buckling in compression
  - Buckling load variation with shear deformation
  - Cavitation and post-cavitation behavior in tension

- Shear

  - Viscoelastic behavior of rubber
  - Hysteretic behavior of the lead core
  - Strength degradation due to the heating of the lead core


- Coupling in axial and shear

  - Shear stiffness variation with axial load
  - Axial stiffness variation with shear deformation


#### Axial (local X direction)

##### Compression

The composite action of the rubber and the steel shims result in high axial stiffness of a bearing. The expression for the critical buckling load in compression is derived from the two-spring model proposed by [!citet](kohandkelly1987) as shown in [lr_eq_1].

\begin{equation}
\label{lr_eq_1}
\begin{aligned}
P_{cr}=\sqrt{P_SP_E} \; \; \; \; \text{where,} \\
P_E=\frac{{\pi}^2EI_s}{h}^2 \; \;\;\; \text{and}\;\;\;\; P_S=GA_S
\end{aligned}
\end{equation}

where, \\
$A_s=A\frac{h}{T_r}$ \\
$I_s=I\frac{h}{T_r}$

[lr_eq_1] provides the critical buckling load at zero shear displacement. Experimental investigations have
shown that the critical buckling load is a function of the shear deformation. [!citet](warnandwhittaker2006) proposed a simplified expression for the critical buckling load as a function of the overlap bonded rubber area. The critical buckling load is updated at every analysis step based on the current shear deformation in the bearing.

\begin{equation}
\label{lr_eq_2}
P_{cr} = \begin{cases}
           P_{cro}\frac{A_r}{A}, & \text{if}\ \; \frac{A_r}{A} \ge 0.2 \\
           0.2P_{cro} & \text{if}\ \; \frac{A_r}{A} \le 0.2
           \end{cases}
\end{equation}

where, \\
$E_r$ is the rotational modulus  \\
$h$ is the height of isolator excluding the end plates  \\
$T_r$ is the total rubber thickness \\
$G$ is the shear modulus of the rubber  \\
$A$ is the initial bonded rubber area \\
$P_{cr}$ is the current value of the critical buckling load  \\
$P_{cro}$ is the critical buckling load at zero shear displacement \\
$A_r$ is the current overlap area of bonded rubber

Once the bearing buckles, the axial stiffness is reduced to zero. However, to avoid numerical problems,
a very small value of axial stiffness is assigned to the post-buckling region.

##### Tension

Beyond a certain tensile stress, rubber undergoes cavitation, which is an expansion of existing voids or defects in the material. Cavitation results in an inelastic deformation due to the onset of permanent damage. The LR isolator exhibits linear elastic behavior, till the point of cavitation ($u_{cn}$,$F_{cn}$), where, $u_{cn}$ and $F_{cn}$ are the  deformation and force, respectively, at the onset of cavitation. Due to the breakage of rubber-filler bonds, the post-cavitation stiffness is reduced. [!citet](constantinouwhittaker2007) suggested an expression for post-cavitation stiffness as:

\begin{equation}
\label{lr_eq_3}
K_{postcavitation}=\frac{EA_o}{T_r}e^{-k(u-u_c)} = \frac{dF}{du}
\end{equation}

Integrating the [lr_eq_3], the expression for the post-cavitation tensile force is obtained as:

\begin{equation}
\label{lr_eq_4}
F=F_c\left[1+\frac{1}{kT_r}\left(1-e^{-k(u-u_c)}\right)\right]
\end{equation}

where,  \\
$F_c$ is the initial cavitation force \\
$k$ is a cavitation parameter \\
$u_c$ is the initial cavitation deformation \\
$u$ is the tensile deformation \\
$E$ is Young's modulus of the rubber

When the bearing is loaded beyond the point of cavitation and unloaded, it does not unload with the initial elastic
stiffness. The unloading stiffness is smaller than the elastic stiffness because of formation of cavities in the rubber.
The cavitation strength is a function of a damage parameter $\phi$ which varies from zero to one. At maximum damage, the tensile strength of rubber layers is not completely zero. Therefore an upper bound of 0.75 value is assumed for the damage parameter.

\begin{equation}
\label{lr_eq_5}
\phi=\phi_{max}\left[1-e^{-a\left(\frac{u-u_c}{u_c}\right)}\right]
\end{equation}

where,  \\
$a$ is the damage constant  \\
$\phi$ is the current value of the damage parameter  \\
$\phi_{max}$ is the upper bound on the damage parameter

For $u=u_c$ (namely, at the cavitation point), $\phi$ is equal to 0 implying no damage. As the tensile deformation increases, the damage parameter converges to $\phi_{max}$. The reduced cavitation strength as a function of damage parameter $\phi$ is calculated as

\begin{equation}
\label{lr_eq_6}
F_{cn}=F_c(1-\phi)
\end{equation}

[fig:axialformulation] shows the assumed response of a LR isolator to cyclic axial loading (both compression and tension).

!media media/materials/lr_isolator/axialformulation.png
       style=width:70%;margin-left:150px;float:center;
       id=fig:axialformulation
       caption=Axial response of an elastomeric isolator [!citep](manishkumarEESD2014).

where,  \\
$K_v$ is the axial stiffness of the isolator \\
$u_h$ is the current shear deformation \\
$r$ is the radius of gyration of the isolator

#### Shear behavior (local Y and Z directions)

The smoothed hysteric model proposed by [!citet](park1986) and extended by [!citet](nagarajaiah1989) is used to model the
behavior of a lead-rubber bearing in shear. The shear resistance of the LR bearing is a combination of the
viscoelastic behavior of the rubber layers and the hysteretic behavior of the lead core, as shown in
[fig:shearformulation]. The behavior of rubber is characterized by elastic stiffness, viscous damping terms and the nonlinear hysteretic contribution of the lead core is modeled using a Bouc-Wen formulation.

!media media/materials/lr_isolator/shearformulation.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:shearformulation
       caption=Shear behavior of the LR isolator [!citep](manishkumarmceer2015a).

\begin{equation}
\label{lr_eq_7}
  \begin{Bmatrix}
           F_y \\
           F_z
  \end{Bmatrix}
= C_d
  \begin{Bmatrix}
           \dot{U_y} \\
           \dot{U_z}
  \end{Bmatrix}
+ k_d
  \begin{Bmatrix}
          U_y \\
          U_z
  \end{Bmatrix}
+ (\sigma_{yl}A_l)
  \begin{Bmatrix}
          Z_y \\
          Z_z
  \end{Bmatrix}
\end{equation}

where, \\
subscripts $y$ and $z$ correspond to shear 1 (local Y) and shear 2 (local Z) directions respectively, as shown in [fig:physicalmodelofbearing] \\
$C_d$ is the viscous damping co-efficient of the rubber</br>
$k_d$ is the shear stiffness of the rubber material </br>
$\sigma_{yl}$ is the dynamic yield strength of the lead core </br>
$A_l$ is the cross sectional area of the lead core

The Bouc-Wen formulation introduces two hysteresis evolution parameters, $Z_y$ and $Z_z$, which are functions of the shear displacements $U_y$ and $U_z$, in the local Y and Z directions respectively.

\begin{equation}
\label{lr_eq_8}
  u_y
  \begin{Bmatrix}
           \dot{Z_y} \\
           \dot{Z_z}
  \end{Bmatrix}
  =([I]-[\Omega])
  \begin{Bmatrix}
           \dot{U_y} \\
           \dot{U_z}
  \end{Bmatrix}
\end{equation}

where,

\begin{equation}
\label{lr_eq_9}
  \Omega =
  \begin{Bmatrix}
  (Z_y)^2((\gamma Sign(\dot{U_y}Z_y))+\beta) & Z_yZ_z((\gamma Sign(\dot{U_z}Z_z))+\beta) \\
  Z_yZ_z((\gamma Sign(\dot{U_y}Z_y))+\beta) & (Z_z)^2((\gamma Sign(\dot{U_z}Z_z))+\beta)
  \end{Bmatrix}
\end{equation}

[lr_eq_8] is iteratively solved using the Newton-Raphson method to calculate the values of $Z_y$ and $Z_z$ at every analysis step in MASTODON.

#### Lead core heating

When the LR isolator is subjected to cyclic loads in shear, the temperature of the lead core increases.
The effective yield stress of the lead core used in [lr_eq_7] varies with the temperature of the lead core, which is a function of time. Therefore, at every time step, the temperature of lead core is calculated and the
dynamic yield strength of the lead is adjusted. The relationship between the dynamic yield
strength of the lead core and the instantaneous temperature was proposed by [!citet](kalpakidiandconstantinou2009a) as

\begin{equation}
\label{lr_eq_10}
\sigma_{yl}(T_L)=\sigma_{yl0}e^{-0.0069T_L}
\end{equation}

where,  \\
$\sigma_{yl}(T_L)$ is the dynamic strength of lead core at the current temperature \\
$\sigma_{yl0}$ is the dynamic yield strength of lead core at a reference temperature  \\
$T_L$ is the instantaneous temperature of the lead core

#### Coupled shear and axial response

When the axial load on the bearing is close to the critical buckling load, the shear stiffness of the rubber is reduced
substantially. This reduction in shear stiffness as a function of axial load is approximated using the equation from
[!citet](kelly1993).

\begin{equation}
\label{lr_eq_11}
K_H = \frac{GA}{T_r}\left[1-\left(\frac{P}{P_{cr}}\right)^{2}\right]
\end{equation}

where,  \\
P is the current axial load on the isolator

The axial stiffness of the LR isolator is also a function of the overlap bonded rubber area, which is
a function of the shear deformation. A simplified expression formulated by [!citet](kohandkelly1987) and
[!citet](warnwhittaker2007) is adopted here
\begin{equation}
\label{lr_eq_12}
K_V = \frac{AE_c}{T_r}\left[1+\frac{3}{\pi^{2}}\left(\frac{u_h}{r}\right)^{2}\right]
\end{equation}

where,  \\
$u_h$ is the current shear deformation

#### Limitations

- Currently, this formulation is limited to simulations that result in small rigid-body rotations in the isolator because the isolator deformations are transformed from the global to the local coordinate system using a transformation matrix that is calculated from the initial position of the isolator. This transformation matrix is not updated during the analysis because, seismic isolators typically undergo very small rigid-body rotations.
- The post-buckling behavior of the LR isolator is modeled using a very small axial stiffness ($1/1000^{th}$) of the initial stiffness to avoid numerical convergence problems.
- The mass of the LR bearing is not modeled. The user can model the mass of an isolator using inertial NodalKernels as shown in a seismic simulation example [here](manuals/include/materials/lr_isolator-user.md#seismic_example).
