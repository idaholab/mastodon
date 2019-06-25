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

### Assumptions and current limitations

The `Fragility` VectorPostprocessor is currently limited to calculating the seismic fragilities
of individual SSC's (systems, structures and components) and therefore, one VectorPostprocessor
should be defined for each SSC. Additionally, `Fragility` VectorPostprocessor is limited to SSC's
that are sensitive to floor spectral acceleration demands at a particular frequency of excitation.

### Usage

An example usage of the `Fragility` VectorPostprocessor is below. Apart from the input file,
`fragility.i` the this VectorPostprocessor requires the csv files containing the output data from
the probabilistic or stochastic simulations. An example output data file, `master_out_run_hazard03_sub1.csv`,
is also shown below. Note that the inputs to the parameters `master_file`, `hazard_multiapp`, and
`probabilistic_multiapp` are used to build the filename of the csv files that contain the results of
probabilistic simulations. Additionally, the `demand_variable` input parameter (accel_2x) in this case
is identical to the column name in the output data file.

!listing test/tests/vectorpostprocessors/fragility/fragility.i block=VectorPostprocessors

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
[!citet](bakermle2014).

!listing test/tests/vectorpostprocessors/fragility/gold/fragility_out_fragility_pump_0002.csv

!syntax parameters /VectorPostprocessors/Fragility

!syntax inputs /VectorPostprocessors/Fragility

!syntax children /VectorPostprocessors/Fragility



!bibtex bibliography
