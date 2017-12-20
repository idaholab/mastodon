# Mastodon Block

## Description
The Mastodon block creates default parameters that are common for most seismic analyses, so that users don't have to manually define them. These parameters include Variables and AuxVariables, Kernels and AuxKernels, executioner settings, output and post processing settings, etc. The automatic creation of these parameters can also be turned off and the parameters can be created manually, if needed. Current options for the Mastodon block are below.

!!! Note
    Currently, the Mastodon block only sets Variables and AuxVariables. This will be extended to other parameters in the near future.

!syntax parameters /Mastodon/MastodonAddVariableAction
