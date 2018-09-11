## Executioner

The executioner input specifies the type of the analysis (the solver) and the corresponding parameters. There are two main types of executioners in MASTODON: steady and transient. Herein, an example is provided to run a transient analysis that will be used later on examples related to geotechnical earthquake engineering
applications.

!listing test/tests/materials/isoil/HYS_darendeli.i
         start=Executioner
         end=Postprocessors

The `type = Transient` command is self-explanatory and sets the executioner type to
transient. `solve_type = PJFNK` is the default option (preconditioned Jacobian Free Newton-Krylov) used
to solve the transient problems in MASTODON without constructing full stiffness matrix. `dt = 0.01` (is set to 0.01 sec here for demonstration) determines the initial time step and the user
is allowed to set a minimum time step, `dtmin`, for the analysis. `start_time` and `end_time` specifies
where the transient analysis will start and where it will end. Further information on available
options can be found in [/Executioner/index.md].
