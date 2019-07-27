### Rayleigh damping id=rayleigh-damping

Rayleigh damping is the most common form of classical damping used in modeling structural dynamic problems. The more generalized form of classical damping, Caughey Damping [!citep](caughey1960classical), is currently not implemented in MASTODON. Rayleigh damping is a specific form of Caughey damping that uses only the first two terms of the series. In this method, the viscous damping is proportional to the inertial contribution and contribution from the stiffness. This implies that in the matrix form of the governing equation, the damping matrix ($\mathbf{C}$) is assumed to be a linear combination of the mass ($\mathbf{M}$) and stiffness ($\mathbf{K}$) matrices, i.e., $\mathbf{C} = \eta \mathbf{M} +\zeta\mathbf{K}$. Here, $\eta$ and $\zeta$ are the mass and stiffness dependent Rayleigh damping parameters, respectively.

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


### Simulation of a constant damping ratio

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

### Rayleigh damping for soils

Small-strain material damping of soils is independent of loading frequency in frequency band of 0.01 Hz - 10 Hz ([!cite](menq2003), [!cite](shibuya2000damp),[!cite](lopresti1997damp), and [!cite](marmureanu2000damp)). The two mode Rayleigh damping is frequency dependent and can only achieve the specified damping at two frequencies while underestimating within and overestimating outside of these frequencies. The parameters $\eta$ and $\zeta$ for a given damping ratio can be calculated as follows:

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

In case of two mode Rayleigh damping, [!cite](kwok2007damp) suggests to use natural frequency and five times of it for the soil column of interest. In addition, selecting first mode frequency of soil column and higher frequency that corresponds to predominant period of the input ground motion is a common practice.

Heterogeneities of the wave travel path may introduce scattering effect which leads to frequency dependent damping ([!cite](campbell2009damp)). This type of damping is of the form ([!cite](withers2015memory)):

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
