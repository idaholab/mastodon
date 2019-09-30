## User-defined Backbone Curve id=user-defined

A procedure to verify the ability of the user-defined backbone curve to control the stress-strain behavior of the element is presented in this section. The soil was assigned a density of 2000 kg/m^3^ and a Poisson's ratio of 0.3. The backbone curve was defined as shown in [backbone-input].

!plot scatter filename=test/tests/materials/isoil/gold/stress_strain_darendeli.csv
              data=[{'x':'strain', 'y':'stress', 'name':'User-defined Backbone Curve'}]
              layout={'xaxis':{'title':'Strain'},
                      'yaxis':{'title':'Stress (Pa)'},
                      'title':'User-defined Backbone Curve'}
              id=backbone-input
              caption=The user-defined backbone curve input into MASTODON to control the stress-strain behavior of the soil using I-Soil.

To investigate the behavior of the user-defined backbone curve, it was assumed that the element had zero initial stresses, no self weight (i.e., zero gravity), and undamped vibration.

### Results

The hysteresis stress-strain curve in the ZX plane is shown in [user-defined-hysteresis-loop].

!plot scatter filename=test/tests/materials/isoil/gold/data_file_hysteresis.csv
              data=[{'x':'strain_zx_el', 'y':'stress_zx_el', 'name':'Hysteresis Loop in the zx-plane'}]
              layout={'xaxis':{'title':'strain_zx'},
                      'yaxis':{'title':'stress_zx (Pa)'},
                      'title':'Hysteresis Loop in the ZX Plane'}
              id=user-defined-hysteresis-loop
              caption=Plot of the element hysteresis stress-strain curve in the ZX plane over the duration of the simulation.

Since the engineering strain in the ZX plane, $\gamma_{zx}$, is equal to two times the tensor shear strain, $\epsilon_{zx}$, it was necessary to multiply the strains output by MOOSE TensorMechanics by two to get the shear stresses, $\tau_{zx}$, which correspond to those defined in [backbone-input]. Next, the engineering shear stress-strain curve was superimposed over those points in [backbone-input], as shown in [user-defined-backbone-curve]. The backbone curve was obtained in MOOSE by sampling the data during the initial load cycle, i.e., from 3 seconds to 4 seconds (see [#problem-statement]). It is clear from [user-defined-backbone-curve] that the user-defined backbone curve was consistent with the MASTODON one.

!media media/materials/isoil/data_file_backbone.png
       id=user-defined-backbone-curve
       caption=The user-defined backbone curve and the engineering shear stress-strain curve in the ZX plane output by MASTODON during the initial loading cycle (from 3 seconds to 4 seconds).
       style=width:100%

Since the maximum stress in the user-defined backbone curve of 3805 Pa was exceeded during the solve, it can be seen in [user-defined-backbone-curve] that the soil continues to deform without any additional shear stress.

### Complete Input File

!listing test/tests/materials/isoil/HYS_data_file_verification.i
