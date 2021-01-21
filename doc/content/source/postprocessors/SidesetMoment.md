# Side Moment Postprocessor

!syntax description /Postprocessors/SidesetMoment

## Description

`SidesetMoment` computes the integral of the stress tensor $\underline{\sigma}$
along vector $\vec{e}$ or pressure $p$ acting on a sideset $\partial S$ multiplied
by the lever arm $x$. When stress tensor is given as an input, `SidesetMoment` is computed as:

\begin{equation}
  M = \int_{\partial S}~\vec{n}^T~\underline{\sigma}~\vec{e}~x~\partial S,
\end{equation}

where $M$ is the sideset moment. `SidesetMoment` requires a `ref_point` with reference
 to which the `x` is evaluated and a `leverarm_direction` along which the lever arm
is computed. When pressure is given as an input, `SidesetMoment` is computed as:

\begin{equation}
  M = \int_{\partial S}~\vec{n}^T~p~x~\partial S,
\end{equation}

!syntax parameters /Postprocessors/SidesetMoment

!syntax inputs /Postprocessors/SidesetMoment

!syntax children /Postprocessors/SidesetMoment
