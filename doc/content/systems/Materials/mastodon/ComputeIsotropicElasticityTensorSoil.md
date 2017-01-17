#ComputeIsotropicElasticityTensorSoil

!description /Materials/ComputeIsotropicElasticityTensorSoil

## Description
There are three essential components which describe the mechanical behavior of a material. These are strain formulation, stress formulation and an elasticity tensor which connects the strains to stresses. In MASTODON, these three can be specified separately. This class defines the elasticity tensor for an isotropic material.

The elasticity tensor ($C_{ijkl}$) is a fourth order tensor containing 81 components. In the case of an isotropic material, $C_{ijkl}$ can be constructed using just 2 independent constants which in this case are the shear modulus ($G$) and the Poisson's ratio ($\nu$). The elasticity tensor is a material property which is defined at quadrature point. So different parts of the domain can have different values for $C_{ijkl}$.

This material has been designed for a layered soil material in which each layer can have a different shear modulus, Poisson's ratio and density ($\rho$). The different layers are distinguished using an elemental AuxVariable that defines a "layer id" for each element within the mesh. Thus, the layer variable is a required parameter in the input. The shear modulus, Poisson's ratio and density are mapped to this "layer id".

This material also computes and stores the shear wave speed ($V_s$), P wave speed ($V_p$) for quadrature point from the layer id, shear modulus, Poisson's ratio and density of the material.

The shear wave speed is calculated as:
$$
V_s = \sqrt{\frac{G}{\rho}}
$$

The P wave speed is calculated as:
$$
V_p = \sqrt{\frac{2G(1+\nu)}{\rho}}
$$

!parameters /Materials/ComputeIsotropicElasticityTensorSoil

!inputfiles /Materials/ComputeIsotropicElasticityTensorSoil

!childobjects /Materials/ComputeIsotropicElasticityTensorSoil
