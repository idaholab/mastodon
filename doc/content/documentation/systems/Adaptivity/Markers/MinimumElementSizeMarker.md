# MinimumElementSizeMarker

!syntax description /Adaptivity/Markers/MinimumElementSizeMarker

## Description
A marker that flags elements for refinement based on element length. The element size is provided to this object via
the "indicator" parameter. If an element has a minimum length less than the minimum element defined by the Indicator
then the element is marked for refinement, otherwise it is not marked.

!syntax parameters /Adaptivity/Markers/MinimumElementSizeMarker

!syntax inputs /Adaptivity/Markers/MinimumElementSizeMarker

!syntax children /Adaptivity/Markers/MinimumElementSizeMarker
