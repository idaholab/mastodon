# MinimumElementSizeMarker

!syntax description /Adaptivity/Markers/MinimumElementSizeMarker

## Description

A marker that flags elements for refinement based on element length. The element size is provided to this object either
via the "indicator" parameter or an "element_size" value. If an element has a minimum dimension greater than the size defined by either the Indicator or the element_size input parameter, then the element is marked for refinement,
 otherwise it is not marked.

!syntax parameters /Adaptivity/Markers/MinimumElementSizeMarker

!syntax inputs /Adaptivity/Markers/MinimumElementSizeMarker

!syntax children /Adaptivity/Markers/MinimumElementSizeMarker
