# Example 4a: Frictional contact element test (pseudo-static loading)

This example demonstrates a frictional contact using the user-defined backbone I-soil material model in MASTODON. Two brick elements are stacked vertically and a thin layer is modeled to describe the frictional contact (see [fig:undeformedcontact]). Top and bottom layers use linear elastic material model whereas thin layer uses I-soil to approximate a contact behavior with elastic - nearly perfectly plastic shear behavior.

!alert note
This type of backbone curve can now be automated using the 'thin_layer' option in I-soil and
ComputeISoilStress.

!listing examples/ex04/ex04a/2BlockFriction_Isoilunit.i


!media media/examples/undeformedcontact.png
       style=width:80%;margin-left:200px;float:center;
       id=fig:undeformedcontact
       caption=Undeformed shape of the numerical model.

The domain is loaded with gravity and top element is sheared on the top surface. At intermediate
steps whereby the contact strength is yet not mobilized, no separation occurs between two elastic
blocks as it is shown in [fig:intermediate].

!media media/examples/intermediate.png
       style=width:30%;margin-left:200px;float:center;
       id=fig:intermediate
       caption=Intermediate step whereby the contact layer strength is not mobilized.

Once the contact layer is yielded, the upper block starts to slide on top of the lower block and thin
layer effectively simulates the interface between two elastic layers (see [fig:deformed]).

!media media/examples/deformed.png
       style=width:30%;margin-left:200px;float:center;
       id=fig:deformed
       caption=Deformed shape of the numerical model after the contact is yielded.

# Example 4b: Frictional contact element test (cyclic loading)

This example is a more general representation of the three block contact problem demonstrated in Example 4a. The problem formulation is exactly the same as the thin layer I-soil contact element sandwiched betweeen the concrete block and the soil block, which uses linear elastic material (see [fig:deformedcontact]). In the input file shown below; the  Materials, Functions, BC, and Controls block will be explained in some detail which is specific to the soil-structure interaction problem.

!listing examples/ex04/ex04b/2BlockFriction_Isoilunit.i

For this example, the units defining the parameters are N, kg, and meter (m); and the material definition is created using the following input:

!listing examples/ex04/ex04b/2BlockFriction_Isoilunit.i
 start=Materials
 end=Preconditioning

where;

- `soil type = 'user-defined'` specifies that the backbone curve will be defined by the user and will correspond to a Coulomb friction formulation.

- `backbone curve = backbone_curveunit.csv` The backbone curve is used to define critical shear stress at the interface which is obtained by multiplying the user-defined friction coefficient with the normal stresses . The shear strain is obtained by dividing the frictional shear stress with the shear modulus of the concrete. The above mentioned stress-strain values are entered via a .csv file or can be defined directly. The initial shear modulus of the thin layer is the same as that of concrete. The motivation is to model the contact element as a set of springs which yields once the critical stresses are reached.

- `initial_shear_modulus = '2.71e10'` specifies the small strain shear modulus (elastic property) of
 the soil and is equivalent to the shear modulus of the concrete block. It should be noted that if we were to model an embedded foundation the shear modulus of the thin layer must match that of the surrounding soil strata.

- `poissons_ratio = '0.15'` specifies the Poisson's ratio for the thin layer elements (same as concrete). The linear elastic properties are matched with concrete in order to ensure uniform scaling of stresses during failure.

- `block = 1002` specifies that the material will be assigned to a mesh volume with block id = 1002.

- `initial_stress = 'initial_ymid 0 0 0 initial_ymid 0 0 0 initial_zzmid` see [Getting Started](manuals/user/index.md). This is not needed if pressure dependency is set to true or when the initial stresses are unknown.

- `density = '8792'` specifies the density of the thin layer which is the same as concrete.

- `p_ref = '904918'` is the reference pressure at which the backbone curve is defined. The reference pressure is obtained by performing a gravity analysis and switching off the inertia kernel for the first few time steps. This helps us in avoiding unrealistic displacement/acceleration response.

The documentation for computation of the finite elastic stresses and strains can be found in [modules/tensor_mechanics/index.md]

!media media/examples/unittestcyclic.jpg
      style=width:60%;margin-left:200px;float:center;
      id=fig:deformedcontact
      caption=Deformed shape of the numerical model.

The boundary conditions and the supporting functions are given in the BCs and Functions block respectively.

!listing examples/ex04/ex04b/2BlockFriction_Isoilunit.i
 start=BC
 end=Functions

where;

- `type = PresetBC` is used to declare fixed boundary condition for the soil block, where the displacements in the chosen boundaries in the x, y and z direction are assigned with the value 0.

- `type = PresetDisplacement` This is used to define the displacement boundary condition on a specific boundary when inertial effects are taken into account. The PresetDisplacement takes the boundary id and the value of the displacement (which can be either expressed as constant value or as a function) as input.

- `function = loading_bc'` specifies the displacement loading function to be input through the Functions block. The loading_bc in this problem is a sawtooth curve with the loading being applied after the first few time steps in order to stabilize gravity effects. The displacement and acceleration response output of the thin layer element must be verified with the input to check for discrepancies. It is advisable to use a displacement-time history with incremental amplitude to tackle sudden spikes in acceleration/velocity response due to convergence issues arising out of inconsistent timestep size input.

The Controls block is used to switch off inertia kernels and auxkernels in the first few time steps when gravity is applied.

!listing examples/ex04/ex04b/2BlockFriction_Isoilunit.i
 start=Controls
 end=Executioner


From the results it is observed that the thin layer element does not reach the critical yield stress in intermediate steps when the contact strength is not completely mobilized thereby preventing separation of the two blocks (see [fig:results]).

!media media/examples/unittestcycresults.jpg
      style=width:90%;margin-left:100px;float:center;
      id=fig:results
      caption=Shear stress vs. shear strain response of the interface element.

Once the contact element is yielded, the upper block starts sliding on top of the lower block and the average stresses in the thin layer equal the Coulomb frictional shear stresses.
