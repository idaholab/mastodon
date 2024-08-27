!template load file=app_srs.md.template app=MASTODON category=mastodon

!template! item key=system-scope
!! system-scope-begin

MASTODON models vibration response of structures with a prime focus on earthquakes. Applications include earthquake response of structures like nuclear power plant buildings and dams, linear and nonlinear site response and soil-structure interaction analysis, and large-scale source-to-site earthquake simulations. MASTODON can be used to both model simple 1D and 2D structures built using beam and shell elements, as well as more complex geometries represented using 3D continuum elements. MASTODON is primarily intended for response history analysis for both linear and nonlinear problems including material and geometric nonlinearities. It includes the most commonly-used and versatile hysteretic soil material models, and seismic protective devices.  MASTODON is also capable of performing acoustic fluid structure interaction simulations through the Fluid Structure Interaction (FSI) module, seismic analysis of structures undergoing aging-related deterioration through inclusion of Blackbear, and uncertainty quantification (UQ) studies through the Stochastic Tools Module (STM). MASTODON also includes automated seismic risk assessment capabilities using the Multiple Stripes Analysis, fragility evaluation. It can also provides Fault Tree and Event Tree analysis capability through a Python library that is included in the MASTODON package.

!! system-scope-finish
!template-end!

!template! item key=system-purpose
!! system-purpose-begin
The purpose of MASTODON is to evaluate the earthquake response of structures and systems both in a deterministic and probabilistic fashion, and also evaluate seismic risk. Through the capability of performing complex response history analyses, MASTODON can be used to evaluate both local and global responses, and also the risk of unwanted events in complex systems such as nuclear power plants.

!! system-purpose-finish
!template-end!

!template! item key=assumptions-and-dependencies
{{app}} has no constraints on hardware and software beyond those of the MOOSE framework and modules listed in their respective SRS documents, which are accessible through the links at the beginning of this document.

{{app}} provides access to a number of code objects that perform computations such as material behavior and boundary conditions. These objects each make their own physics-based assumptions, such as the units of the inputs and outputs. Those assumptions are described in the documentation for those individual objects.
!template-end!

!template! item key=user-characteristics
{{app}} users will fall into the following three categories:

- +{{app}} Developers+: These are the core developers of {{app}}. They are responsible for designing, implementing, and maintaining the software, while following and enforcing its software development standards.
- +Developers+: These are scientists or engineers that modify or add capabilities to {{app}} for their own purposes, which may include research or extending its capabilities. They will typically have a background in structural or mechanical engineering, and in modeling and simulation techniques, but may have more limited background in code development using the C++ language. In many cases, these developers will be encouraged to contribute code back to {{app}}.
- +Analysts+: These are users that run {{app}} to run simulations, but do not develop code. The primary interface of these users with {{app}} is the input files that define their simulations. These users may interact with developers of the system requesting new features and reporting bugs found.
!template-end!

!template! item key=information-management
{{app}} as well as the core MOOSE framework in its entirety will be made publicly available on an appropriate repository hosting site. Day-to-day backups and security services will be provided by the hosting service. More information about backups of the public repository on [!ac](INL)-hosted services can be found on the following page: [sqa/github_backup.md]
!template-end!

!template! item key=policies-and-regulations
!include framework_srs.md start=policies-and-regulations-begin end=policies-and-regulations-finish
!template-end!

!template! item key=packaging
No special requirements are needed for packaging or shipping any media containing the [!ac](MOOSE) and {{app}} source code. However, some [!ac](MOOSE)-based applications that use the {{app}} code may be export-controlled, in which case all export control restrictions must be adhered to when packaging and shipping media.
!template-end!

!template item key=reliability
The regression test suite will cover at least 90% of all lines of code at all times. Known
regressions will be recorded and tracked (see [#maintainability]) to an independent and
satisfactory resolution.