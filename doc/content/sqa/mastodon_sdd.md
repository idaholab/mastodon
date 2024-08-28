!template load file=app_sdd.md.template app=MASTODON category=mastodon

!template! item key=introduction
Calculating earthquake response using response history analysis involves performing a transient analysis and solving the equation of motion using finite-element methods. {{app}} uses the MOOSE framework and several of its modules to solve this problem but includes various material models, boundary conditions, postprocessors, and other objects that are relevant to earthquake engineering. {{app}} also uses the MOOSE framework to account for the coupling between different physics such as fluid structure interaction. This document describes the system design of {{app}}.
!template-end!

!template! item key=system-scope
!include mastodon_srs.md start=system-scope-begin end=system-scope-finish
!template-end!

!template! item key=dependencies-and-limitations
{{app}} inherits the
[software dependencies of the MOOSE framework](framework_sdd.md#dependencies-and-limitations),
with no additional dependencies.
!template-end!

!template! item key=design-stakeholders
!include framework_sdd.md start=design-stakeholders-begin end=design-stakeholders-finish
!template-end!

!template! item key=system-design
{{app}} relies on MOOSE to solve the equation of motion using a transient analysis. The MOOSE design is based on the concept of modular code objects that define all of the aspects of the physics model. {{app}} follows
this design, providing code objects that define specific aspects of the
solutions for its physics that derive from the base classes defined by the
MOOSE framework and the modules that it depends on.

{{app}} provides specialized `Material` classes that define the constitutive behavior of soils, structures, and seismic protective systems that are often used in earthquake engineering. {{app}} also provides `DiracKernels` that describe the energy release during an earthquake, `BC`s that create an absorbing boundary to simulate an infinite domain, and miscellaneous `Action` and
`VectorPostprocessor` classes to facilitate various aspects of these simulations. Much of the functionality of {{app}} is provided by the MOOSE modules that it builds on.
!template-end!

!template! item key=system-structure
{{app}} relies on the MOOSE framework to provide the core functionality of
solving the dynamic equation of motion using the finite element method. It also relies
on the MOOSE modules such as [Solid Mechanics](solid_mechanics/index.md) and
[Fluid Structure Interaction](fsi/index.md) for much of its core functionality.
The structure of {{app}} is based on defining C++ classes that derive from
classes in the MOOSE framework or modules that provide functionality that is
specifically tailored to earthquake engineering and seismic risk assessment problems. By using the
interfaces defined in MOOSE base classes for these classes, {{app}} is able to
rely on MOOSE to execute these models at the appropriate times during the
simulation and use their results in the desired ways.
!template-end!

!syntax complete subsystems=False actions=False objects=False
!template-end!
