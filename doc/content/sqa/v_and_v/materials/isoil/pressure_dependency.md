## Pressure Dependency id=pressure-dependency

A procedure to verify the ability to vary the shear yield strength and the shear modulus to account for the pressure dependent stress-strain behavior of soils is presented in this section. This discussion is split into three subsections: [#strength], [#stiffness], and [#strength-stiffness]. In all cases, the element was assigned the properties listed in [p-ref-mat-props].

!table id=p-ref-mat-props caption=Soil Properties.
| Property | Symbol | Value |
| - | - | - |
| Density | $\rho$ | 2000 kg/m^3^ |
| Poisson's Ratio | $\nu$ | 0.3 |
| Mass-proportional Damping | $\eta$ | 7.854 |
| Stiffness-proportional Damping | $\zeta$ | 6.366e-05 |

The backbone curve was defined as shown in [p-ref-backbone-input].

!plot scatter filename=test/tests/materials/isoil/gold/stress_strain20.csv
              data=[{'x':'strain', 'y':'stress', 'name':'User-defined Backbone Curve'}]
              layout={'xaxis':{'title':'Strain'},
                      'yaxis':{'title':'Stress (Pa)'},
                      'title':'User-defined Backbone Curve'}
              id=p-ref-backbone-input
              caption=The user-defined backbone curve input into MASTODON to control the stress-strain behavior of the soil using I-Soil.

To conserve dynamic equilibrium in the first time-step, the element was initialized with the following state of stress, which corresponds to three times the acceleration of gravity, i.e., $g = -29.43 m/s^{2}$ (see [Initial Stresses](manuals/user/index.md#initial_stresses-user) for more information on stress initialization):

\begin{equation}
\sigma_{initial}=[-12613\,\,\,\,0\,\,\,\,0\,\,\,\,0\,\,\,\,-12613\,\,\,\,0\,\,\,\,0\,\,\,\,0\,\,\,\,-29430]^{\mathrm{T}}\,\,\,\text{Pa}
\end{equation}

Then a body force equal to three times the force of gravity was applied thereby increasing the vertical pressure experienced by the element. Finally a ramp displacement was applied in the manner described in [#problem-statement].

I-Soil is capable of varying the shear yield strength and the shear modulus as a function of engineering strain, $\gamma$, mean effective stress, $p$, a specified reference pressure, $p_{ref}$, and a tension pressure cutoff value, $p_0<0$. Assuming positive compression, MASTODON computes the mean effective stress as

\begin{equation}
p=-\frac{\sigma_{00}+\sigma_{11}+\sigma_{22}}{3}
\end{equation}

([!citet](numanoglu2017phd)) provided the following formula to model pressure dependent shear strength using I-Soil:

\begin{equation}
\label{p-ref-strength}
\frac{\tau_{y}(p,\gamma)}{\tau_{y}(p_{ref},\gamma)}=\sqrt{\frac{a_{0}+a_{1}(p-p_{0})+a_{2}(p-p_{0})^{2}}{a_{0}+a_{1}p_{ref}+a_2p_{ref}^{2}}}
\end{equation}

where $\tau_{y}(p_{ref})$ is the unmodified shear strength determined from the backbone curve and $a_{0}$, $a_{1}$, and $a_{2}$ are pressure dependent strength coefficients. The formula to model pressure dependent shear modulus is as follows:

\begin{equation}
\label{p-ref-modulus}
G=G_{0}\Big(\frac{p-p_{0}}{p_{ref}}\Big)^{b}
\end{equation}

where $G_{0}$ is the unmodified shear modulus determined from the backbone curve and $b$ is a pressure dependent stiffness coefficient. In all tests presented in this section, the tensile cutoff value was set to -1 Pa.

### Pressure Dependent Strength id=strength

The five cases described in [strength-cases] were used to investigate the behavior of [p-ref-strength] with varying parameters, $a_{0}$, $a_{1}$, $a_{2}$, and $p_{ref}$. In all cases, the pressure dependent stiffness parameter, $b$, was set to zero. Therefore, [p-ref-modulus] was anomalously canceled in these models.

!table id=strength-cases caption=Five combinations of $a_{0}$, $a_{1}$, $a_{2}$, and $p_{ref}$ used to investigate their effect on [p-ref-strength].
| Case No. | $a_{0}$ | $a_{1}$ | $a_{2}$ | $p_{ref}$ |
| - | - | - | - | - |
| 1 | 0 | 0 | 1 | 6072.86 Pa |
| 2 | 0 | 1 | 0 | 6072.86 Pa |
| 3 | 1 | 0 | 0 | 6072.86 Pa |
| 4 | 0 | 1 | 0 | 29430 Pa |
| 5 | 0 | 1 | 0 | 42043 Pa |

#### Results

The engineering shear stress-strain backbone curve results for Cases 1, 2, and 3 are presented in [strength-comparison1], and those for Cases 2, 4, and 5 are presented in [strength-comparison2]. Both figures show their results along with the user-defined backbone curve shown in [p-ref-backbone-input]. The backbone curves were obtained in MOOSE by sampling the data during the initial load cycle, i.e., from 3 seconds to 4 seconds (see [#problem-statement]). It can be seen in [strength-comparison1] and [strength-comparison2] that MASTODON effectively modifies the shear strength. In some cases, stiffness changes on those strain increments for which the mean effective stress exceeds $\tau_{y}(p,\gamma)$ can also be observed in these two figures, even though $b=0$. This is reasoned by the fact that, at a constant rate of strain, a change in strength of a material naturally (and numerically) leads to one in stiffness.

!media media/materials/isoil/pressure_dependent_strength1.png
       id=strength-comparison1
       caption=The user-defined backbone curve and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds) for for pressure dependent strength Cases 1, 2, and 3.

!media media/materials/isoil/pressure_dependent_strength2.png
       id=strength-comparison2
       caption=The user-defined backbone curve and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds) for pressure dependent strength Cases 2, 4, and 5.

+Qualitative Analysis:+ Since setting $a_{2}=1$ and $a_{1}=0$ causes the shear strength to increase in proportion to $(p-p_{0})/p_{ref}$, while setting $a_{2}=0$ and $a_{1}=1$ causes it to increase in proportion to $\sqrt{(p-p_{0})/p_{ref}}$, [strength-comparison1] shows that Case 1 is significantly stronger than Case 2. Also, since setting $a_{0}=1$ while setting the others to zero causes the right-hand-side of [p-ref-strength] to be equal to unity, [strength-comparison1] shows that Case 3 is practically equivalent to the original backbone curve (the slight discrepancy arises from the presence of initial stresses, the tensile cutoff value, $p_{0}=-1$, and the application of the body force). Cases 4 and 5 are essentially Case 2 except with different $p_{ref}$ values, hence, Case 2 was included in [strength-comparison2] for comparison. This plot shows that setting the reference pressure equal to the applied vertical pressure (Case 4) causes the shear strength to remain virtually unmodified, while setting it lower (Case 2) causes the shear strength to increase and setting it higher (Case 5) causes the shear strength to decrease. These observations made from each of the five cases are arithmetically consistent with [p-ref-strength].

#### Complete Input File

!listing test/tests/materials/isoil/HYS_pressure_dependent_strength.i

### Pressure Dependent Stiffness id=stiffness

The three cases described in [stiffness-cases] were used to investigate the behavior of [p-ref-modulus] with different values for the parameter $b$. In all cases, $p_{ref}$ was set to 6072.86 Pa. and the pressure dependent strength parameters were set to the following: $a_{0}=1$, $a_{1}=0$, and $a_{2}=0$. Therefore, [p-ref-strength] was anomalously canceled in these models.

!table id=stiffness-cases caption=Three different values of $b$ used to investigate its effect on [p-ref-modulus].
| Case No. | $b$ |
| - | - |
| 1 | 1.0 |
| 2 | 0.5 |
| 3 | 0.0 |

#### Results

The engineering shear stress-strain backbone curve results for all cases are presented in [stiffness-comparison]. The results are shown along with the user-defined backbone curve shown in [p-ref-backbone-input]. The backbone curves were obtained in MOOSE by sampling the data during the initial load cycle, i.e., from 3 seconds to 4 seconds (see [#problem-statement]). It can be seen in [stiffness-comparison] that MASTODON effectively modifies the shear modulus. For reasons similar to why the curves shown in [strength-comparison1] and [strength-comparison2] exhibit stiffness changes, strength changes, particularly at the lower strains of Case 1 and Case 2, can also be observed in this figure, even though $a_{0}=1$, $a_{1}=0$, and $a_{2}=0$. However, the ultimate shear strength at which the material completely fails, which corresponds to the maximum stress value defined by the backbone curve ([p-ref-backbone-input]), remains the same.

!media media/materials/isoil/pressure_dependent_stiffness.png
       id=stiffness-comparison
       caption=The user-defined backbone curve and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds) for the pressure dependent stiffness cases.

+Qualitative Analysis:+ [stiffness-comparison] shows that Case 1 results in a greater stiffness increase than Case 2. Also, since [p-ref-modulus] results in $G=G_{0}$ when $b=0$, Case 3 is practically equivalent to the original backbone curve (the slight discrepancy arises from the presence of initial stresses, the tensile cutoff value, $p_{0}=-1$, and the application of the body force). These observations made from each of the three cases are arithmetically consistent with [p-ref-modulus].

#### Complete Input File

!listing test/tests/materials/isoil/HYS_pressure_dependent_stiffness.i

### Pressure Dependent Strength and Stiffness id=strength-stiffness

The two cases described in [strength-stiffness-cases] were used to simultaneously investigate the behavior of both [p-ref-strength] and [p-ref-modulus] with varying parameters, $a_{1}$ and $a_{2}$, while setting $a_{0}=0$ and $b=0.5$ for both cases. These cases should cause both the shear strength and the shear modulus of the element to be modified. In both cases, $p_{ref}$ was set to 6072.86 Pa.

!table id=strength-stiffness-cases caption=Two combinations of $a_{0}$, $a_{1}$, $a_{2}$, and $b$ used to investigate their simultaneous effect on both [p-ref-strength] and [p-ref-modulus].
| Case No. | $a_{0}$ | $a_{1}$ | $a_{2}$ | $b$ |
| - | - | - | - | - |
| 1 | 0 | 0 | 1 | 0.5 |
| 2 | 0 | 1 | 0 | 0.5 |

#### Results

The engineering shear stress-strain backbone curve results for both cases are presented in [strength-stiffness-comparison]. The results are shown along with the user-defined backbone curve shown in [p-ref-backbone-input]. The backbone curves were obtained in MOOSE by sampling the data during the initial load cycle, i.e., from 3 seconds to 4 seconds (see [#problem-statement]). It can be seen in [strength-stiffness-comparison] that MASTODON effectively modifies both the strength and stiffness for a particular strain increment. Specifically, one should compare the curve shown in this figure to those in [strength-comparison1], [strength-comparison2], and [stiffness-comparison].

!media media/materials/isoil/stiffness_and_strength_pressure_dependency.png
       id=strength-stiffness-comparison
       caption=The user-defined backbone curve and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds) for the simultaneous strength and stiffness pressure dependency cases.

+Qualitative Analysis:+ [strength-stiffness-comparison] shows that Case 1 results in a greater shear strength increase than Case 2, but their shear modulus increases are equivalent since both used $b=0.5$. Note that Case 1 and Case 2 shown in [strength-stiffness-comparison] have equal shear strengths to Case 1 and Case 2 shown in [strength-comparison1], but the former cases are stiffer than the latter. These observations made from each of the two cases are arithmetically consistent with [p-ref-strength] and [p-ref-modulus].

#### Complete Input File

!listing test/tests/materials/isoil/HYS_stiffness_and_strength_pressure_dependency.i
