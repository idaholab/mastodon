!devel /Adaptivity/Indicators/LayerInterfaceIndicator float=right width=auto margin=20px padding=20px background-color=#F8F8F8

# LayerInterfaceIndicator

!description /Adaptivity/Indicators/LayerInterfaceIndicator

## Description
This indicator detects layer boundaries, when a layer is defined by an Variable. Given a variable that defines a
"layer id" for the system, this indicator will have a value of 1.0 if the current element has a neighbor with
a different id. Otherwise, the indicator value is set to zero.

!parameters /Adaptivity/Indicators/LayerInterfaceIndicator

!inputfiles /Adaptivity/Indicators/LayerInterfaceIndicator

!childobjects /Adaptivity/Indicators/LayerInterfaceIndicator
