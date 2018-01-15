# Model Block

##Description
The Model sub block in the [Mastodon](Mastodon/index.md) block automatically creates the model parameters based on the dimension of the model. Currently, these parameters include:

- displacement `Variables`
- velocity and acceleration `AuxVariables`
- inertia `Kernels`
- velocity, acceleration, stress and strain `AuxKernels`.

!!! Note
    The Mastodon block and the associated sub blocks are a work in progress. They will be extended to other parameters in the near future.

These parameters are created using the actions below:

!syntax actions /Mastodon/Model
