### Newmark time integration id=newmark-beta

In Newmark time integration [!citep](newmark1959amethod), the acceleration and velocity at $t+\Delta t$ are written in terms of the displacement ($\mathbf{u}$), velocity ($\mathbf{\dot{u}}$) and acceleration ($\mathbf{\ddot{u}}$) at time $t$ and the displacement at $t+\Delta t$.

\begin{equation} \label{eqn:Newmark}
\begin{aligned}
\mathbf{\ddot{u}}(t+\Delta t) &= \frac{\mathbf{u}(t+\Delta t)-\mathbf{u}(t)}{\beta \Delta t^2} - \frac{\mathbf{\dot{u}}(t)}{\beta \Delta t}+\frac{\beta -0.5}{\beta}\mathbf{\ddot{u}}(t) \\
\mathbf{\dot{u}}(t+ \Delta t) &= \mathbf{\dot{u}}(t)+ (1-\gamma)\Delta t \mathbf{\ddot{u}}(t) + \gamma \Delta t \mathbf{\ddot{u}}(t+\Delta t)
\end{aligned}
\end{equation}

In the above equations, $\beta$ and $\gamma$ are Newmark time integration parameters. Substituting the above two equations into the equation of motion will result in a linear system of equations ($\mathbf{Au}(t+\Delta t) = \mathbf{b}$) from which $\mathbf{u}(t+\Delta t)$ can be estimated.

For $\beta = 0.25$ and $\gamma = 0.5$, the Newmark time integration scheme is the same as the trapezoidal rule. The trapezoidal rule is an unconditionally stable integration scheme, i.e., the solution does not diverge for any choice of $\Delta t$, and the solution obtained from this scheme is second order accurate. One disadvantage with using trapezoidal rule is the absence of numerical damping to damp out any high frequency numerical noise that is generated due to the discretization of the equation of motion in time.

The Newmark time integration scheme is unconditionally stable for $\gamma \ge \frac{1}{2}$ and $\beta \ge \frac{1}{4}\gamma$. For $\gamma > 0.5$, high frequency oscillations are damped out, but the solution accuracy decreases to first order.
