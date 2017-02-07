#include "MastodonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

// Modules
#include "SolidMechanicsApp.h"
#include "TensorMechanicsApp.h"
#include "ContactApp.h"

// Actions
#include "NonReflectingBCAction.h"
#include "SeismicForceAction.h"
#include "SeismicDisplacementAction.h"
#include "SeismicSourceAction.h"
#include "ISoilAction.h"

// BCs
#include "NonReflectingBC.h"
#include "SeismicForce.h"

// AuxKernels
#include "UniformLayerAuxKernel.h"

// Dirackernels
#include "FunctionPointForce.h"
#include "SeismicSource.h"

// Indicators
#include "ShearWaveIndicator.h"
#include "LayerInterfaceIndicator.h"

// Materials
#include "LinearSoilMaterial.h"
#include "ComputeIsotropicElasticityTensorSoil.h"
#include "ComputeISoilStress.h"

// Markers
#include "MinimumElementSizeMarker.h"

template<>
InputParameters validParams<MastodonApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

MastodonApp::MastodonApp(InputParameters parameters) :
    MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  MastodonApp::registerObjects(_factory);
  SolidMechanicsApp::registerObjects(_factory);
  TensorMechanicsApp::registerObjects(_factory);
  ContactApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  MastodonApp::associateSyntax(_syntax, _action_factory);
  SolidMechanicsApp::associateSyntax(_syntax, _action_factory);
  TensorMechanicsApp::associateSyntax(_syntax, _action_factory);
  ContactApp::associateSyntax(_syntax, _action_factory);
}

MastodonApp::~MastodonApp()
{
}

// External entry point for dynamic application loading
extern "C" void MastodonApp__registerApps() { MastodonApp::registerApps(); }
void
MastodonApp::registerApps()
{
  registerApp(MastodonApp);
}

// External entry point for dynamic object registration
extern "C" void MastodonApp__registerObjects(Factory & factory) { MastodonApp::registerObjects(factory); }
void
MastodonApp::registerObjects(Factory & factory)
{
  // BCs
  registerBoundaryCondition(NonReflectingBC);
  registerBoundaryCondition(SeismicForce);

  // AuxKernels
  registerAuxKernel(UniformLayerAuxKernel);

  // DiracKernels
  registerDiracKernel(FunctionPointForce);
  registerDiracKernel(SeismicSource);

  // Indicators
  registerIndicator(ShearWaveIndicator);
  registerIndicator(LayerInterfaceIndicator);

  // Materials
  registerMaterial(LinearSoilMaterial);
  registerMaterial(ComputeIsotropicElasticityTensorSoil);
  registerMaterial(ComputeISoilStress);

  // Markers
  registerMarker(MinimumElementSizeMarker);
}

// External entry point for dynamic syntax association
extern "C" void MastodonApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { MastodonApp::associateSyntax(syntax, action_factory); }
void
MastodonApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  syntax.registerActionSyntax("EmptyAction", "BCs/SeismicForce");
  syntax.registerActionSyntax("SeismicForceAction", "BCs/SeismicForce/*");
  registerAction(SeismicForceAction, "add_bc");

  syntax.registerActionSyntax("EmptyAction", "BCs/SeismicDisplacement");
  syntax.registerActionSyntax("SeismicDisplacementAction", "BCs/SeismicDisplacement/*");
  registerAction(SeismicDisplacementAction, "add_bc");

  syntax.registerActionSyntax("EmptyAction", "BCs/NonReflectingBC");
  syntax.registerActionSyntax("NonReflectingBCAction", "BCs/NonReflectingBC/*");
  registerAction(NonReflectingBCAction, "add_bc");

  syntax.registerActionSyntax("EmptyAction", "DiracKernels/SeismicSource");
  syntax.registerActionSyntax("SeismicSourceAction", "DiracKernels/SeismicSource/*");
  registerAction(SeismicSourceAction, "add_dirac_kernel");

  syntax.registerActionSyntax("EmptyAction", "Materials/I_Soil");
  syntax.registerActionSyntax("ISoilAction", "Materials/I_Soil/*");
  registerAction(ISoilAction, "add_material");
}
