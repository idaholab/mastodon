# ResponseHistoryMean

`ResponseHistoryMean` computes the mean history given the response histories of a list of nodes or a boundary. These nodal response histories are computed using the `ResponseHistoryBuilder` class. This vector is then passed on to `ResponseHistoryMean` to compute the mean history calculated as a function of time and will be stored in a csv file.

Implementation of `ResponseHistoryMean` is presented below. Further information on the theory of `ResponseHistoryMean` can be found in [theory manual](manuals/include/outputs/responsehistorymean-theory.md). For detailed description on the usage of `ResponseHistoryMean`, refer to [user manual](manuals/include/outputs/responsehistorymean-user.md).

!listing test/tests/vectorpostprocessors/response_history_mean/response_history_mean.i block=VectorPostprocessors

!syntax parameters /VectorPostprocessors/ResponseHistoryMean

!syntax inputs /VectorPostprocessors/ResponseHistoryMean

!syntax children /VectorPostprocessors/ResponseHistoryMean
