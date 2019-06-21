### Soil layers and meshing

Small strain properties (shear wave velocity, small strain modulus etc.) as well as mobilized shear strength of soils change with depth. Thus, in numerical models, soil profile (layers) is constructed to incorporate the depth dependent properties. The ground surface as well as layers that define the soil domain can be horizontal
or non-horizontal. For the horizontal ground surface and layering scenario, the location
of the interfaces can be provided as input and MASTODON will use that
information to generate a set of soil layers, each with a unique
identification number. These layer ids are later used to assign material
properties to each soil layer. The same procedure can also be used for
non-horizontal but planar soil layers by specifying the normal to the
plane and the interface locations measured along the normal direction.

For scenarios where the soil layers are non-horizontal and non-planar, images (.jpg, .png, etc.) of the soil profile can be provided as input. The different soil layers are distinguished from the image by reading either the red, green or blue color value (as per user's directions) at each pixel. Gray scale images in which the red, green and blue values are all the same also work well for this purpose. For creating 3D soil layers, multiple 2D images with soil profiles at different 2-D cross-sections of the soil domain can be provided as input.

Once the soil layers have been distinguished, it is necessary to ensure that the different soil layers are meshed such that they can accurately transmit waves of the required frequency. The optimum element size for each soil layer depends on the type of element used for meshing, cut-off frequency (f) of the wave and the shear wave velocity ($V_s$) of the soil layer. A minimum of 10 points is required per wavelength of the wave to accurately represent the wave in space [!citep](coleman2016time). The minimum wavelength ($\lambda_{min}$) is calculated as:

\begin{equation}
\lambda_{min} = \frac{V_s}{f}
\end{equation}

If linear elements such as QUAD4 or HEX8 are used, then the optimum mesh size is $\lambda_{min}/10$. If quadratic elements such as QUAD9 or HEX27 are used, then the optimum mesh size is $\lambda_{min}/5$. Using the minimum element size information, MASTODON refines the mesh such that the element size criterion is met and at the same time the layers separations are visible. An example of this meshing scheme is presented in [fig:adaptive_meshing] where a 2D soil domain is divided into 3 soil layers and these soil layers are meshed such that the element size criterion is satisfied. A denser mesh is created at the interface between different soil layers.

!media media/theory/adaptive_mesh.png
       style=width:60%;margin-left:150px;float:center;
       id=fig:adaptive_meshing
       caption=Auto-generated mesh for a soil domain with three non-horizontal non-planar soil layers.
