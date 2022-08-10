# Frequency Domain Dynamics

The following example presents a frequency domain analysis done in the MOOSE Tensor Mechanics module. A frequency domain analysis provides the structural response at a discrete frequencies where for each frequency, an independent steady state simulation is performed. This document provides an example of modeling a dynamic problem at a single frequency (time-harmonic problem).

Frequency domain analysis is often used to determine a frequency response function (FRF). An FRF describes the relationship between an input (frequency of the input forcing source) and output (displacement response of a system). For simple systems, an analytic FRF can be derived. For more complex systems, FRF is numerically obtained by sweeping over frequency range of interest and plotting frequency vs. the response of the system. The frequencies correspond to the peaks on the FRF plot are the natural modes of the system (the eigen/fundamental frequncies), whereas the displacement profiles correspond to those eigen freqeuncies are the mode shapes (eigen vectors).

Other applications of frequency domain dynamics are: (1) computation of band structure (dispersion curves) of lattices/metamaterial, (2) inverse design for vibration control, e.g. design a system so that it has as minimum/maximum response at particular frequencie/s, (3) material properties inversion/optimization given discrete responses.

On the other hand, frequency domain analyses can be advantageous over time domain counterpart in several cases, for example, when the frequency spectrum of a signal consists of a few frequencies, or, when it is needed to have a better control of the numerical dispersion.

# Problem Description

A a uniaxial beam, of length L=1, fixed on the left. On the right boundary, the displacement boundary condition is a cosine function, see [fig_cook].

!row!
!media media/examples/ex15_cantilever_beam.png
      style=width:60%;
      id=fig_cook
      caption=2D cantilever problem with a prescribed displacement boundary condition on the right end.
!row-end!

The PDE that mathematically describes this time-harmonic structural dynamics problem is as follows:
\begin{equation}
    -\nabla \cdot \sigma +\rho \frac{\partial^2 u}{\partial t^2}=0 \label{}
\end{equation}

For the boundary condition, we apply the sommerfeld radiation condition on the left, and a harmonic source (cosine) on the right as follows:
\begin{equation}
    \sqrt E \, \nabla u= i\sqrt \rho \, \frac{\partial u}{\partial t} \, \qquad at \, x=0,
    \\
    u= A \, cos(2 \pi f t) \, \qquad at \, x=L,\label{}
\end{equation}
where $f$ is the frequency, $A=0.5$, and  $2 \pi f=\omega=10$.

In equation 1, we consider Young's modulus and the density as constants and equal to unity.
Equation (1) can be written as:
\begin{equation}
     -\nabla \cdot \sigma + \frac{\partial^2 u}{\partial t^2} = 0 \label{eq3}
\end{equation}

Now, we consider the plane wave $u= r e^{i(\omega t - k x)}$ is a solution for equation (3), with $r$ being the complex amplitudes, and $k$ is the wave number and $i=\sqrt{-1}$. "hint: you can verify that $r e^{i(\omega t - k x)}$ is a solution by direct substitution in equation (3)", $\Longleftrightarrow$ $k^2= \frac{\omega^2}{c_p^2}$, where $k$ is the wave number, and $c_p=\sqrt \frac{E}{\rho}$ is the longitudinal wave velocity. As we noted before that Young's modulus and the density as constants and equal to unity, thus, $c_p=1$.

Now, equation (2) and the associated BCs. and can be written as,
\begin{equation}
     -\nabla \cdot \sigma- \omega^2 u = 0 \label{}
\end{equation}

\begin{equation}
    \nabla u= ik \, u \, \qquad at \, x=0,
    \\
    u= A \, cos(2 \pi f t) \, \qquad at \, x=L,\label{}
\end{equation}

Transforming equation (4) and the associated boundary conditions (5) in time to the frequency domain, we obtain:
\begin{equation}
     -\nabla \cdot \hat \sigma- \omega^2  U = 0 \label{}
\end{equation}
with the following boundary conditions:
\begin{equation}
    \nabla U= ik \, U \, \qquad at \, x=0,
    \\
U= 0.5 \, \qquad at \, x=L,\label{}
\end{equation}

where $\hat \sigma = \nabla U$ with $U(x,\omega)$ being the Fourier transform of $u(x,t)$.

!alert note title= Fourier Transfrom of Boundary Conditions
Not only the pde is transformed to the frequency domain, but also, we must transform all B.Cs as well. Transfroming $A cos(2\pi f t)$ into the frquency domain will result in an impulse at frequency = $f$ with amplitude of A.

When transform the time dependent variable $u$ to the frequency domain, the time-dependency will be omitted. The result is a nother variable $U$ that is frequency dependent. In general, $U$ is complex-valued function/variable in the form of $U_r+iU_i$, where $U_r$ and $U_i$ are the real and the imaginary part of $U$.\\ \\
At this stage, we split the complex sytem of equations into two $\textbf{real}$ systems, which yields two real-valued systems that live on the same mesh. The strong form PDEs becomes as follows:

\begin{equation}
    -\nabla \cdot \hat \sigma_r- \omega^2  U_r = 0
    \\
    -\nabla \cdot \hat \sigma_i- \omega^2  U_i = 0 \label{}
\end{equation}
with the following boundary conditions:
\begin{equation}
   \nabla U_r= -k U_i \quad ,\nabla U_i= k U_r\, \qquad at \, x=0,\\
   U_r=U_i=0 \, \qquad at \, x=L,
\end{equation}

Note that this decomposition is exact and there is no infromation missed while decompisition. Also note that, as a result of the radiation BCs on left, $U_r$ and $U_i$ are coupled, hence the two systems in (8).

The following is how we define the kernels of this problem,

!listing examples/ex15/1D_elastic_wave_propagation.i block=Kernels

and here is how we define the BCs. of this problem,

!listing examples/ex15/1D_elastic_wave_propagation.i block=BCs

!alert note
$u$ is a function of time and space $(x,t)$, whereas $U$ is function of space and frequency $(x,\omega)$.

The moose solution for this problem is shown below, see [fig_cook2].

!row!
!media media/examples/ex15_wavefieldandprofile.png
      style=width:40%;
      id=fig_cook2
      caption=the solution at $\omega =10$.
!row-end!

The analytical solution of equation (4) can be straight forward obtained knowing that we can express the solution of $U$ as a combination of sines and cosines representing the traveling and reflecting waves, i.e $U=C_1 e^{ikx} + C_2e^{-ikx}$.\\ \\
Applying the boundary conditions, and consider that the wave is traveling in the negative $x$ direction, we obtain, $C_1=0$ with $C_2=0.5/(e^{-ikL})$. So, the analytical solution is: $U=0.5 e^{ik(L-x)}.$
