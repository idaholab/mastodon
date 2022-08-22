# Ground motion simulation with application to Monte Carlo analysis of FSI


### Problem Statement

The goal is to stochastically simulate ground motions using Magintude (M), Distance (R), and Shear Wave Velocity (Vs30). Other input parameters that cannot currently be generated stochastically
are Distance Type (Joyner-Boore or Epicentral) and Style of Faulting (Strike-Slip, Thrust Fault, Normal Fault). This example is a model of a cylindrical tank filled with fluid and will utilize Monte Carlo Sampling from the Stochastic Tools
Module in MOOSE and the Fluid-Structure Interaction Module for the solve.


!media media/examples/ex15/TankSchem.png
       style=width:40%;margin:0 auto;float:center;
       id=fig:Schematic
       caption=Schematic of the Tank and its dimensions.


## GroundMotionSim Function

The GroundMotionSim Function object in MASTODON takes simple geophysical inputs and generates realistic ground motions that can then be applied to the model. [list1] below shows an example function block with inputs. The function is defined by 6 inputs, 3 of which can be stochastically generated. The stochastically compatible inputs are Magnitude (M), Distance (R), and Shear Wave Velocity at a depth of 30m (Vs30). The remaining two variables, are Style of Faulting (F), Distance type (R_type) and random seed (n). There are 3 distinct fault types that can be used in this function: Strike-Slip, Thrust Fault, and Normal Fault denoted as 'SS','TF', and 'NF', respectively, with the default being 'SS'. R_type indicates whether the distance provided in R is Joyner-Boore Distance or Epicentral Distance, denoted as 'Rjb' (default) or 'Repi'. The n variable is a random seed used to ensure consistent ground motions with the same set of input parameters. Part of how the function generates the ground motions uses randomly generated phase angles, so the same set of input parameters can yield slightly different ground motions if the seed is not set, although the frequency domains are equivalent. A generated ground motion based on the set of inputs in [list1] are shown in [sample]. 


!listing test/tests/functions/ground_motion_sim/ground_motion_sim.i start=[Functions] end=[Postprocessors] id=list1


!plot scatter filename=test/tests/functions/ground_motion_sim/gold/ground_motion_sim_out.csv
              data=[{'x':'time', 'y':'func', 'name':'Ground Motion'}]
              layout={'xaxis':{'title':'Time (sec)'},
                      'yaxis':{'title':'Acceleration (m/s<sup>2</sup>)'},
                      'title':'Simulated Ground Motion'}
              caption=Time series of a sample ground motion
              id=sample


## Application using Monte Carlo Simulation

Now that we have a basic understanding of the function, we can stochastically generate ground motions and use these motions for seismic uncertainty quantification. The variables M, R, and Vs30 will be generated from normal distributions, using 20 samples. We will use Strike-Slip ('SS') for the Style of Faulting (F), Joyner-Boore Distance ('Rjb') for R_type and use random seed (n) 0. The input variable distributions from the generated samples are shown in [MC_M],[MC_Dist], and [MC_vs30]. The quanitites of interest will be the Peak Pressure, Peak Wave Height, and Peak Acceleration (at bottom of tank). We can show how the changes in our input parameters affect these quanities.

!listing examples/ex15/monte_carlo.i

!listing examples/ex15/TankUQ_mod.i

!row!
!col! small=12 medium=6 large=6
!plot histogram filename=examples/ex15/monte_carlo_out_data_0000.csv
                vectors=sample_0
                xlabel=Magnitude
                id=MC_M
                legend=False
                caption=Input distribution of Magnitudes.
!col-end!

!col! small=12 medium=6 large=6
!plot histogram filename=examples/ex15/monte_carlo_out_data_0000.csv
                vectors=sample_1
                xlabel=Joyner-Boore Distance
                id=MC_Dist
                legend=False
                caption=Input distribution of Distances.
!col-end!

!col! small=12 medium=6 large=6
!plot histogram filename=examples/ex15/monte_carlo_out_data_0000.csv
                vectors=sample_2
                xlabel=Vs30
                id=MC_vs30
                legend=False
                caption=Input distribution of Vs30.
!col-end!

!row-end!

### Results: Sensitivity and Uncertainty

The pairplot in [fig:pairplot] shown below shows the relationships between the input parameters and the responses. From this, we can see that higher magnitudes and shorter Rjb distances lead to higher responses, as expected.

!media media/examples/ex15/pairplot.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:pairplot
       caption=Pairplot


The 3D plots show how the interactions of the input parameters are related to the responses

!row!
!col! small=12 medium=6 large=6
!media media/examples/ex15/PeakAcc.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:3Dacc
       caption=3D Peak Acceleration plot
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/ex15/PeakDisp.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:3Ddisp
       caption=3D Peak Displacement plot
!col-end!
!row-end!

!row!
!col! small=12 medium=6 large=6
!media media/examples/ex15/PeakPressure.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:3Dpress
       caption=3D Peak Pressure plot
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/ex15/PeakWaveHeight.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:3Dwh
       caption=3D Peak Wave Height plot
!col-end!
!row-end!

We can plot the Sample Indices against the response values to see how well the algorithm explored the sample space.

!row!
!col! small=12 medium=6 large=6
!media media/examples/ex15/sampleindex_Peak_Acc.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:accidx
       caption= Peak Acc Sample Indices
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/ex15/sampleindex_Peak_Disp.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:dispidx
       caption= Peak Disp Sample Indices
!col-end!
!row-end!

!row!
!col! small=12 medium=6 large=6
!media media/examples/ex15/sampleindex_Peak_Pressure.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:pressidx
       caption= Peak Pressure Sample Indices
!col-end!

!col! small=12 medium=6 large=6
!media media/examples/ex15/sampleindex_Peak_Wave_Height.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:whidx
       caption= Peak Wave Height Sample Indices            
!col-end!
!row-end!

We can also utilize the ResponseSpectraCalculator from the VectorPostprocessors to visualize the response spectra induced by the applied motion over all simulations. The spectra were recorded at the red circle in [fig:Schematic]. 

!media media/examples/ex15/spectra.png
       style=width:100%;margin-right:0px;float:center;
       id=fig:spectra
       caption=Response Spectra