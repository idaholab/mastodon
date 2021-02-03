# Side Moment Postprocessor

!syntax description /Postprocessors/SidesetMoment

## Description

`SidesetMoment` computes the integral of the stress tensor $\underline{\sigma}$
along vector $\vec{e}$ or pressure $p$ acting on a sideset $\partial S$ multiplied
by the lever arm. When stress tensor is given as an input, `SidesetMoment` is computed as:

\begin{equation}
  M = \int_{\partial S}~\vec{n}^T~\underline{\sigma}~\vec{e}~(\vec{x}-\vec{r})^T~\vec{l}~\partial S,
\end{equation}

where $M$ is the sideset moment, $\vec{x}$ is the current quadrature point, $\vec{r}$
is the reference point, and $\vec{l}$ is the lever arm direction vector. The product
$(\vec{x}-\vec{r})^T~\vec{l}$ gives the required lever arm which is a scalar. When
pressure is given as an input, `SidesetMoment` is computed as:

\begin{equation}
  M = \int_{\partial S}~p~(\vec{x}-\vec{r})^T~\vec{l}~\partial S,
\end{equation}

The figure below illustrates `SidesetMoment` for a simple example. Moment is to be
computed about the z-axis (out-of-plane) at the reference point using either the
stress tensor or the pressure at the right boundary. If the stress tensor is used,
the normal vector to the boundary is $\vec{n} = (1, 0, 0)$ and the desired stress
direction is along x-axis (i.e., $\vec{e} = (1, 0, 0)$). The product $\vec{n}^T~\underline{\sigma}~\vec{e}$
 will now result in a scalar. If the pressure is used, no operations on $p$ are necessary
 since it is a scalar and acts normal to the boundary. $(\vec{x}-\vec{r})^T$ is the
 relative distance vector between the the current quadrature and reference points.
 The leverarm direction vector is $(0, 1, 0)$. The product $(\vec{x}-\vec{r})^T~\vec{l}$
 thus gives the desired lever arm and this is also a scalar.  

!media media/postprocessors/SidesetMoment.png
      style=width:100%;margin-right:0px;float:center;
      id=fig:SidesetMoment
      caption=Illustration of the SidesetMoment Postprocessor with a simple example.

!syntax parameters /Postprocessors/SidesetMoment

!syntax inputs /Postprocessors/SidesetMoment

!syntax children /Postprocessors/SidesetMoment
