github_edit: False

# SeismicForce Syntax
The input file syntax for SeismicForce with the the `[BCs]` block provides
syntax for creating the correct [SeismicForce](BCs/mastodon/SeismicForce.md) boundary
conditions for each direction components specified.

In some cases, the ground excitation is measured at a rock outcrop (where rock is found at surface
level and there is no soil above it). To apply this to a location where rock is say $10$m deep and
there is soil above it, a sideset is created at $10$m depth and the ground excitation (converted
into a stress) is applied at this depth. To apply ground excitation as a stress, the input function
should be given as ground velocity. More details about the conversion of ground velocity to stress
can be found in [SeismicForce](mastodon/SeismicForce.md).

## Example Usage
!listing tests/bcs/seismic_force/seismic_force_test_x_y.i block=BCs

!syntax parameters /BCs/SeismicForce/SeismicForceAction
