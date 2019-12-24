# Example 8: Seismic risk assessment of a generic nuclear facility using the MASTODON FTA Python module

## Model description

This example demonstrates the seismic risk assessment of a generic nuclear facility (GNF) located at the
INL site and subject to the INL site seismic hazard. The risk analysis is performed using the MASTODON's
FTA Python module that performs fault tree analysis and calculates the seismic risk.

!alert note title=This example does not use `.i` files
This example involves the usage of the MASTODON FTA Python module. This module is separate from the MASTODON finite-element analysis and resides in the `mastodonutils` package. The files for this package can be found in the `mastodon/python/mastodonutils` folder.

The generic nuclear facility (GNF) represents a safety-critical facility in
the Department of Energy (DOE) complex. The GNF is assumed to host materials at risk (MAR) that
need to be contained in the facility and is classified as a Seismic Design category 3 (SDC 3) structure
per ASCE 43 [!citep](asce43-05). The containment structure of GNF is assumed to be the
primary containment and that the collapse of the structure will release the MAR
into the atmosphere, resulting in radiation exposure of collocated workers and off-site
receptors. Therefore, the seismic risk assessment of the GNF involves the calculation
of the collapse risk of the GNF structure.

The inputs for seismic risk assessment of GNF include the seismic hazard at the INL
site, the seismic collapse fragility of the GNF structure, and the event tree and
fault tree for the structural collapse. For this example, the seismic hazard curve is adopted
from the probabilistic seismic hazard analyses performed for the INL site. The
seismic fragilities, event tree, and fault trees are gathered from those of similar
facilities and are assumed to be representative of safety-critical DOE facilities.

### Seismic hazard

The seismic risk assessment of GNF is performed for two seismic hazard curves, both
corresponding to the INL site. The first hazard curve is calculated from the PSHA for
the INL site performed in 2006, and the second hazard curve corresponds to an updated
PSHA performed in 2015 [!citet](inl-ext-16-37751). The updated hazard curve is a result of using
new data and models for characterizing the seismic sources and the ground motions
at the target site, as described in [!citet](Payne2017b) and [!citet](Payne2017).
[fig:SH] presents the original and updated seismic hazard curves and shows that
the 2015 curve forecasts a smaller seismic hazard than the 2006 curve for most
spectral accelerations.

!media media/examples/ex08/Seismic_Hazard.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:SH
       caption=Seismic hazard curves and the corresponding bin values for the 2006 and the 2015 assessments.

### Seismic fragility

Seismic fragility here refers to the probability of collapse of the GNF under
seismic shaking. It is assumed that the spectral acceleration ($Sa$) at 10Hz is a
representative seismic demand for the GNF and therefore, the seismic fragility (as well as
the seismic hazard) is expressed in terms of $Sa$ at 10Hz. The fragility curve
and its uncertainty are expressed using a double lognormal distribution described
by the lognormal median (Am), and logarithmic standard deviations representing randomness
and uncertainty, $\beta_R$ and $\beta_U$, respectively. The median fragility
function, which is the collapse probability conditioned on the input ground spectral
acceleration at 10Hz, is used for the risk assessment in this example. This fragility
is calculated as:

\begin{equation}
\label{eq2}
P\big(collapse|Sa(10Hz)) = \Phi \Big(\frac{ln\big(Sa(10Hz)/A_m\big)}{\beta_C}\Big)
\end{equation}

where, $\beta_C$ is the composite logarithmic standard deviation calculated as
$\sqrt{\beta_R^2 + \beta_U^2}$ and $\Phi$ is the standard normal cumulative
distribution function.

Note that the seismic fragilities also were updated in 2015 and therefore the
seismic fragilities for the 2006 assessment and the 2015 assessment are different.
The structural collapse seismic fragility for the 2006 and 2015 hazard are listed
in [table-fragilities] below. These fragilities are also plotted in [fig:fra1]
and [fig:fra2] below.

!table id=table-fragilities caption=Structural collapse fragilities for GNF.
| Year of assessment | $A_m$ | $\beta_R$ |$\beta_U$ |$\beta_C$ |
| - | - | - | - | - |
| 2006 | 1.31 | 0.24 | 0.38 | 0.45 |
| 2015 | 2.11 | 0.29 | 0.41 | 0.50 |

!row!

!col! small=12 medium=6 large=6
!media media/examples/ex08/Fragility_2006.png
       style=width:100% id=fig:fra1
       caption=Mean, median, 5th, and 95th percentile fragility functions for the 2006 assessment.
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/ex08/Fragility_2015.png
       style=width:100% id=fig:fra2
       caption=Mean, median, 5th and 95th percentile fragility functions for the 2015 assessment.
!col-end!

!row-end!

### Event tree and fault tree for risk evaluation

The event tree and fault tree of the GNF used in this example are presented in
[fig:ET] and [fig:FT], respectively.
The seismic failure case of the GNF can be expressed as a simple fault tree with
a single basic event, which is the collapse of the structure (named `STRUCT_COLLAPSE`)
from a seismic event. Since the MASTODON FTA module requires at least two basic
events, this event is combined with a `DUMMY` event with a probability of 1.0 as
shown in [fig:FT]. This is described further in the next section. The event tree
can also be expressed using a single event, where the collapse of
the structure leads to a breach in containment.

!media media/examples/ex08/ET.png
       style=width:80%;margin-right:0px;float:center;
       id=fig:ET
       caption=Event tree for each bin of the seismic hazard curve for both the 2006 and the 2015 assessments. The logic of the event trees across the bins is the same except for the initiating event.

!media media/examples/ex08/FT.png
       style=width:80%;margin-right:0px;float:center;
       id=fig:FT
       caption=Fault tree used for aggregating the seismic risk for both the 2006 and the 2015 assessments.

## Inputs to the MASTODON FTA Python module

Seismic risk assessment of the GNF is performed separately for the two seismic
hazard curves. The seismic risk is calculated by splitting the hazard curve into
a number of bins, and calculating the risk of collapse for each bin. In this
example, both the seismic hazard curves in [fig:SH] are split into 10 bins. The
spectral acceleration corresponding to each bin is calculated as the geometric
mean of the spectral accelerations of the extents of the bin. The corresponding
mean annual frequency of exceedance (MAFE) for the bin is calculated through linear
interpolation of the hazard curve in the log-log scale.

The inputs required for the seismic risk assessment using the FTA python module
include the seismic hazard curve, fault tree logic, and the seismic fragilities
of the basic events. The MASTODON FTA module is currently limited to fault tree analyses,
and not event tree analyses. Since the event tree in this example comprises of a
single event, event tree analysis will not be required. The seismic hazard,
fault tree logic, and basic event fragilities can be either input directly as
python lists, or as csv files. For this example, they are provided as csv files.
The seismic hazard input file for the 2006 hazard is listed below. As seen in
the file, the seismic hazard is simply specified as pairs of intensity measure
and the corresponding MAFE.

!listing examples/ex08/seismic_hazard_2006.csv

The fault tree analysis in the FTA module cannot be performed with a single basic
event, which is the case for this example. Therefore, a dummy basic event with a
point estimate probability of 1.0 is included in the fault tree logic along with
an AND condition. This is logically
equivalent to having the one basic event as shown in [fig:FT]. The fault tree
logic input file is shown below, along with the basic event input file. The two basic
events in the files below named `STR_COLLAPSE` and `DUMMY`, correspond to the
collapse of the structure and the dummy event, respectively. `TE` in the logic
file denotes the top event for which, the probability of failure is being
calculated. Note that the structural collapse fragility is expressed as a
lognormal distribution (indicated by `LNORM` in the basic events file) with the
median and composite logarithmic standard deviation as described before. The
basic event fragility input files for the 2006 assessment is listed below.

!listing examples/ex08/logic.csv

!listing examples/ex08/bas_events_2006.csv

The analysis is performed in Python by simply creating an instance of the FaultTree
class in the FTA module. For example, the assessment for the 2006 hazard is
performed by the code below.

```
from mastodonutils import FTA
quant2006 = FTA.Quantification("gnf_2006",
                           logic='logic.csv',
                           basic_events='bas_events_2006.csv',
                           analysis='Fragility',
                           hazard='seismic_hazard_2006.csv',
                           IM=[0.04, 0.11, 0.21, 0.42, 0.64, 0.87, 1.09, 1.52, 3.26, 4.34, 6.51],
                           lite=True,
                           nbins=10,
                           write_output=True)
```

In this code, note that the logic, basic event fragilities, and the seismic hazard
are provided using the input parameters, `logic`, `basic_events`, and `hazard`,
respectively. The `IM` parameter inputs the intensity bins in the hazard curve.
In this case, since a list of intensities is provided, MASTODON will assume that
they correspond to the extents of the intensity bins. Since 11 intensities are
provided here, MASTODON uses these values to divide the hazard curve into 10 bins,
and the `nbins` parameter is ignored. The `lite` option ensures that Approach 2 is
used to calculated the risk. Details of this approach can be found in the
documentation in the `FTA.py` file. The `write_output` option ensures that the
results are saved in csv format.

This code and the code for the 2015 hazard can be found in the Python notebook,
`risk_calcs.ipynb` of the `example8` folder. Executing this file will perform
the risk analysis for the 2006 and 2015 seismic hazards, and also plot the
resulting collapse frequencies for all bins.

## Results

The results calculated from the code above will be saved as csv files in the
folder, `gnf_2006_results/approach_2/`. Since the `lite` version of the analysis
is performed, the results from Approach 1 are not calculated. The results file
for the analysis in this example is listed below.

!listing examples/ex08/gnf_2006_results/approach_2/results.csv

The columns names in the results file indicate the following:

- `bin#`: bin number
- `im`: intensity measure ($Sa$ at 10Hz in this case).
- `mafe`: MAFE for the bin.
- `top_failure_prob`: failure probability of top event, i.e., the top event
   fragility. In this case, the collapse probability of the structure given
   an input ground $Sa$ at 10Hz.
- `delta_mafe`: mean annual frequency of occurrence of the intensity measures
    of the bin. This is calculated as the difference between the MAFE values of the
    extents of bin.
- `bin_risk`: risk corresponding to this bin and is calculated as the
    product of `top_failure_prob` and `delta_mafe`. In this case, it is the collapse
    frequency for the bin. The summation of this value across all the bins provides the total
    risk.

The collapse fragility calculated from the fault tree analysis is convolved with
the hazard curve to calculate the final risk of collapse of the facility. In the
MASTODON FTA module, the final risk is stored in the `toprisk_2' class variable and
can be printed as follows:

```
print quant2006.toprisk_2
```

The final collapse risk for the 2006 and the 2015 assessments calculated using
MASTODON are listed in [table-risks] below.

!table id=table-risks caption=Structural collapse risks for GNF calculated using MASTODON.
| Year of assessment| Risk of collapse |
| - | - |
| 2006 | 5.27e-05 |
| 2015 | 5.20e-06 |

Note that the collapse risk calculated using the updated seismic hazard and
fragilities in 2015 is almost an order of magnitude smaller than those calculated
in 2006. The collapse frequencies for each bin calculated using MASTODON for the
2006 and 2015 assessment are plotted in [fig:Risk] below. Note that the collapse
frequencies in each bin are also smaller in the 2015 assessment, except for the last
bin where the collapse frequencies for the two assessments are almost equal.

!media media/examples/ex08/collapse_frequencies.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:Risk
       caption=Collapse frequencies for the 2006 and 2015 seismic risk
               assessments of GNF calculated using MASTODON.


!bibtex bibliography
