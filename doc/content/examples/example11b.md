# Example 11b: Mesh refinement using `Marker` and `Indicator`, and demonstration of `SidesetMoment` and `AverageValue` postprocessors

This example expands on [Example 11a](examples/example11.md) to demonstrate mesh refinement using `Markers` and `Indicators`. This example also demonstrates the use and verification of `SidesetMoment` and `AverageValue` postprocessors. For demonstration purposes,
the beam model subjected to static loading from [Example 11a](examples/example11.md) is borrowed. This beam model is divided into three subdomains represented using three colors (red, green, and gray) in the figure below.

!media media/examples/ex11b/ex11b_blocks.png
       style=width:50%;margin-left:150px;float:center;
       id=fig:beam_blocks
       caption=Beam model with the domain divided into three subdomains. The red, green, and gray colors represent three subdomains.

## Mesh refinement using `Indicator` and `Marker`

MASTODON is capable of automatic mesh refinement using the MOOSE's [Adaptivity System](syntax/Adaptivity/index.md). The [Adaptivity System](syntax/Adaptivity/index.md) can use an [`Indicator`](/Adaptivity/Indicators/index.md) to provide measurement of "errors" about the required mesh refinement. These "errors" are passed on to a [`Marker`](/Adaptivity/Markers/index.md) to decide which elements need refinement. Alternatively, the `Marker` can calculate the "errors" by itself to inform the mesh refinement without relying on the `Indicator`. Both these cases will be demonstrated. Specifically, the [`MinimumElementSizeMarker`](markers/MinimumElementSizeMarker.md) for mesh refinement is demonstrated. The [`MinimumElementSizeMarker`](markers/MinimumElementSizeMarker.md) is capable of refining elements based on either an `indicator` or an `element_size` value. These capabilities are first individually demonstrated below, and are then combined using the [`ComboMarker`](/markers/ComboMarker.md).

### Mesh refinement using the `MinimumElementSizeMarker` with the `indicator` option

In the cantilever beam model, the goal is to refine the subdomain in green with the "errors" computed by the [`ShearWaveIndicator`](indicators/ShearWaveIndicator.md). The initial mesh is such that it has 10 partitions along the length (x-direction) and 2 partitions along the breadth and height (y- and z-directions). The shear-wave velocity of the beam model is set to 250 m/s as shown below.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=Materials/shear_wave_speed
         end=Preconditioning

The [`ShearWaveIndicator`](indicators/ShearWaveIndicator.md) is defined in the `Adaptivity` block shown below with a cutoff frequency of 1000 Hz. With a shear_wave velocity of 250 m/s, this implies the minimum element size will be 0.25 m in the green subdomain.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=Indicators
         end=Markers

The `Markers` block has the `MinimumElementSizeMarker` with the `indicator` option, as shown below.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=marker2
         end=combo

 After the mesh refinement, as presented in the figure below, we see that the green subdomain has an element size less than or equal to 0.25 m, as specified by the [`ShearWaveIndicator`](indicators/ShearWaveIndicator.md). The initial element size in the green subdomain before refinement is 0.5 m.

!media media/examples/ex11b/ex11b_indicatormarker.png
      style=width:50%;margin-left:150px;float:center;
      id=fig:beam_indicatormarker
      caption=Beam model with the green block's mesh refined using the [`MinimumElementSizeMarker`](markers/MinimumElementSizeMarker.md) with the `indicator` option.

### Mesh refinement using the `MinimumElementSizeMarker` with the `element_size` option

Alternatively, instead of relying on an indicator, the [`MinimumElementSizeMarker`](markers/MinimumElementSizeMarker.md) can directly take an `element_size` value for the mesh refinement. This capability is demonstrated here. The goal is refine the red subdomain in the beam such that the minimum element size is less than or equal to 0.15 m. This can be accomplished using the `element_size` option as shown in the code block below.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=marker1
         end=marker2

The end result, as presented in the figure below, is the red subdomain having a element size less than 0.15 m. Additionally, it is worth noticing that the green subdomain is also refined near the interface with the red subdomain. However, we did not intend to refine the green subdomain here. This behavior is due to the nature of $h$-adaptivity which MOOSE employs. For more information, please refer to the [Adaptivity System](syntax/Adaptivity/index.md).

!media media/examples/ex11b/ex11b_valuemarker.png
      style=width:50%;margin-left:150px;float:center;
      id=fig:beam_valuemarker
      caption=Beam model with the red block's mesh refined using the [`MinimumElementSizeMarker`](markers/MinimumElementSizeMarker.md) with the `element_size` option.

### Mesh refinement using the `ComboMarker`

It is also possible to combine two different `Markers` into a single `Marker` with the [`ComboMarker`](/markers/ComboMarker.md) option. The goal now is to combine the `MinimumElementSizeMarker` with the `indicator` option and `MinimumElementSizeMarker` with the `element_size` option. The below code block accomplishes this.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=Markers
         end=GlobalParams

The figure below presents the mesh of the beam after refinement using the [`ComboMarker`](/markers/ComboMarker.md). Notice that even though the goal is to refine the red subdomain to have an element size less than 0.15 m and the green subdomain to have an element size less than 0.25 m, both these subdomains have an element size less than 0.15 m. This behavior is due to how the [`ComboMarker`](/markers/ComboMarker.md) operates. Given several `Markers`, the [`ComboMarker`](/markers/ComboMarker.md) refines all the associated subdomains based on the largest refinement required across all the given `Markers`.

!media media/examples/ex11b/ex11b_combomarker.png
      style=width:50%;margin-left:150px;float:center;
      id=fig:beam_combomarker
      caption=Beam model with the red and green blocks' mesh refined using the [`ComboMarker`](/markers/ComboMarker.md).

## Demonstration of `SidesetMoment` and `AverageValue` postprocessors

Often during analyses of structures, we are required to compute auxiliary quantities such as stresses and moments which are dependent on the primary variables, the displacements. These auxiliary quantities are computed using postprocessors. In this example, two types of postprocessors (namely, `SidesetMoment` and `AverageValue`) are demonstrated.

### `SidesetMoment` postprocessor

[`SidesetMoment`](/postprocessors/SidesetMoment.md) is a generic postprocessor used for computing moments about a required axis acting at a reference point on a sideset. Therefore, this postprocessor takes the reference point coordinates and either stress or pressure as some of the inputs. Stress is used in this example. In addition, it requires the stress and moment directions. These directions should be defined such that their cross product should be the direction about which the moment acts. For example, in the code block below, it is desired to compute the moment at the left end (i.e., the fixed end) of the beam about the y-axis. Now, the stress direction is defined as (1,0,0) and the moment direction is defined as (0,0,1). The cross product of these gives (0,1,0) or moment about the y-axis.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=Postprocessors
         end=Outputs

The computed moment from stresses using the `SidesetMoment` postprocessor is 5180 N. Where as, the applied moment at this sideset is 5000 N. It is seen that the computed and applied moments are quite close, with an error of about 3.6 %. This error can be further reduced by refinement of the mesh.

### `SideAverageValue` and `ElementAverageValue` postprocessors

Both [`SideAverageValue`](/postprocessors/SideAverageValue.md) and [`ElementAverageValue`](/postprocessors/ElementAverageValue.md) postprocessors compute averaged values of quantities (such as stresses or displacements) across a sideset and subdomain, respectively. The below code block uses these two postprocessors to compute averaged stress in the x-direction across a sideset and a subdomain.

!listing examples/ex11b/3d_beam_static_refine_PP.i
         start=Postprocessors
         end=Outputs

The computed average stress across the left end sideset and the red subdomain of the beam are both close to zero. Theoretically, these close to zero values are due to the symmetry of the stress distribution along cross section of the beam.
