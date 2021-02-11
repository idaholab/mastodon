# Sideset Moment Postprocessor

!syntax description /Postprocessors/SidesetMoment

## Description

`SidesetMoment` computes the integral of the stress tensor $\underline{\sigma}$
along vector $\vec{e}$ or pressure $p$ acting on a sideset $\partial S$ multiplied
by the lever arm. When stress tensor is given as an input, `SidesetMoment` is computed as:

\begin{equation}
  M = \int_{\partial S}~\bigg( \big(\vec{n}~\cdot~\underline{\sigma}~\vec{e}\big) \vec{n} \times ~(\vec{x}-\vec{r}) \bigg) \cdot \vec{l}~\partial S,
\end{equation}

where $M$ is the sideset moment, $\vec{x}$ is the current quadrature point, $\vec{r}$
is the reference point, and $\vec{l}$ is the moment direction vector. The quantity
 $\big(\vec{n}~\cdot~\underline{\sigma}~\vec{e}\big)$ gives a scalar and when multiplied
 with $\vec{n}$ gives a force-like vector. The cross-product of this force-like vector
and the relative distance vector between the current `qp` and the reference point $(\vec{x}-\vec{r})$
 gives the moment vector. This moment vector when dotted with the `moment_direction` vector
 gives the scalar moment about the required direction. When using pressure, the computation
 of the `SidesetMoment` is similar, except that the force-like vector is simply $p \vec{n}$:

\begin{equation}
  M = \int_{\partial S}~\bigg( p \vec{n} \times ~(\vec{x}-\vec{r}) \bigg) \cdot \vec{l}~\partial S,
\end{equation}

The figure below illustrates `SidesetMoment` for a simple example. Moment is to be
computed about the z-axis (out-of-plane) at the reference point using either the
stress tensor or the pressure at the right boundary. If the stress tensor is used,
the normal vector to the boundary is $\vec{n} = (1, 0, 0)$ and the desired stress
direction is along x-axis (i.e., $\vec{e} = (1, 0, 0)$). The quantity $\big(\vec{n}~\cdot~\underline{\sigma}~\vec{e}\big) \vec{n}$ is a force-like vector. If the pressure is used, no operations on $p$ are necessary
 since it is a scalar, and $p \vec{n}$ is the force-like vector. $(\vec{x}-\vec{r})$ is the
 relative distance vector between the the current quadrature and reference points. The cross-product
 of the force-like vector with the relative distance vector gives the moment vector
$\Bigg($ either $\bigg( \big(\vec{n}~\cdot~\underline{\sigma}~\vec{e}\big) \vec{n} \times ~(\vec{x}-\vec{r}) \bigg)$ or $\bigg( p \vec{n} \times ~(\vec{x}-\vec{r}) \bigg)$ $\Bigg)$. Dotting this moment vector
 with the moment direction vector $\vec{l} = (0, 0, 1)$ gives the scalar moment about the z-axis.

!media media/postprocessors/SidesetMoment.png
      style=width:100%;margin-right:0px;float:center;
      id=fig:SidesetMoment
      caption=Illustration of the SidesetMoment Postprocessor for a simple example.

!syntax parameters /Postprocessors/SidesetMoment

!syntax inputs /Postprocessors/SidesetMoment

!syntax children /Postprocessors/SidesetMoment
