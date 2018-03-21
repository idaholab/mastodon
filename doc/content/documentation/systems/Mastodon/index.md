# Mastodon Block

## Description

The Mastodon block creates default parameters that are used in most seismic analyses, so that users
don't have to manually define them. These include parameters such as Variables and AuxVariables,
Kernels and AuxKernels, executioner settings, output and post processing settings, etc. The automatic
creation of these parameters can also be turned off and the parameters can be created manually, if
needed.

!alert note
The Mastodon block and the associated sub blocks are a work in progress. They will be extended to
other parameters in the near future.

The Mastodon block is organized into several sub blocks that create different parameters. Currently
included sub blocks are as follows:

1. [Model](Mastodon/Model/MastodonModelAction.md)
2. [Outputs](Mastodon/Outputs/MastodonOutputsAction.md)

The usage of the Mastodon block along with the currently implemented sub blocks is illustrated in the
example input file below.

!listing test/tests/mastodonblock/combined/mastodon_block_actions.i block=Mastodon
