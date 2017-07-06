# LayeredMaterialInterface

The LayeredMaterialInterface allows Material properties that vary with "layer id" to be rapidly developed by managing
the connection between the layer and input parameters. The following sections discuss the purpose and use of this
interface.

## Layer IDs
Foremost, this interface relies on the concept of layer ids, which in Mastodon are defined by a field variable, which
is typically an AuxVariable that defines regions within the domain that represent areas where material properties
vary (e.g., due to changing soil type). The LayeredMaterialInterface must be supplied the layer id field variable, via
the "layer_variable" parameter.

## Layer Parameters
When defining a Material object it is often desired to vary the value of a property according to the layer id. This
connection between layer id and property value occurs in the input file using the "up-and-down" syntax that is
ubiquitous throughout MOOSE.

Firstly, using this interface adds a "layer_ids" parameter, which contains a list that will linked one-to-one with
other input parameters. For example, in the input file snippet below two parameters ("shear_modulus" and "density")
are to be connected to the corresponding value in the "layer_ids" parameter.

!listing tests/materials/linear_soil/linear_soil.i block=Materials label=False

Within the code layer parameters should be accessed using the "getLayerParam" method that is defined in the LayeredMaterialInterface class. This method returns a reference to a container that is sized according to the number of quadrature points, in similar fashion to MooseVariable for MaterialProperty types, allowing the usage of these
layer parameters to be trivial. When the parameters are utilized within the computeQpProperties method they are updated
automatically to contain the correct values based on the values of the layer variable at the quadrature points.


The [LinearSoilMaterial](mastodon/LinearSoilMaterial.md) should be used as a guide for creating new Material objects that require the use of layer parameters. In general, there is a three step process for defining layer parameters. First, the parameter is defined in the validParams function of the object being created.

!listing src/materials/LinearSoilMaterial.C label=False start="shear_modulus" end=addClassDescription strip-leading-whitespace=True

Second, the parameters are stored as member variables (`const MooseArray<Real> &`) in the initialization list using
the `getLayerParam` method.

!!! note
    When adding a parameter, the type should be a `std::vector<T>`, where T is any type supported by the InputParameters
    object. When using the 'getLayerParam' method, the template parameter type is also T. In the case shown here `T = Real`.

!listing src/materials/LinearSoilMaterial.C label=False start=_layer_shear_modulus end={ strip-leading-whitespace=True

Finally, the layer parameters are utilized to compute a material property.

!listing src/materials/LinearSoilMaterial.C label=False strip-leading-whitespace=True line=_shear_wave_speed[_qp]

## Documentation
To ensure that documentation for the "layer_ids" parameters is accurate it is required that the documentation string be modified by the parent class using the `setDocString`. This is done to ensure that some reference to the corresponding parameters is defined. For example, the following is found in the [LinearSoilMaterial](mastodon/LinearSoilMaterial.md) `validParams` function.

!listing src/materials/LinearSoilMaterial.C label=False strip-leading-whitespace=True line=setDocString
