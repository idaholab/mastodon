## Darendeli Material Model id=darendeli

A procedure to analytically verify the automatically generated backbone curve using the Darendeli type material model is presented in this section. The element was assigned the properties listed in [darendeli-mat-props].

!table id=darendeli-mat-props caption=Soil Properties.
| Property | Symbol | Value |
| - | - | - |
| Density | $\rho$ | 2 Mg/m^3^ |
| Overconsolidation Ratio | $OCR$ | 1 |
| Plasticity Index | $PI$ | 0 |
| Initial Shear Modulus | $G_{max}$ | 20,000 kPa |
| Reference (Confining) Pressure | $\sigma_{o}^{'}$ | 6.07286 kPa |

To investigate the behavior of the Darendeli backbone curve, it was assumed that the element had zero initial stresses, no self weight (i.e., zero gravity), and undamped vibration.

For the [!citet](darendeli2001development) material model, the shear stress-strain backbone curve is described by the following equation:

\begin{equation}
\label{shear}
\tau = \frac{\gamma G_{max}}{1+\Big(\dfrac{100\gamma}{\gamma_{r}}\Big)^{0.919}}
\end{equation}

where the reference shear strain, $\gamma_{r}$, is related to the reference pressure by

\begin{equation}
\label{ref_strain}
\gamma_{r}=(0.0352+0.001(PI)(OCR)^{0.3246})(\sigma_{o}^{'})^{0.3483}
\end{equation}

[ref_strain] requires that $\sigma_{o}^{'}$ be in atmospheric pressure units. It is important to note that, in the current implementation of the MASTODON code, *$\sigma_{o}^{'}$ must be input in units of kPa*, and that the code handles the conversion to atmospheric units. Also, note that [ref_strain] results in percent strain.

[shear] was sampled at $n=100$ shear strain points, $\gamma_{j}$. In MASTODON, the $n$ sample strains are equally spaced between 1.0e-06 and 0.1 within the log-space to better capture backbone curvature at lower strains, i.e., the sample points are given by

\begin{equation}
\gamma_{j}=10^{[-6+5j/(n-1)]}\,\,\,,\,\,\,j=0,1,2,...,n-1
\end{equation}

### Results

The hysteresis stress-strain curve in the ZX plane is shown in [darendeli-hysteresis-loop].

!plot scatter filename=test/tests/materials/isoil/gold/darendeli_hysteresis.csv
              data=[{'x':'strain_zx_el', 'y':'stress_zx_el', 'name':'Hysteresis Loop in the zx-plane'}]
              layout={'xaxis':{'title':'strain_zx'},
                      'yaxis':{'title':'stress_zx (kPa)'},
                      'title':'Hysteresis Loop in the ZX Plane'}
              id=darendeli-hysteresis-loop
              caption=Plot of the element hysteresis stress-strain curve in the ZX plane over the duration of the simulation.

Since the engineering strain in the ZX plane, $\gamma_{zx}$, is equal to two times the tensor shear strain, $\epsilon_{zx}$, it was necessary to multiply the strains output by MOOSE TensorMechanics by two to get the shear stresses, $\tau_{zx}$, which correspond to those determined by [shear]. Next, the engineering shear stress-strain curve was superimposed over the 100 points, calculated by hand with [shear], as shown in [darendeli-backbone-curve]. The backbone curve was obtained in MOOSE by sampling the data during the initial load cycle, i.e., from 3 seconds to 4 seconds (see [#problem-statement]). It is clear from [darendeli-backbone-curve] that the analytical solution of the Darendeli model was consistent with the MASTODON one.

!media media/materials/isoil/darendeli_backbone.png
       id=darendeli-backbone-curve
       caption=The analytical Darendeli backbone curve sampled at 100 points and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds).
       style=width:100%
<!--
  TODO: Need to change this plot to be generated using Plotly.
-->

### Complete Input File

!listing test/tests/materials/isoil/HYS_darendeli_verification.i
