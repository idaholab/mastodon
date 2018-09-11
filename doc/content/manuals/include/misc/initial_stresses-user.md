## Initial Stresses

Initial stress command is available to apply the prescribed initial stresses to the finite element
domain. If the user applies initial stress state that is equal to that of gravity loadings, upon
transient analysis, no vibration occurs and the domain reaches equilibrium with zero strains. This
eliminates the necessity of conducting separate analysis for free-field problems and allows the user
to start directly with a dynamic analysis such as base shaking etc. The following command can be used
within the material definition to activate the initial stresses in the material block:

```
initial_stress = '-4.204286 0 0  0 -4.204286 0  0 0 -9.810'
```

The nine numerical values fill the stress tensor in the following order - xx, xy, xz, yz, yy, yz, zx,
zy, and zz. The above example command activates -4.2025 units of stress along both xx and yy
direction, and -9.81 units of stress along zz direction.  These stresses are the gravity stresses at
the mid-depth of the element in [fig:single_element] with density = 2. Therefore, the element will
immediately start with gravity stress conditions without any elastic deformations or stress
fluctuations.
