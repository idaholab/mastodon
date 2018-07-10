# LinearSoilMaterial

!syntax description /Materials/LinearSoilMaterial

## Description

A material for computing the shear-wave speed ($v_s$) based on a linear relationship between shear
modulus ($G$) and density ($\rho$).

This material defines the shear-wave speed as:

\begin{equation}
v_s = \sqrt{\frac{G}{\rho}}
\end{equation}

The shear-wave speed may differ for different materials throughout the domain. Materials are defined
by "layer ids" using an elemental AuxVariable that defines a "layer id" for each element within the
mesh. Thus, the layer variable is a required parameter in the input.

Using the "layer id" to determine which value to utilize, this material will compute the shear-wave
speed based on the mapping of layer id to $G$ and $\rho$, as shown in the input snippet below.

!listing test/tests/materials/linear_soil/linear_soil.i block=Materials

!syntax parameters /Materials/LinearSoilMaterial

!syntax inputs /Materials/LinearSoilMaterial

!syntax children /Materials/LinearSoilMaterial
