# ElementSizeIndicator

!syntax description /Adaptivity/Indicators/ElementSizeIndicator

## Description

This indicator takes a `MaterialProperty` value as an input and passes this
information to a `Marker` for mesh adaptivity. When the `ElementSizeIndicator`
is used with a `MinimumElementSizeMarker`, the mesh is refined until the minimum
element size is equal to the `MaterialProperty` value provided by the user.

As an example, the following input file demonstrates the use of `ElementSizeIndicator`.
 The `Adaptivity` block requests the types of indicator and marker, in addition
to the number of `initial_steps` for adaptivity. In the `Indicators` sub-block, the
`element_size` MaterialProperty is specified with the name `ele_size`.

!listing test/tests/indicators/element_size_indicator/element_size_indicator.i
         block=Adaptivity

The `Materials` block specifies the required element size through a `MaterialProperty`,
 named `ele_size`.

!listing test/tests/indicators/element_size_indicator/element_size_indicator.i
          block=Materials

At the end of Adaptivity, provided there are enough number of `initial_steps`,
the minimum element size in the block will be equal to the given `element_size` in
the `Indicators` sub-block.

!syntax parameters /Adaptivity/Indicators/ElementSizeIndicator

!syntax inputs /Adaptivity/Indicators/ElementSizeIndicator

!syntax children /Adaptivity/Indicators/ElementSizeIndicator
