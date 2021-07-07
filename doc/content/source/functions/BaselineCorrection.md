# Baseline Correction

`BaselineCorrection` is a [`Function`](syntax/Functions/index.md) object that applies a [!ac](BLC) to an acceleration time history, $\ddot{u}(t)$. The corrections are performed by first computing the nominal (*uncorrected*) velocity, $\dot{u}(t)$, and displacement, $u(t)$, by the [Newmark-beta method](manuals/theory/index.md#time-integration). An $n$-th order polynomial approximation of the acceleration, velocity, and/or displacement is then found by the method of least squares and each are successively subtracted from the nominal acceleration to obtain the corrected one, $\ddot{u}^{\ast}$. That is, the corrected acceleration time history is given by the following:

!equation id=corrected-accel
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}_{A}(t) - \ddot{P}_{V}(t) - \ddot{P}_{D}(t) \, , \, \, \, \forall \, t \in \mathbb{R}

where $P_{A}$, $P_{V}$, and $P_{D}$ are the least squares approximations of the acceleration, velocity, and displacement, respectively, and the overhead dots represent their derivative order.

The corrected velocity, $\dot{u}^{\ast}$, and the corrected displacement, $u^{\ast}$, are obtained by integrating [corrected-accel]. The output for this function object is, by default, the corrected acceleration, $\ddot{u}^{\ast}$. However, a user may request that the output be $\dot{u}^{\ast}$ or $u^{\ast}$, as well. Finally, after one of the three kinematic variables has been adjusted, the corrected ordinates are linearly interpolated with respect to simulation time to ensure accurate results at any given real-valued time instance. Optionally, a scale factor may be applied to the final output.

!alert note title=[Correction Combinations](#scheme)
It is possible to apply corrections using a best-fit of only one or any combination of the three kinematic variables, for which, any unused polynomials, $\ddot{P}_{A}$, $\ddot{P}_{V}$, or $\ddot{P}_{D}$, would result in their values being zero in [corrected-accel]. Please see the [#scheme] section for more information about how the adjustments are made.

## Numerical Formulation id=theory

This method for [!ac](BLC) of acceleration time histories was first presented by [!citet](brady1966) and later expanded upon by [!citet](pan2016)<!--eventually cite reference to future journal paper from Chris Wong-->. To begin understanding the MASTODON implementation, consider a given acceleration, $\ddot{u}$, specified at $N$ discrete time instances. The nominal velocity, $\dot{u}$, and the nominal displacement, $u$, are obtained by integrating with the [Newmark-beta method](manuals/theory/index.md#time-integration):

!equation id=newmark-gamma
\dot{u}_{i+1} = \dot{u}_{i} + [(1 - \gamma) \Delta t] \ddot{u}_{i} + (\gamma \Delta t) \ddot{u}_{i} \, , \, \, \, \forall \, i = 0, 1, 2, ..., N - 2

!equation id=newmark-beta
u_{i+1} = u_{i} + (\Delta t) \dot{u}_{i} + [(0.5 - \beta) (\Delta t)^{2}] \ddot{u}_{i} + [\beta (\Delta t)^{2}] \ddot{u}_{i+1}

Here, the size of a time increment, $\Delta t$, will depend on the acceleration record, and could vary between time steps, in which case, $\Delta t_{i} = t_{i + 1} - t_{i}$. When evaluating [newmark-gamma] and [newmark-beta], it is assumed that $\dot{u}_{0} = 0$ and that $u_{0} = 0$. The initial nominal acceleration, $\ddot{u}_0$, may be any arbitrary value.

A least squares approximation is found by minimizing the area bounded by the square difference between the nominal record and the best-fit, $n$-th order, polynomial itself. For example, a best-fit polynomial of $\ddot{u}$ can be determined by solving the following equation:

!equation id=least-squares
\dfrac{\partial}{\partial C_{k}} \int_{t_{0}}^{T} [\ddot{u}(t) - \ddot{P}_{A}(t)]^{2} \, dt = 0 \, , \, \, \, \forall \, k = 0, 1, ..., n

where $T = t_{N - 1}$ , i.e., the value corresponding to the final time-step, and $C_{k}$ are the $(n + 1)$ unknown coefficients of the polynomial, $\ddot{P}_{A}$. Rearranging [least-squares] and evaluating wherever immediately possible leads to the following $(n + 1) \times (n + 1)$ system of equations to solve for the unknown polynomial coefficients:

!equation id=linear-normal
\sum_{j = 0}^{n} \dfrac{\left(j^{2} + 3 j + 2 \right) T^{k + j + 1}}{k + j + 1} C_{j} = \int_{t_{0}}^{T} t^{k} \ddot{u}(t) \, dt

[linear-normal] could be written in matrix notation, i.e.,

!equation id=linear-normal-system
\mathbf{K} C = I

In terms of Linear Algebra jargon, [linear-normal], or [linear-normal-system], is referred to as the linear +normal equation+. The definite integrals on the right-hand side of [linear-normal] are evaluated with a modified form of Newmark's method, i.e.,

!equation id=newmark-definite
I = \int_{t_{0}}^{T} t^{k} \ddot{u}(t) \, dt = \sum_{i = 0}^{N - 1} [(1 - \gamma) \Delta t] t_{i}^{k} \ddot{u}_{i} + (\gamma \Delta t) t_{i + 1}^{k} \ddot{u}_{i + 1}

Once all components in $\mathbf{K}$ and in $I$ are known, any method for solving a dense, nonsymmetric, nonhomogenous, square system of equations can be applied to solve [linear-normal-system] for the unknown vector of fitting coefficients, $C = \begin{Bmatrix} C_{0} && C_{1} && \cdot \cdot \cdot && C_{n} \end{Bmatrix}^{\intercal}$. The MASTODON implementation uses [libMesh's `lu_solve()`](https://libmesh.github.io/doxygen/classlibMesh_1_1DenseMatrix.html) method. Note that, the solution tends to become computationally unstable for $n \ge 10$, because $\mathbf{K}$ becomes ill-conditioned.

The equations presented so far are exclusive to finding the acceleration correction and were given for demonstration purposes. The procedure thus followed may also be applied to the velocity and displacement corrections as demonstrated in [!citet](pan2016). The complete set of equations used to construct a linear normal equation to solve for the coefficients of all three polynomial approximation types: $P_{A}$, $P_{V}$, and $P_{D}$, are given in [formulas]. The values for the corrected time histories, $\ddot{u}^{\ast}(t)$, $\dot{u}^{\ast}(t)$, and $u^{\ast}(t)$, in the three "Least Squares Approximation" equations depend on which combination of best-fit curves are applied to the correction (see the [#scheme] section).

!table id=formulas
       caption=Complete set of equations for constructing linear normal equations for the three correction types.
| Best-fit | Least Squares\\ Approximation | $\mathbf{K}$ | $\bm{I}$ |
| - | - | - | - |
| $P_{A}$ | $\small \displaystyle \dfrac{\partial}{\partial C_{k}} \int_{t_{0}}^{T} [\ddot{u}^{\ast}(t)]^{2} \, dt = 0$ | $\small \displaystyle \sum_{j = 0}^{n} \dfrac{\left(j^{2} + 3 j + 2 \right) T^{k + j + 1}}{k + j + 1}$ | $\small \displaystyle I = \sum_{i = 0}^{N - 2} [(1 - \gamma) \Delta t] t_{i}^{k} \ddot{u}_{i} + (\gamma \Delta t) t_{i + 1}^{k} \ddot{u}_{i + 1}$ |
| $P_{V}$ | $\small \displaystyle \dfrac{\partial}{\partial C_{k}} \int_{t_{0}}^{T} [\dot{u}^{\ast}(t)]^{2} \, dt = 0$ | $\small \displaystyle \sum_{j = 0}^{n} \dfrac{\left(j + 2 \right) T^{k + j + 3}}{k + j + 3}$ | $\small \displaystyle I = \sum_{i = 0}^{N - 2} (\Delta t) t_{i}^{k + 1} \dot{u}_{i} + \newline \left[ \left(\dfrac{1}{2} - \beta \right) (\Delta t)^{2} \right] \left(t_{i}^{k + 1} \ddot{u}_{i} + (k + 1) t_{i}^{k} \dot{u}_{i} \right) + \newline \left[\beta (\Delta t)^{2} \right] \left(t_{i + 1}^{k + 1} \ddot{u}_{i + 1} + (k + 1) t_{i + 1}^{k} \dot{u}_{i + 1} \right)$ |
| $P_{D}$ | $\small \displaystyle \dfrac{\partial}{\partial C_{k}} \int_{t_{0}}^{T} [u^{\ast}(t)]^{2} \, dt = 0$ | $\small \displaystyle \sum_{j = 0}^{n} \dfrac{T^{k + j + 5}}{k + j + 5}$ | $\small \displaystyle I = \dfrac{1}{2} \sum_{i = 0}^{N - 2} (\Delta t) \left(t_{i + 1}^{k + 2} u_{i + 1} + t_{i}^{k + 2} u_{i} \right)$ |

### Correction Scheme id=scheme

Once the polynomial coefficients, $C_{k} \, \forall \, k = 0, 1, ..., n$, have been determined from [linear-normal-system] by substituting the necessary formulae given in [formulas], corrections are applied to all three kinematic variables in the following, general, way:

!equation id=all-corrections
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}(t) = \ddot{u}(t) - \sum_{k = 0}^{n} (k^{2} + 3 k + 2) C_{k} t^{k}
\newline \, \newline
\dot{u}^{\ast}(t) = \dot{u}(t) - \dot{P}(t) = \dot{u}(t) - \sum_{k = 0}^{n} (k + 2) C_{k} t^{k + 1}
\newline \, \newline
u^{\ast}(t) = u(t) - P(t) = u(t) - \sum_{k = 0}^{n} C_{k} t^{k+2}

The values for $\ddot{P}$, $\dot{P}$, and $P$ in [all-corrections], depend on the combination of best-fit curves used. [correction-combos] presents all possible combinations of the best-fit polynomials, whose sum equal $P(t)$, and assigns them a shorthand identifier. Each polynomial approximation, $P_{A}$, $P_{V}$, and $P_{D}$, takes on the same form given by the rightmost terms in [all-corrections], but the coefficients may vary between combinations It is also important to note that the corrections always occur in a specific sequence and accumulate as each correction type is applied.

!table id=correction-combos
       caption=List of all possible combinations of the three correction types, along with their values for $P(t)$ for use in [all-corrections]. Here, +A+ denotes an acceleration correction, +V+ denotes a velocity one, and +D+ denotes a displacement one. Combination identifiers are assigned based on these notations.
| Combination | $\bm{P(t)}$ |
| - | - |
| +A+ | $P_{A}(t)$ |
| +AV+ | $P_{A}(t) - P_{V}(t)$ |
| +AD+ | $P_{A}(t) - P_{D}(t)$ |
| +AVD+ | $P_{A}(t) - P_{V}(t) - P_{D}(t)$ |
| +V+ | $P_{V}(t)$ |
| +VD+ | $P_{V}(t) - P_{D}(t)$ |
| +D+ | $P_{D}(t)$ |

#### Correction Procedure Example id=example

Consider combination +AVD+. For this case, MASTODON will apply [!ac](BLC) by the following procedure:

1. The corrected acceleration time history, which is substituted into the least squares approximation given in [formulas], is

!equation
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}_{A}(t)

2. Then, by substituting the polynomial coefficients into [all-corrections], the velocity and displacement time histories are adjusted:

!equation
\dot{u}^{\ast}(t) = \dot{u}(t) - \dot{P}_{A}(t)

!equation
u^{\ast}(t) = u(t) - P_{A}(t)

3. Before applying the velocity correction, the nominal time histories are updated:

!equation
\ddot{u}(t) = \ddot{u}^{\ast}(t), \, \, \, \dot{u}(t) = \dot{u}^{\ast}(t), \, \, \, u(t) = u^{\ast}(t)

*Now, the nominal time histories reflect the adjustments from the acceleration correction.*

4. Proceeding on with the velocity correction, the corrected velocity time history, which is substituted into the least squares approximation given in [formulas], is

!equation
\dot{u}^{\ast}(t) = \dot{u}(t) - \dot{P}_{V}(t) = \dot{u}(t) - \dot{P}_{A}(t) -\dot{P}_{V}(t)

5. Then, by substituting the polynomial coefficients into [all-corrections], the acceleration and displacement time histories are adjusted:

!equation
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}_{V}(t) = \ddot{u}(t) - \ddot{P}_{A}(t) - \ddot{P}_{V}(t)

!equation
u^{\ast}(t) = u(t) - P_{V}(t) = u(t) - P_{A}(t) - P_{V}(t)

6. Before applying the displacement correction, the nominal time histories are, once again, updated:

!equation
\ddot{u}(t) = \ddot{u}^{\ast}(t), \, \, \, \dot{u}(t) = \dot{u}^{\ast}(t), \, \, \, u(t) = u^{\ast}(t)

*Now, the nominal time histories reflect the adjustments from the acceleration correction and the velocity correction.*

7. Proceeding on with the displacement correction, the corrected displacement time history, which is substituted into the least squares approximation given in [formulas], is

!equation
u^{\ast}(t) = u(t) - P_{D}(t) = u(t) - P_{A}(t) - P_{V}(t) - P_{D}(t)

8. Finally, by substituting the polynomial coefficients into [all-corrections], the acceleration and velocity time histories are adjusted:

!equation
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}_{D}(t) = \ddot{u}(t) - \ddot{P}_{A}(t) - \ddot{P}_{V}(t) - \ddot{P}_{D}(t)

!equation
\dot{u}^{\ast}(t) = \dot{u}(t) - \dot{P}_{D}(t) = \dot{u}(t) - \dot{P}_{A}(t) -\dot{P}_{V}(t) - \dot{P}_{D}(t)

!alert note prefix=False title=Corrections are Applied Sequentially and Cumulatively
The corrections always occur in the sequence: +A+ [!icon!arrow_forward] +V+ [!icon!arrow_forward] +D+. They will also accumulate for the case of multiple corrections. For example, the displacement correction for combination +VD+ will be applied to the displacement time history adjusted by the velocity correction, while the displacement correction for combination +D+ will be applied to the nominal one.

## Usage id=usage

The `BaselineCorrection` object requires, as input, some sort of record of an acceleration time history. The data may be provided either by specifying the `time_values` and `acceleration_values` parameters in an abscissa-ordinate pair fashion, as demonstrated in [example-1], or by specifying the `data_file` parameter with the name of a file containing the time and acceleration pairs in a delimited format, such as comma-separated values (see the "DelimitedFileReader" section on the [source/utils/MooseUtils.md] page). Optionally, a user may specify the `time_name` and `acceleration_name` parameters and the file reader will search for a row or column where the first index contains a text string that matches the provided input, as demonstrated in [example-2]. If no input for `time_name` and `acceleration_name` is provided, then it is assumed that the abscissa are in the very first row or column and the ordinates are in the second.

!listing baselinecorrection/half_sine_scaled.i
         block=Functions
         id=example-1
         caption=Sample input syntax for `BaselineCorrection` with the `time_values` and `acceleration_values` parameters used to input a half-sine acceleration time history. The corrected displacement is multiplied by the factor provided for the `scale_factor` parameter before being returned.

!listing baselinecorrection/twenty_sines_with_bc.i
         block=Functions
         id=example-2
         caption=Sample input syntax for `BaselineCorrection` with the `data_file` parameter used to input a twenty-cycle sinusoidal acceleration time history. Note that if no input for the `time_name` and `acceleration_name` parameters is provided, it is assumed that time values are in the first row or column of the delimited file and accelerations are in the second.

The `series_type` parameter controls which kinematic variable is output. For example, since `series_type = displacement` was provided in [example-1], and since `scale_factor = 4.886` was also provided, the output will be $4.886 u^{\ast}$ interpolated through time. Whereas in [example-2], the output will be $\ddot{u}^{\ast}$ interpolated through time, since `series_type = acceleration` and `scale_factor = 1` are the defaults.

The `beta` and `gamma` parameters are those used for Newmark Integration. Ideally, they should correspond to the values used elsewhere throughout the input file. The `accel_fit_order`, `vel_fit_order`, and `disp_fit_order` variables control whether or not to use the polynomials, $P_{A}$, $P_{V}$, and $P_{D}$, when formulating the corrected time histories. That is, if a value is specified, then a polynomial of that type and order will be found. Control of these three parameters alone leads to the different combinations of corrections presented in [correction-combos]. For example, the set of inputs shown in [example-1] produces combination +A+, while those shown in [example-2] produces combination +AVD+.

The order of the approximating polynomials, $n$, which are set by the `accel_fit_order`, `vel_fit_order`, or `disp_fit_order` parameters, can be assigned a maximum integer value of nine, since [linear-normal-system] tends to become unstable when attempting to satisfy polynomials of larger order. The corrections are highly sensitive to the approximation order, and so it is recommended that a user run with several trial values until the desired result is achieved. For multiple cycles of certain transcendental or quasi-transcendental functions, finding the right correction can be difficult, if not impossible. The [examples/example11.md] page provides several examples that should clarify why 1) it is important to choose a suitable polynomial order, and 2) that such a choice is not necessarily a trivial one.

!alert tip title=Use the Fewest Polynomials with the Lowest Orders Possible
When it is unclear which combination, and what approximation order, is the most suitable one, start by applying the simplest one: Combination +A+ with the order of $P_{A}$ being zero (constant). Then, incrementally increase the strength of the correction until a satisfactory result is achieved. The strongest correction possible is combination +AVD+ with all three, $P_{A}$, $P_{V}$, and $P_{D}$, being ninth-order. It is important to try to avoid an *overcorrection*!

### Apply a Corrected Acceleration as a Boundary Condition id=apply-bc

<!-- reference action system here when/if it becomes available-->

The corrected acceleration output from the `BaselineCorrection` object can be passed to [`PresetAcceleration`](source/bcs/PresetAcceleration.md) to be used as a dynamic excitation on a mesh boundary. However, the initial corrected acceleration may not be zero-valued, and by default, MOOSE assumes that $\ddot{u}(0) = 0$ for a given instance of `NewmarkAccelAux`. Thus, to set the correct initial condition, i.e., $\ddot{u}(0) = \ddot{u}^{\ast}(0)$, so that integration is properly handled by the boundary condition object, a user may invoke the `FunctionIC` object, as demonstrated in [example-3].

!listing baselinecorrection/twenty_sines_with_bc.i
         id=example-3
         block=Functions ICs BCs
         caption=Sample input file that supplies a `BaselineCorrection` object for a `PresetAcceleration` object's `'function'` parameter (see the [examples/example11.md] page for further details).

!alert tip title=Free Vibration Periods
When an acceleration boundary condition suddenly comes to a half at time $T$, i.e., $\ddot{u}(T) = 0$, but $\dot{u}(T) \ne 0$ and $u(T) \ne 0$, this can cause displacement drifts during the subsequent free vibration period, $t \ge T$, even if the acceleration were baseline corrected. To circumvent this behavior, consider using the [`TimePeriod`](source/controls/TimePeriod.md) object to activate a [`DirichletBC`](source/bcs/DirichletBC.md) object that controls $u(t), \, \forall \, t \ge T$.

!syntax parameters /Functions/BaselineCorrection

!syntax inputs /Functions/BaselineCorrection

!syntax children /Functions/BaselineCorrection
