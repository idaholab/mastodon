# Baseline Correction

`BaselineCorrection` is a [`Function`](syntax/Functions/index.md) object that applies a [!ac](BLC) to an acceleration time history, $\ddot{u}(t)$, and outputs the adjusted values. The corrections are performed by first computing the nominal (*uncorrected*) velocity, $\dot{u}$, and displacement, $u$, by the [Newmark-beta method](manuals/theory/index.md#time-integration). An $n$-th order polynomial approximation of the acceleration, velocity, and/or displacement is then found by the method of least squares and each are successively subtracted from the nominal acceleration to obtain the corrected one, $\ddot{u}^{\ast}$. That is, the corrected acceleration time history is given by the following:

!equation id=corrected-accel
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}_{A}(t) - \ddot{P}_{V}(t) - \ddot{P}_{D}(t)

Here, $P_{A}$, $P_{V}$, and $P_{D}$ are the least squares approximations of the acceleration, velocity, and displacement time histories, respectively, and the overhead dots represent their derivative order. Finally, after $\ddot{u}^{\ast}(t)$ has been computed in accordance with [corrected-accel], it is linearly interpolated with respect to the real simulation time to ensure accurate results. Optionally, a scale factor may be applied to the final output. The [#theory] section will explain how the corrections are made in greater detail.

!alert note title=[Correction Combinations](#scheme)
It is possible to apply corrections using a best-fit of only one or any combination of the three kinematic variables, for which, any unused polynomials, $\ddot{P}$, $\dot{P}$, or $P$, would result in their values being zero in [corrected-accel]. Please see the [#usage] section for more information about how to control the adjustments.

## Numerical Formulation id=theory

This method for [!ac](BLC) of acceleration time histories was first presented by [!citet](brady1966) and later expanded upon by [!citet](pan2016)<!--eventually cite reference to future journal paper from Chris Wong-->. To begin understanding the MASTODON implementation, consider a given acceleration, $\ddot{u}$, specified over $N$ time intervals. The nominal velocity, $\dot{u}$, and the nominal displacement, $u$, are obtained by integrating with the [Newmark-beta method](manuals/theory/index.md#time-integration):

!equation id=newmark-gamma
\dot{u}_{i+1} = \dot{u}_{i} + [(1 - \gamma) \Delta t] \ddot{u}_{i} + (\gamma \Delta t) \ddot{u}_{i} \, , \, \, \, \forall \, i = 0, 1, 2, ..., N - 2

!equation id=newmark-beta
u_{i+1} = u_{i} + (\Delta t) \dot{u}_{i} + [(0.5 - \beta) (\Delta t)^{2}] \ddot{u}_{i} + [\beta (\Delta t)^{2}] \ddot{u}_{i+1}

When computing [newmark-gamma] and [newmark-beta], it is assumed that $\dot{u}_{0} = 0$ and that $u_{0} = 0$. The initial nominal acceleration, $\ddot{u}_0$, may be any arbitrary value.

A least squares approximation is evaluated by minimizing the area bounded by the square difference between the nominal record, e.g., $\ddot{u}(t)$, and the best-fit, $n$-th order, polynomial itself, e.g., $\ddot{P}_{A}(t)$. For example, a best-fit polynomial of $\ddot{u}$ can be found by solving the following equation:

!equation id=least-squares
\dfrac{\partial}{\partial C_{k}} \int_{0}^{T} [\ddot{u}(t) - \ddot{P}_{A}(t)]^{2} \, dt = 0 \, , \, \, \, \forall \, k = 0, 1, ..., n

Here, $T$ is the length of the record, i.e., the value corresponding to the final time-step, $i = N - 1$, and $C_{k}$ are the $(n + 1)$ unknown coefficients of the polynomial, $\ddot{P}_{A}$. Rearranging [least-squares] and evaluating wherever immediately possible leads to the following $(n + 1) \times (n + 1)$ system of equations to solve for the unknown polynomial coefficients:

!equation id=linear-normal
\sum_{j = 0}^{n} \dfrac{\left(j^{2} + 3 j + 2 \right) T^{k + j + 1}}{k + j + 1} C_{j} = \int_{0}^{T} t^{k} \ddot{u}(t) \, dt

[linear-normal] could be written in matrix notation, i.e.,

!equation id=linear-normal-system
\mathbf{K} C = I

In terms of Linear Algebra jargon, [linear-normal], or [linear-normal-system], is referred to as the linear +normal equation+. The definite integrals on the right-hand side of [linear-normal] is evaluated with a modified form of Newmark's method, i.e.,

!equation id=newmark-definite
I = \int_{0}^{T} t^{k} \ddot{u}(t) \, dt = \sum_{i = 0}^{N - 1} [(1 - \gamma) \Delta t] t_{i}^{k} \ddot{u}_{i} + (\gamma \Delta t) t_{i + 1}^{k} \ddot{u}_{i + 1}

Once all components in $\mathbf{K}$ and in $I$ are known, any method for solving a dense, nonsymmetric, square system of equations can be applied to solve [linear-normal-system].
The MASTODON implementation uses [libMesh's `lu_solve()`](https://libmesh.github.io/doxygen/classlibMesh_1_1DenseMatrix.html) method. Note that, the solution tends to become computationally unstable for $n \ge 10$.

The complete set of equations used to construct a linear normal equation to solve for the coefficients of all three polynomial approximation types, $P_{A}$, $P_{V}$, and $P_{D}$, are given in [formulas]. These formulas were derived according to the same logic used for [least-squares], [linear-normal], and [newmark-definite]. The values for the corrected time histories, $\ddot{u}^{\ast}(t)$, $\dot{u}^{\ast}(t)$, and $u^{\ast}(t)$, in the three "Least Squares Approximation" equations depend on which combination of best-fit curves are applied to the correction (see the [#scheme] section).

!table id=formulas
       caption=Complete set of equations for constructing linear normal equations for the three correction types.
| Best-fit\\ Polynomial | Least Squares\\ Approximation | $\mathbf{K}$ | $\bm{I}$ |
| - | - | - | - |
| $P_{A}$ | $\small \displaystyle \dfrac{\partial}{\partial C_{k}} \int_{0}^{T} [\ddot{u}^{\ast}(t)]^{2} \, dt = 0$ | $\small \displaystyle \sum_{j = 0}^{n} \dfrac{\left(j^{2} + 3 j + 2 \right) T^{k + j + 1}}{k + j + 1}$ | $\small \displaystyle I = \sum_{i = 0}^{N - 2} [(1 - \gamma) \Delta t] t_{i}^{k} \ddot{u}_{i} + (\gamma \Delta t) t_{i + 1}^{k} \ddot{u}_{i + 1}$ |
| $P_{V}$ | $\small \displaystyle \dfrac{\partial}{\partial C_{k}} \int_{0}^{T} [\dot{u}^{\ast}(t)]^{2} \, dt = 0$ | $\small \displaystyle \sum_{j = 0}^{n} \dfrac{\left(j + 2 \right) T^{k + j + 3}}{k + j + 3}$ | $\small \displaystyle I = \sum_{i = 0}^{N - 2} (\Delta t) t_{i}^{k + 1} \dot{u}_{i} + \newline \left[ \left(\dfrac{1}{2} - \beta \right) (\Delta t)^{2} \right] \left(t_{i}^{k + 1} \ddot{u}_{i} + (k + 1) t_{i}^{k} \dot{u}_{i} \right) + \newline \left[\beta (\Delta t)^{2} \right] \left(t_{i + 1}^{k + 1} \ddot{u}_{i + 1} + (k + 1) t_{i + 1}^{k} \dot{u}_{i + 1} \right)$ |
| $P_{D}$ | $\small \displaystyle \dfrac{\partial}{\partial C_{k}} \int_{0}^{T} [u^{\ast}(t)]^{2} \, dt = 0$ | $\small \displaystyle \sum_{j = 0}^{n} \dfrac{T^{k + j + 5}}{k + j + 5}$ | $\small \displaystyle I = \dfrac{1}{2} \sum_{i = 0}^{N - 2} (\Delta t) \left(t_{i + 1}^{k + 2} u_{i + 1} + t_{i}^{k + 2} u_{i} \right)$ |

### Correction Scheme id=scheme

Once the polynomial coefficients, $C_{k} \, \forall \, k = 0, 1, ..., n$, have been determined from [linear-normal-system] by substituting the necessary formulae given in [formulas], corrections are applied to all three kinematic variables in the following, general, way:

!equation id=all-corrections
\ddot{u}^{\ast}(t) = \ddot{u}(t) - \ddot{P}(t) = \ddot{u}(t) - \sum_{k = 0}^{n} (k^{2} + 3 k + 2) C_{k} t^{k}
\newline \, \newline
\dot{u}^{\ast}(t) = \dot{u}(t) - \dot{P}(t) = \dot{u}(t) - \sum_{k = 0}^{n} (k + 2) C_{k} t^{k + 1}
\newline \, \newline
u^{\ast}(t) = u(t) - P(t) = u(t) - \sum_{k = 0}^{n} C_{k} t^{k+2}

The values for $\ddot{P}$, $\dot{P}$, and $P$ in [all-corrections], depend on the combination of best-fit curves used. [correction-combos] presents all possible combinations of the best-fit polynomials whose sum equal $P(t)$. Each polynomial approximation, $P_{A}$, $P_{V}$, and $P_{D}$, takes on the same form given by the rightmost terms in [all-corrections], but the coefficients will be different for each. It is also important to note that the corrections always occur in a specific sequence and accumulate as each correction type is applied (see the [#example] section to understand what this means).

!table id=correction-combos
       caption=List of all possible combinations of the three correction types, along with their values for $P(t)$ for use in [all-corrections]. Here, +A+ denotes the acceleration correction, +V+ denotes the velocity correction, and +D+ denotes the displacement correction.
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

The `BaselineCorrection` object requires, as input, some sort of record of an acceleration time history. This may be provided by specifying the `data_file` parameter, as demonstrated in [example-1], or, directly, by specifying the `time_values` and `acceleration_values` parameters, as demonstrated in [example-2].

!listing baselinecorrection/twenty_sines.i
         block=Functions
         id=example-1
         caption=Sample input syntax for `BaselineCorrection` with the `data_file` parameter used to input a twenty-cycle sinusoidal acceleration time history.

!listing baselinecorrection/half_sine.i
         block=Functions
         id=example-2
         caption=Sample input syntax for `BaselineCorrection` with the `time_values` and `acceleration_values` parameters used to input a half-sine acceleration time history.

The `beta` and `gamma` parameters are those used for Newmark Integration. Ideally, they should correspond to the values used elsewhere throughout the input file. The `accel_fit_order`, `vel_fit_order`, and `disp_fit_order` variables control whether or not to use the polynomials present in [corrected-accel] when formulating the corrected time histories. If a value is specified, then a polynomial of that type and order will be found. Note that these are the parameters that define which of the combinations, described in [correction-combos], is used. For example, the set of inputs shown in [example-1] produces combination +AVD+, while those shown in [example-2] produces combination +A+.

The order of the approximating polynomials, $n$, which are set by the `accel_fit_order`, `vel_fit_order`, or `disp_fit_order` parameters, can be assigned a maximum integer value of nine, since the computations typically become unstable when attempting to find polynomials of orders greater than nine. The corrections are highly sensitive to the approximation order, and so it is recommended that a user run with several trial values until the desired result is achieved. In general, the order should be less than that which is required to produce a true best-fit curve of $\ddot{u}(t)$. For example, in [example-2], a half-sine wave was input. Since a half-sine wave could be, nearly, perfectly approximated by a certain quadratic function, the order for the corrections was set to one. However, for higher order inputs, such as the 20-cycle sine wave used in [example-2], finding the right correction configuration becomes tricky, and is a matter of user-preference. The [#demo] section provides several examples that should clarify why it is important to choose a suitable polynomial order.

!alert tip title=Use the Fewest Polynomials with the Lowest Orders Possible
When it is unclear which combination, and what approximation order, is the most suitable one, start by applying the simplest one: Combination +A+ with the order of $P_{A}$ being zero (constant). Then, incrementally increase the strength of the correction until a satisfactory result is achieved. The strongest correction possible is combination +AVD+ with all three, $P_{A}$, $P_{V}$, and $P_{D}$, being ninth-order. It is important to try to avoid an *overcorrection*!

### Apply a Corrected Acceleration as a Boundary Condition id=apply-bc

<!-- reference action system here when/if it becomes available-->

The corrected acceleration output from the `BaselineCorrection` object can be passed to [`PresetAcceleration`](source/bcs/PresetAcceleration.md) to be used as a dynamic excitation on a mesh boundary. However, the initial corrected acceleration may not be zero-valued, and by default, MOOSE assumes that $\ddot{u}(0) = 0$ for a given instance of `NewmarkAccelAux`. Thus, to set the correct initial condition, i.e., $\ddot{u}(0) = \ddot{u}^{\ast}(0)$, so that integration is properly handled by the boundary condition object, a user may invoke the `FunctionIC` object, as demonstrated in [example-3].

!listing baselinecorrection/twenty_sines_with_bc.i
         id=example-3
         caption=Sample input file that supplies a `BaselineCorrection` object for a `PresetAcceleration` object's `'function'` parameter (see the [#demo-5] section for further details).

The input file shown in [example-3] is an extension of [example-1] that solves a simple structural dynamics problem. This problem is described in the [#demo-5] section.

!alert tip title=Free Vibration Periods
When an acceleration boundary condition suddenly comes to a half at time $T$, i.e., $\ddot{u}(T) = 0$, but $\dot{u}(T) \ne 0$ and $u(T) \ne 0$, this can cause displacement drifts during the subsequent free vibration period, $t \ge T$, even if the acceleration were baseline corrected. To circumvent this behavior, consider using the [`TimePeriod`](source/controls/TimePeriod.md) object to activate a [`DirichletBC`](source/bcs/DirichletBC.md) object that controls $u(t), \, \forall \, t \ge T$.

## Demonstrations id=demo

The section presents several different cases of inputs and discusses certain properties of the adjustments that MASTODON's `BaselineCorrection` object performs on those inputs. The purpose of [!ac](BLC), and how it is applied to the [!ac](FEM), will be explained.

### Correction of a Half-sine id=demo-1

Consider the half-sine acceleration whose data points were provided as input for the `BaselineCorrection` object in [example-2] and which is plotted in [demo-1]. This acceleration can be defined, analytically, by the following function:

!equation id=basic-sine
\ddot{u}(t) = \pi^{2} \sin(\pi t), \, \, \, \forall \, t\in[0, 1]

!plot scatter filename=baselinecorrection_data/demo_1.csv
              data=[{'x':'time', 'y':'acceleration', 'name':'Nominal Acceleration', 'mode':'lines'}]
              layout={'xaxis':{'title':'Time'}, 'yaxis':{'title':'Acceleration'}, 'title':'Nominal Acceleration Time History'}
              id=demo-1
              caption=Plot of a half-cycle sinusoidal acceleration time history to be adjusted by the `BaselineCorrection` object.

Integrating [basic-sine] twice leads to the following functions for the velocity and displacement:

!equation id=basic-sine-ints
\dot{u}(t) = -\pi \cos(\pi t) + C_{0}
\newline \, \newline
u(t) = -sin(\pi t) + C_{0} t + C_{1}

In MOOSE, and in many computer applications that provide dynamic analysis capabilities, the following [!ac](IVP) is often the default:

!equation id=ivp
\dot{u}(0) = 0, u(0) = 0

[basic-sine-ints] is merely the *general solution* for [basic-sine].
A *particular solution* is found by enforcing the [!ac](IVP) of [ivp] and solving for the constants of integration, $C_{0}$, and $C_{1}$, i.e.,

!equation id=particular
\dot{u}(t) = -\pi \cos(\pi t) + \pi
\newline \, \newline
u(t) = -sin(\pi t) + \pi t

It is clear that, by the assumption of [ivp], the nominal displacement time history is a superposition of a monotonic sine wave and a linear function. Thus, while the body oscillates, it will also continuously drift at a rate of $\pi t$. To demonstrate this behavior, the particular solution for $u(t)$, given by [particular], is shown in [demo-1-drift].

!plot scatter filename=baselinecorrection_data/demo_1_drift.csv
              data=[{'x':'time', 'y':'displacement', 'name':'Nominal Displacement', 'mode':'lines'}]
              layout={'xaxis':{'title':'Time'},
                      'yaxis':{'title':'Displacement'},
                      'title':'Nominal Displacement Time History'}
              id=demo-1-drift
              caption=Plot of the displacement time history resulting from integration of the acceleration shown in [demo-1]. The values exhibit a drifting behavior, i.e., they are steadily increasing over time.

To be sure, [particular] is a *real* solution, however, it is not appropriate for evaluating the response of solids to an oscillatory excitation, such as earthquake ground motion. A particular solution to [basic-sine-ints] that is more characteristic of vibratory motion could be found. For example, by assuming that the [!ac](IVP) is given by $\dot{u}(t) = -\pi$ and $u(t) = 0$, then the displacement time history would simply be $u(t) = -sin(\pi t)$, which is also a real solution.

It is usually not so easy, but, rather, impossible to find an [!ac](IVP) that is compatible with vibratory motion. Especially for the case of acceleration data obtained from field measurements (see the [#demo-6] section). [!ac](BLC) is a common signal processing technique whose purpose is to remove an arbitrary set of functions superposed within the raw data, which are the result of initial conditions that are inconsistent with reality, while still satisfying the assumptions of [ivp].
The least squares approach to [!ac](BLC) assumes that those functions to be removed are best-fit polynomials (see the [#theory] section).

MASTODON's `BaselineCorrection` object corrected the acceleration defined by [basic-sine] and the corresponding displacement time history is shown in [demo-1-adjusted] - compare this to [demo-1-drift]. Also, note that the shape of this plot closely resembles that of the particular solution, $u(t) = -sin(\pi t)$.

!plot scatter filename=baselinecorrection_data/demo_1_adjusted.csv
              data=[{'x':'time', 'y':'displacement', 'name':'Corrected Displacement', 'mode':'lines'}]
              layout={'xaxis':{'title':'Time'},
                      'yaxis':{'title':'Displacement'},
                      'title':'Corrected Displacement Time History'}
              id=demo-1-adjusted
              caption=Plot of the displacement time history resulting from integration of the baseline corrected values of the acceleration shown in [demo-1]. These values are more characteristic of sinusoidal motion than the nominal ones shown in [demo-1-drift].

### Correction of a Half-sine with Scaling id=demo-2

Sometimes, it may be desirable to achieve a target displacement amplitude. If the `BaselineCorrection` object fails to achieve such a value, a scaling factor can be applied by providing the `scale_factor` parameter. For example, consider the corrected displacement shown in [demo-1-adjusted] whose amplitude is approximately $-0.2047$ and assume that the target amplitude is $-1.0$. For this case, the following scale factor may be applied:

!equation
\textrm{scale} = \frac{\textrm{desired}}{\textrm{result}} = \frac{-1.0}{-0.2047} = 4.886

This value can be input for the `BaselineCorrection` object, as demonstrated in [demo-2-input].

!listing baselinecorrection/half_sine_scaled.i
         block=Functions
         id=demo-2-input
         caption=Sample input syntax that is similar to [example-2], but provides a value for the `scale_factor` parameter.

Now, the corrected displacement time history would resemble that shown in [demo-2-adjusted].

!plot scatter filename=baselinecorrection_data/demo_2_adjusted.csv
              data=[{'x':'time', 'y':'displacement', 'name':'Corrected Displacement', 'mode':'lines'}]
              layout={'xaxis':{'title':'Time'},
                      'yaxis':{'title':'Displacement'},
                      'title':'Corrected and Scaled Displacement Time History'}
              id=demo-2-adjusted
              caption=Plot of the displacement time history resulting from integration of the baseline corrected and scaled values of the acceleration shown in [demo-1].

### Correction of a Full-sine id=demo-3

Consider the acceleration defined by [basic-sine], but over a full-sine interval, i.e., with $T = 2$. The input syntax for use of the `BaselineCorrection` object with this acceleration time history is given in [demo-3-input].

!listing baselinecorrection/full_sine.i
         block=Functions
         id=demo-3-input
         caption=Sample input syntax for applying the `BaselineCorrection` object to a full-sine acceleration time history.

Notice that the polynomial order used for the correction of a full-sine wave, shown in [demo-3-input], is higher than that used for the correction of a half-sine wave, shown in [example-2] or [demo-2-input]. MASTODON's `BaselineCorrection` object produced a corrected acceleration that corresponds with the displacement time history shown in [demo-3-adjusted].

!plot scatter filename=baselinecorrection_data/demo_3_adjusted.csv
              data=[{'x':'time', 'y':'displacement', 'name':'Corrected Displacement'}]
              layout={'xaxis':{'title':'Time'},
                      'yaxis':{'title':'Displacement'},
                      'title':'Corrected Displacement Time History'}
              id=demo-3-adjusted
              caption=Plot of the displacement time history resulting from integration of the baseline corrected values of the acceleration shown in [demo-1] extrapolated out to $t = 2$.

### Correction of a Twenty-sine id=demo-4

Consider the half-sine acceleration whose data points were provided as input for the `BaselineCorrection` object in [example-1] and which is plotted in [demo-4]. This acceleration can be defined, analytically, by the following function:

!equation id=twenty-sine
\ddot{u}(t) = -250 \pi^{2} sin(50 \pi t), \, \, \, \forall \, t\in[0, 0.8]

!plot scatter filename=baselinecorrection/twenty_sines_accel.csv
              data=[{'x':'time', 'y':'acceleration', 'name':'Nominal Acceleration'}]
              layout={'xaxis':{'title':'Time'}, 'yaxis':{'title':'Acceleration'}, 'title':'Nominal Acceleration Time History'}
              id=demo-4
              caption=Plot of a twenty-cycle sinusoidal acceleration time history to be adjusted by the `BaselineCorrection` object.

When the nominal acceleration is of high order and uniformity, a stronger correction is required. Thus, for the nominal time history given by [twenty-sine], combination +AVD+ was used with high-order polynomials for each type. MASTODON's `BaselineCorrection` object produced a corrected acceleration that corresponds with the displacement time history shown in [demo-4-adjusted].

!plot scatter filename=baselinecorrection_data/demo_4_adjusted.csv
              data=[{'x':'time', 'y':'displacement', 'name':'Corrected Displacement'}]
              layout={'xaxis':{'title':'Time'},
                      'yaxis':{'title':'Displacement'},
                      'title':'Corrected Displacement Time History'}
              id=demo-4-adjusted
              caption=Plot of the displacement time history resulting from integration of the baseline corrected values of the acceleration shown in [demo-4].

### Apply a Corrected Twenty-sine as a Boundary Condition id=demo-5

[!ac](BLC) is especially necessary for the analysis of structures subject to external, harmonic accelerations, because, without it, the displacement solution can be unrealistic, or even unachievable, depending on the model details. To demonstrate this point, a simple solid dynamics problem, whose boundary conditions consists of a sinusoidal acceleration, shall be solved using the [!ac](FEM) with, and without, [!ac](BLC) and the two solutions shall be compared. It will be shown that the solution without [!ac](BLC) is not merely unrealistic, but outright absurd.

Consider a $1 \, \textrm{m} \times 1 \, \textrm{m} \times 1 \, \textrm{m}$ block meshed with a single hexahedral element. The bottom face of the block ($z = 0$) is restrained in all directions and the acceleration time history given by [twenty-sine] is applied to the top face ($z = 1 \, \textrm{m}$) in the $x$-direction. The unit value of the acceleration is $\textrm{m} / \textrm{s}^{2}$. The block is constrained in the $x$-direction and $y$-direction such that deformations are caused by shear strains only (see the [source/actions/AddPeriodicBCAction.md] page). The goal is to determine the displacement, in the $x$-direction, at the top face of the block when the applied acceleration is a baseline corrected one and when it is not. For this problem, the material properties of the block are trivial, but it was assigned those of intact structural steel.

The input file used for this problem's baseline corrected case was given in [example-3].
For the nominal case, the acceleration was applied directly by passing the output from a [`PiecewiseLinear`](source/functions/PiecewiseLinear.md) object to `PresetAcceleration`, as opposed to that from a `BaselineCorrection` one. That is, the `[ICs]` block in [example-3] was removed, and the `[Functions]` and `[BCs]` blocks were replaced with the following:

```
[Functions]
  [nominal_accel_func]
    type = PiecewiseLinear
    data_file = twenty_sines_accel.csv
    format = columns
  []
[]

[BCs]
  #
  # <DirichletBC objects to restrain the bottom face>
  #
  [apply_nominal_accel]
    type = PresetAcceleration
    variable = disp_x
    velocity = vel_x
    acceleration = accel_x
    boundary = 5
    function = nominal_accel_func
  []
[]
```

A graphical rendering of each of the two solutions are shown, simultaneously, in [demo-5]. The contour values represent the magnitude of the displacement field at a given point. The displacement time history at the top face of the block for the corrected case matches that shown in [demo-4-adjusted], while that for the nominal case exhibits drifting behavior.

!media baselinecorrection/demo_5.gif
       style=width:100%;margin-left:auto;margin-right:auto;
       id=demo-5
       caption=Renderings of solutions resulting from two different boundary value problems: one where the acceleration applied to the top of the block is baseline corrected, and another where it is not. The transparent block, which appears to vibrate in shear, represents the former case and the opaque block, which appears to stretch along the negative $x$-direction, represents the latter.

If the expected result was that the block would vibrate about its centroid, then, by comparing those renderings of the two cases shown in [demo-5], it becomes clear that the model must use a corrected acceleration time history. Otherwise, the top face of the block continuously drifts relative to its bottom face, causing it to stretch indefinitely. Aside from this kind of behavior's appearance being silly, it could also lead to bogus results, numerical errors, and/or convergence issues when it occurs in more complicated models. In a slightly different scenario - one where the nominal acceleration was applied to the block as a ground excitation (i.e., applied to the bottom face, while the top face was left unrestrained) - the whole block would continuously drift in one direction, which is something that structures normally don't do during earthquakes.

### Correction of a Raw Accelerogram id=demo-6

Records of raw accelerograms from the September 20, 1999 Chi-Chi earthquake were obtained from the [Strong-Motion Virtual Data Center (VDC)](https://strongmotioncenter.org/vdc/scripts/event.plx?evt=629) from two stations: [TAP007](https://strongmotioncenter.org/vdc/scripts/stnpage.plx?stations=1893) and [TCU071](https://strongmotioncenter.org/vdc/scripts/stnpage.plx?stations=2252). The accelerograms from the two station's recordings of the Chi-Chi event are plotted in [demo-6-1] and [demo-6-2], respectively.

!plot scatter filename=baselinecorrection/chichi_A0126300_N.csv
              data=[{'x':'time', 'y':'acceleration', 'name':'Nominal Acceleration'}]
              layout={'xaxis':{'title':'Time (s)'}, 'yaxis':{'title':'Acceleration (cm/s/s)'}, 'title':'Nominal Acceleration Time History'}
              id=demo-6-1
              caption=Raw accelerogram (Chi-Chi earthquake) from the TAP007 station in Tapei, Taiwan to be adjusted by the `BaselineCorrection` object.

!plot scatter filename=baselinecorrection/chichi_QT063000_V.csv
              data=[{'x':'time', 'y':'acceleration', 'name':'Nominal Acceleration'}]
              layout={'xaxis':{'title':'Time (s)'}, 'yaxis':{'title':'Acceleration (cm/s/s)'}, 'title':'Nominal Acceleration Time History'}
              id=demo-6-2
              caption=Raw accelerogram (Chi-Chi earthquake) from the TCU071 station in Taichung, Taiwan to be adjusted by the `BaselineCorrection` object.

The input syntax for use of the `BaselineCorrection` object with these acceleration time histories are given in [demo-6-1-input] and [demo-6-2-input], respectively.

!listing baselinecorrection/chichi_A0126300_N.i
         block=Functions
         id=demo-6-1-input
         caption=Sample input syntax for applying the `BaselineCorrection` object to the acceleration time history shown in [demo-6-1].

!listing baselinecorrection/chichi_QT063000_V.i
        block=Functions
        id=demo-6-2-input
        caption=Sample input syntax for applying the `BaselineCorrection` object to the acceleration time history shown in [demo-6-2].

As mentioned in the [#usage] section, when an appropriate correction configuration is unclear, it is best to start with the simplest one, and increase the strength of the correction until the results are reasonable and satisfactory. Such was the approach used for selecting the inputs shown in [demo-6-1-input] and [demo-6-2-input]. [demo-6-1-comparison] and [demo-6-2-comparison] compare the nominal and corrected displacement time histories that correspond with the TAP007 and the TCU071 accelerograms, respectively.

!plot scatter filename=baselinecorrection_data/demo_6_A0126300_N.csv
              data=[{'x':'time', 'y':'nominal_disp', 'name':'Nominal Displacement'},
                    {'x':'time', 'y':'corrected_disp', 'name':'Corrected Displacement'}]
              layout={'xaxis':{'title':'Time (s)'}, 'yaxis':{'title':'Displacement (cm)'}, 'title':'Nominal vs. Corrected Displacement Time History'}
              id=demo-6-1-comparison
              caption=Comparison of the displacement time histories resulting from: 1) integration of the nominal values and 2) integration of the baseline corrected values of the acceleration shown in [demo-6-1].

!plot scatter filename=baselinecorrection_data/demo_6_QT063000_V.csv
              data=[{'x':'time', 'y':'nominal_disp', 'name':'Nominal Displacement'},
                    {'x':'time', 'y':'corrected_disp', 'name':'Corrected Displacement'}]
              layout={'xaxis':{'title':'Time (s)'}, 'yaxis':{'title':'Displacement (cm)'}, 'title':'Nominal vs. Corrected Displacement Time History'}
              id=demo-6-2-comparison
              caption=Comparison of the displacement time histories resulting from: 1) integration of the nominal values and 2) integration of the baseline corrected values of the acceleration shown in [demo-6-2].

The corrected displacements shown in [demo-6-1-comparison] or [demo-6-2-comparison] are more characteristic of earthquake ground motions than their nominal counterparts. Typically, ground motions will oscillate about the initial (baseline) position, and come to rest at that position as well. Permanent ground displacements can occur from earthquakes, but, surely, a continuous drifting motion would still seem strange.

!syntax parameters /Functions/BaselineCorrection

!syntax inputs /Functions/BaselineCorrection

!syntax children /Functions/BaselineCorrection

!!!
The `complete` setting for the `!acronym list` command is currently broken in `moose/python/MooseDocs/extensions/acronym.py`, but is easily fixable - the invocation of `getAcronyms()` on line 157 is passing `complete=True` by default.

TODO: Add the following when it gets fixed:

## List of Acronyms

!acronym list complete=False
!!!
