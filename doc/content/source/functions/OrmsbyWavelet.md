# Ormsby Wavelet

!syntax description /Functions/OrmsbyWavelet

This object generates an Ormsby wavelet defined by [ormsby_def] below. The Ormsby
wavelet is a wavelet function that has a near constant Fourier amplitude for a
given frequency range. It is defined by four frequencies, $f_1$,
$f_2$, $f_3$, and $f_4$, and the time of maximum amplitude, $t_s$. The approximate
shape of the fast Fourier transform (FFT) of the Ormsby wavelet is as follows:
it starts at zero, increases linearly between
$f_1$ and $f_2$, stays constant between $f_2$ and $f_3$, decreases linearly to
zero between $f_3$ and $f_4$, and stays at zero after $f_4$. The OrmsbyWavelet
function returns an amplitude-normalized wavelet with a maximum amplitude of 1.0.

\begin{equation}
\label{ormsby_def}
\begin{aligned}
f(t)=\left[\frac{\pi f_4^2}{f_4-f_3}sinc(\pi f_4(t-t_s))-\frac{\pi f_3^2}{f_4-f_3}sinc(\pi f_3(t-t_s))\right]- \\
     \left[\frac{\pi f_2^2}{f_2-f_1}sinc(\pi f_2(t-t_s))-\frac{\pi f_1^2}{f_2-f_1}sinc(\pi f_1(t-t_s))\right]
\end{aligned}
\end{equation}

A sample Ormsby wavelet calculated using the $f_1$, $f_2$, $f_3$, and $f_4$ values
of 0 $Hz$, 0.2 $Hz$, 20 $Hz$, and 25 $Hz$, a $t_s$ value of 2.0 $sec$, and a scale
factor of 0.5, is shown in [sample]. The input syntax for generating this wavelet
is shown below.

!listing test/tests/functions/ormsby_wavelet/ormsby.i start=[Functions] end=[Postprocessors]

!plot scatter filename=test/tests/functions/ormsby_wavelet/gold/ormsby_out.csv
              data=[{'x':'time', 'y':'func', 'name':'Ormsby wavelet'}]
              layout={'xaxis':{'title':'Time (sec)'},
                      'yaxis':{'title':'Amplitude'},
                      'title':'The Ormsby wavelet'}
              caption=Time series of a sample Ormsby wavelet
              id=sample

More detailed information on the Ormsby wavelet can be found in Appendix B of
[!citet](BolisettiandWhittakerMCEER2015), as well as [here](https://wiki.seg.org/wiki/Ormsby_wavelet).

!syntax parameters /Functions/OrmsbyWavelet

!syntax inputs /Functions/OrmsbyWavelet

!syntax children /Functions/OrmsbyWavelet

!bibtex bibliography
