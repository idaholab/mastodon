!!template load file=app_srs.md.template app=MASTODON category=mastodon

!!!
!SQA-load system_requirements_specification.md

!SQA-template-item project_description

!alert construction
Work in Progress

The template for this page is currently under development, the completed SRS is available
as a pdf here: [MASTODON SRS](https://hpcgitlab.inl.gov/idaholab/mastodon/uploads/d50895b47f1cf489b504bf3036d79bae/MastodonSoftwareRequirementsSpecification.pdf).

MASTODON is a nonlinear, three-dimensional seismic soil-structure interaction analysis framework.

!END-template-item

!SQA-template-item system_scope

Multi-hazard Analysis for STOchastic time-DOmaiN phenomena (MASTODON) is a finite element application
that analyzes the response of 3-D soil-structure systems to earthquakes. MASTODON currently focuses
on the simulation of seismic events and has the capability to perform extensive 'source-to-site'
simulations including earthquake fault rupture, nonlinear wave propagation and nonlinear
soil-structure interaction (NLSSI) analysis. MASTODON is being developed to be a dynamic
probabilistic risk assessment framework that enables analysts to not only perform deterministic
analyses, but also easily perform probabilistic or stochastic simulations for the purpose of risk
assessment.

MASTODON is a MOOSE-based application and performs finite-element analysis of the dynamics of solids,
mechanics of interfaces and porous media flow. It is equipped with numerical material models of dry
and saturated soils including a nonlinear hysteretic soil model, and a uP-U model for saturated
soil, as well as structural materials such as reinforced concrete. It is also equipped with
interface models that simulate gapping, sliding and uplift at the interfaces of solid media such as
the foundation-soil interface of structures. MASTODON also includes absorbing boundary models for
the simulation of infinite or semi-infinite domains, fault rupture model for seismic source
simulation, and the domain reduction method for the input of complex, three dimensional wave fields.
Since MASTODON is a MOOSE-based application, it can efficiently solve problems using either standard
workstations or very large high-performance computers.

!END-template-item

!SQA-template-item system_context

!alert construction
Under Development

The current MOOSE template for the SRS is not complete and the sections do not align with
the SRS for {{PROJECT}}. As development continues sections will be more configurable to meet
the needs of application.

!END-template-item

!SQA-template-item system_requirements

### Technical and Functional Requirements

!SQA-requirement-list title=Time Integration
    F1.1 The system shall be capable of performing time integration using the Newmark Beta method
    F1.2 The system shall be capable of performing time integration using the HHT method

!SQA-requirement-list title=Small Strain Damping
    F2.1 The system shall allow Rayleigh damping
    F2.2 The system shall allow frequency-independent damping

!SQA-requirement-list title=Soil Layers and Meshing
    F3.1 The system shall allow automatic mesh adaptivity to maintain a user-defined number of solid hexagonal elements per wavelength
    F3.2 The system shall allow automatic mesh generation based on a user-defined dynamic shear wave velocity
    F3.3 The system shall allow for scenarios with both horizontal and non-horizontal soil layers

!SQA-requirement-list title=Material Models
    F4.1 The system shall simulate a linear elastic material
    F4.2 The system shall simulate a nonlinear soil material (ISoil)

!SQA-requirement-list title=Foundation-Soil Interface Models
    F5.1 The system shall allow for the simulation of a foundation-soil interface using the thin-layer method
    F5.2 The system shall allow for the simulation of a foundation-soil interface using cyclic frictional contact algorithms

!SQA-requirement-list title=Special Boundary Conditions
    F6.1 The system shall allow Domain Reduction Method (DRM)
    F6.2 The system shall allow non-reflecting boundary conditions
    F6.3 The system shall allow a preset acceleration boundary condition
    F6.4 The system shall allow a seismic force boundary condition

!SQA-requirement-list title=Earthquake Fault Rupture
    F7.1 The system shall allow for fault rupture using multiple point sources

!SQA-requirement-list title=Post-Processing
    F8.1 The system shall allow the calculation of response histories. Acceleration, velocity, and displacement time histories are used by analysts to evaluate structural response.
    F8.2 The system shall allow the calculation of response spectra
    F8.3 The system shall allow for the calculation of Housner Spectrum Index (HSI)

### Design Inputs, Outputs and Design Constraints

!SQA-requirement-list title=Design Inputs
    DI1.1 The system shall use the input file syntax as defined by MOOSE
    DI1.2 The system shall employ the dynamic time domain input file. Uses time integration.
    DI1.3 The system shall allow the location of the interfaces to be provided as input for scenarios with horizontal soil layers
    DI1.4 The system shall allow images (.jpg, .png, etc.) of the soil profile to be provided as input for mesh generation for scenarios where the soil layers are non-horizontal and non-planar

!SQA-requirement-list title=Design Constraints
    DC1.1 The system shall be implemented in MOOSE, INL's HPC development and runtime framework
    DC1.2 The system shall be written with object oriented programming language C++

!SQA-requirement-list title=Output File Structure
    DO1.1 The system shall use the output capabilities of MOOSE framework
    DO1.2 The system shall provide the PRA output (file structure TBD)

### System Interfaces

!SQA-requirement-list
    SI1.1 MOOSE
    SI1.2 BlackBear

### Installation Considerations

Detailed installation instructions, including the following installation considerations, will be included in the MASTODON User's Manual.

!SQA-requirement-list
    IC1.1 The user shall obtain a valid license.
    IC1.2 MOOSE Environment Setup:  The user's system environment must be set up for MOOSE.
    IC1.3 Code repository access: The user must establish a secure connection between their computer and the system where the source code repository is hosted.
    IC1.4 Code Checkout:  It is necessary to build LibMesh before building any application.  Once LibMesh has been successfully compiled, the system can be compiled.
    IC1.5 Verification Testing:  Once the system has been compiled successfully, it is required to run the tests to make sure the version of the code installed is running correctly.

!END-template-item
!!!
