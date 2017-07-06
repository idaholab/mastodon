# ShearWaveIndicator

!syntax description /Adaptivity/Indicators/ShearWaveIndicator

## Description
An indicator that computes the minimum element size based on the shear-wave speed.

The minimum element length ($\lambda_{min}$), which this indicator computes, is computed based on the cutoff
frequency ($f$) and shear-wave speed ($v_s$) defined above as:

$$
\lambda_{min} = \frac{v_s}{f}.
$$

The shear wave speed is supplied as a material property and the cutoff frequency is defined as a constant value
via the input parameters.

!syntax parameters /Adaptivity/Indicators/ShearWaveIndicator

!syntax inputs /Adaptivity/Indicators/ShearWaveIndicator

!syntax children /Adaptivity/Indicators/ShearWaveIndicator
