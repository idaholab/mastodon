## GQ/H Material Model id=gqh

A procedure to analytically verify the automatically generated backbone curve using the General Quadratic/Hyperbolic (GQ/H) type material model is presented in this section. The element was assigned the properties listed in [gqh-mat-props].

!table id=gqh-mat-props caption=Soil Properties.
| Property | Symbol | Value |
| - | - | - |
| Density | $\rho$ | 2,000 kg/m^3^ |
| Maximum Shear Modulus | $G_{max}$ | 2.0e+07 Pa |
| Poisson's Ratio | $\nu$ | 0.3 |
| Maximum Shear Stress | $\tau_{max}$ | 7,500 Pa |
| GQ/H Curve Fitting Parameters | $\theta_{1}$ | -2.28 |
|                               | $\theta_{2}$ | -5.54 |
|                               | $\theta_{3}$ | 1.0 |
|                               | $\theta_{4}$ | 1.0 |
|                               | $\theta_{5}$ | 0.99 |

To investigate the behavior of the GQ/H backbone curve, it was assumed that the element had zero initial stresses, no self weight (i.e., zero gravity), and undamped vibration.

[!citet](groholski2016simplified) proposed a curve fitting scheme to describe the following hyperbolic relationship between the normalized shear stress and shear strain:

\begin{equation}
\label{normalized-shear}
\theta_{\tau}\Big(\frac{\tau}{\tau_{max}}\Big)^{2}-\Big(1+\frac{\gamma}{\gamma_{r}}\Big)\Big(\frac{\tau}{\tau_{max}}\Big)+\frac{\gamma}{\gamma_{r}}=0
\end{equation}

The coefficient $\theta_{\tau}$ is a model-fitting function described by the following equation:

\begin{equation}
\label{curve}
\theta_{\tau}=\theta_{1}+\theta_{2}\frac{\theta_{4}\Big(\dfrac{\gamma}{\gamma_{r}}\Big)^{\theta_{5}}}{\theta_{3}^{\theta_{5}}+\theta_{4}\Big(\dfrac{\gamma}{\gamma_{r}}\Big)^{\theta_{5}}}\leq1
\end{equation}

The general solution of [normalized-shear] yields the backbone curve. For the special case where $\theta_{\tau}\ne0$, the solution is given by

\begin{equation}
\label{backbone}
\frac{\tau}{\tau_{max}}=\frac{1+\Big(\dfrac{\gamma}{\gamma_{r}}\Big)-\sqrt{\Big[1+\Big(\dfrac{\gamma}{\gamma_{r}}\Big)\Big]^{2}-4\theta_{\tau}\Big(\dfrac{\gamma}{\gamma_{r}}\Big)}}{2\theta_{\tau}}
\end{equation}

The GQ/H model also encompasses the special case where $\theta_{\tau}=0$. For this case, MASTODON employs the KZ hyperbolic model described in [!citet](groholski2016simplified), i.e,

\begin{equation}
\label{backbone-zero}
\tau=\frac{G_{max}\gamma}{1+\Big(\dfrac{\gamma}{\gamma_{r}}\Big)}
\end{equation}

The reference strain, $\gamma_{r}$, used in both [backbone] and [backbone-zero] is given by

\begin{equation}
\gamma_{r}=\frac{\tau_{max}}{G_{max}}
\end{equation}

[curve] and [backbone] were sampled at $n=100$ shear strain points, $\gamma_{j}$ ([backbone-zero] did not apply for this problem since $\theta_{\tau}$, computed by [curve], was never zero). In MASTODON, the $n$ sample strains are equally spaced between 1.0e-06 and 0.1 within the log-space to better capture backbone curvature at lower strains, i.e., the sample points are given by

\begin{equation}
\gamma_{j}=10^{[-6+5j/(n-1)]}\,\,\,,\,\,\,j=0,1,2,...,n-1
\end{equation}

### Results

The hysteresis stress-strain curve in the ZX plane is shown in [gqh-hysteresis-loop].

!plot scatter filename=test/tests/materials/isoil/gold/GQH_hysteresis.csv
              data=[{'x':'strain_zx_el', 'y':'stress_zx_el', 'name':'Hysteresis Loop in the zx-plane'}]
              layout={'xaxis':{'title':'strain_zx'},
                      'yaxis':{'title':'stress_zx (Pa)'},
                      'title':'Hysteresis Loop in the ZX Plane'}
              id=gqh-hysteresis-loop
              caption=Plot of the element hysteresis stress-strain curve in the ZX plane over the duration of the simulation.

Since the engineering strain in the ZX plane, $\gamma_{zx}$, is equal to two times the tensor shear strain, $\epsilon_{zx}$, it was necessary to multiply the strains output by MOOSE TensorMechanics by two to get the shear stresses, $\tau_{zx}$, which correspond to those determined by [backbone]. Next, the engineering shear stress-strain curve was superimposed over the 100 points, calculated by hand with [curve] and [backbone], as shown in [gqh-backbone-curve]. The backbone curve was obtained in MOOSE by sampling the data during the initial load cycle, i.e., from 3 seconds to 4 seconds (see [isoil/problem_statement.md]). It is clear from [gqh-backbone-curve] that the analytical solution of the GQ/H model was consistent with the MASTODON one.

!media media/materials/isoil/GQH_backbone.png
       id=gqh-backbone-curve
       caption=The analytical GQH backbone curve sampled at 100 points and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds).
       style=width:100%

### Complete Input File

!listing test/tests/materials/isoil/HYS_GQH_verification.i
