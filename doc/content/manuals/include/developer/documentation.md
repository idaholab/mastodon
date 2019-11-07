## Documenting new objects

Prior to merging the code, MOOSE and MASTODON require adequate documentation
describing the implemented feature, such as materials, post processors, etc.
Several tools are available for MOOSE-based documentation (see
[mooseframework.org](https://www.mooseframework.org)).
For each new feature, the following documentation files are required.

#### Documentation of C++ objects

For each C++ object created, the developer should include the documentation as
prescribed by MOOSE documentation requirements, see
[mooseframework.org](https://www.mooseframework.org).

#### User manual

For each feature (such as material, boundary condition, etc., which may include
one of more C++ objects), the developer must create one documentation file that
describes the method of usage of this feature. This file must be named as
`<feature_name>-user.md` and be placed in the appropriate sub folder in
`content/manuals/include`. A rough template for the user manual document is
below.

```
# <Name of the feature, e.g., lead-rubber isolator>

Introduction and link to the corresponding theory manual document.

Objects needed to simulate this feature. Links to the documentation of these
objects.

Associated actions, and usage in the Mastodon block, if available. Link to the
documentation of the corresponding Mastodon block action.

Any known quirks/limitations about the usage of the model that need to be known.

```

#### Theory manual

For each feature (such as material, boundary condition, etc., which may include
one of more C++ objects), the developer must create one documentation file that
describes the theory behind this feature. This file must be named as
`<feature_name>-theory.md` and be placed in the appropriate sub folder in
`content/manuals/include`. A rough template for the theory manual document is
below.

```
# <Name of the feature, e.g., lead-rubber isolator>

Introduction about what the feature does and a link to the corresponding user
manual document.

Description of the physics, or the theory behind the feature including
references, equations, figures, etc.

Limitations, if any, of the formulation.

```
