### Domain reduction method (DRM)

Earthquake 'source-to-site' simulations require simulating a huge soil domain (order of many
kilometers) with a earthquake fault. The nuclear power plant structure, which is usually less than
100 m wide, is located very far from the earthquake fault, and the presence of the structure only
affects the response of the soil in the vicinity of the structure. In most of these situations, where
a localized feature such as a structure is present in a huge soil domain, the problem can be divided
into two parts: (i) a free-field 'source-to-site' simulation is run on the huge soil domain (
[fig:DRM](a)) that does not contain the localized feature, and (ii) the forces from the free-field
simulation at one element layer, which is the element layer separating the bigger and smaller soil
domain, can be transferred to a much smaller domain containing the localized feature (
[fig:DRM](b)). This method of reducing the domain is called the domain reduction method (DRM)
[citep:bielak2003domain]. [fig:DRM] is reproduced from [citet:bielak2003domain].

!media media/theory/DRM.png
       style=width:100%;float:center;
       id=fig:DRM
       caption=Domain reduction method summary: (a) Big soil domain containing the earthquake fault
               but not the localized feature. The displacements are obtained at the boundaries
               $\Gamma$ and $\Gamma_e$ and are converted to equivalent forces. (b) Smaller soil
               domain containing the localized feature but not the earthquake fault. The equivalent
               forced calculated in (a) are applied at the boundaries $\Gamma$ and $\Gamma_e$.

To convert the displacements at $\Gamma$ and $\Gamma_e$ from part (i) to equivalent forces, a finite
element model of the one element layer between $\Gamma$ and $\Gamma_e$ is simulated in two
steps. First, the boundary $\Gamma_e$ is fixed and the boundary $\Gamma$ is moved with the
displacements recorded at $\Gamma$. This step gives the equivalent forces at $\Gamma_e$. Second, the
boundary $\Gamma$ is fixed and the boundary $\Gamma_e$ is moved with the displacements recorded at
$\Gamma_e$. This steps gives the equivalent forces at $\Gamma$.

Note: The meshes for the bigger soil domain and smaller soil domain need not align between $\Gamma$
and $\Gamma_e$. The equivalent forces can be applied as point forces at the same coordinate location
at which nodes are present in the bigger model, irrespective of whether these locations correspond to
nodal locations in the smaller model.
