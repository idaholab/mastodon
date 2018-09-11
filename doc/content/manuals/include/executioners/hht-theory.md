#### Hilber-Hughes-Taylor (HHT) time integration

The HHT time integration scheme [citep:hughes2000thefinite] is built upon Newmark time integration method to provide an unconditionally stable and second order accurate numerical scheme with the ability to damp out high frequency numerical noise. Here, in addition to the Newmark equations, the equation of motion is also altered resulting in:

\begin{equation}
\rho\mathbf{\ddot{u}}(t+\Delta t) + \nabla \cdot [(1+\alpha) \sigma(t+\Delta t) - \alpha \sigma(t)] = \mathbf{F_{ext}}(t+\alpha\Delta t)
\end{equation}

Here, $\alpha$ is the HHT parameter. The optimum parameter combination to use for this time integration scheme is $\beta = \frac{1}{4}(1-\alpha)^2$, $\gamma = \frac{1}{2} - \alpha$, and $-0.3 \le \alpha \le 0$.
