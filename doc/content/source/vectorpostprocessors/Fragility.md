# Fragility

!syntax description /VectorPostprocessors/Fragility

## Description

The `Fragility` VectorPostprocessor computes the seismic fragility of a component
given the dynamic properties of the component, capacity distribution of the component,
and seismic demands from the probabilistic simulations. `Fragility` operates
in the following steps: 1) calculate the demand distribution for each hazard bin, 2) use
demand distribution and capacity distribution and calculate the conditional probability of failure
in each bin, and 3) fit a lognormal distribution in the conditional probabilities to calculate a
fragility. This kind of fragility is also referred to as 'enhanced fragility'.

Fitting a lognormal fragility function to the probabilities of failure requires an optimization
algorithm to find the parameters, median and lognormal standard deviation. Two methods are
currently available for calculating these fragility parameters: 'brute force' and
'Stochastic Gradient Descent (SGD)'. Brute force finds that combination of parameters
 which maximizes the likelihood by looping over all alternative combinations. SGD finds
the parameter combination by running a Newton-Raphson type algorithm with random seed values.
SGD also requires the additional parameters:

1. tolerance (To declare convergence of the algorithm. 1e-03 can be optimal.)
2. gamma (Controls the step size of the algorithm. 0.001 can be optimal.)
3. numrnd (Number of random seed values. 1000 can be optimal.)
4. seed (Seed for the random number generator. Currently set to 1028).

Of course, both algorithms have their pros and cons. For example, brute force takes orders
of magnitude more time to run. SGD may not guarantee the best fragility fit and users should
change the SGD parameters and make sure that the results don't change significantly, to
ensure that the best results are calculated.

### Assumptions and current limitations

The `Fragility` VectorPostprocessor is currently limited to calculating the seismic fragilities
of individual SSC's (systems, structures and components) and therefore, one VectorPostprocessor
should be defined for each SSC. Additionally, `Fragility` VectorPostprocessor is limited to SSC's
that are sensitive to floor spectral acceleration demands at a particular frequency of excitation.

### Usage

An example usage of the `Fragility` VectorPostprocessor is below. For the `brute_force`
option, either true or false can be chosen. Apart from the input file (`fragility_brute_force.i`),
this VectorPostprocessor requires the csv files containing the output data from probabilistic or
stochastic simulations. An example output data file (`master_out_run_hazard03_sub1.csv`) is also
shown below. Note that the inputs to the parameters `master_file`, `hazard_multiapp`, and
`probabilistic_multiapp` are used to build the filename of the csv files that contain the
results of probabilistic simulations. Additionally, the `demand_variable` input parameter (accel_2x)
in this case is identical to the column name in the output data file.

!listing test/tests/vectorpostprocessors/fragility/fragility_brute_force.i block=VectorPostprocessors

!listing test/tests/vectorpostprocessors/fragility/master_out_run_hazard03_sub1.csv

The output file of the `Fragility` VectorPostprocessor is shown below. This file presents
the following parameters for each hazard bin of the probabilistic simulations:

1. intensity (from the hazard curve, same as the input, `im_values`)
2. median demand of the demand distribution
3. lognormal standard deviation of the demand distribution
4. conditional probability of failure of the component

Since the analysis corresponding to this example involves 4 bins (`num_bins` = 4),
the output file contains four rows for the first four columns. The last two columns
are the final results, namely, the median and lognormal standard deviation of the
seismic fragility distribution calculated for this SSC. This fragility distribution
is calculated by fitting a lognormal distribution into the 'intensity' and 'conditional_pf'
data using the Maximum Likelihood Estimate (MLE) fitting procedure described in
[!citet](bakermle2014). As mentioned previously, MLE is performed using an optimization
algorithm. Two algorithms are currently available: brute force and stochastic gradient descent.

!listing test/tests/vectorpostprocessors/fragility/gold/fragility_brute_force_out_fragility_pump_0002.csv

!syntax parameters /VectorPostprocessors/Fragility

!syntax inputs /VectorPostprocessors/Fragility

!syntax children /VectorPostprocessors/Fragility



!bibtex bibliography
