#include "AppFactory.h"
#include "MastodonApp.h"
#include "Moose.h"
#include "MooseSyntax.h"

// Modules
#include "ContactApp.h"
#include "SolidMechanicsApp.h"
#include "TensorMechanicsApp.h"
#include "StochasticToolsApp.h"

// Actions
#include "MastodonModelAction.h"
#include "MastodonOutputsAction.h"
#include "ISoilAction.h"
#include "NonReflectingBCAction.h"
#include "SeismicDisplacementAction.h"
#include "SeismicForceAction.h"
#include "SeismicSourceAction.h"

// BCs
#include "NonReflectingBC.h"
#include "SeismicForce.h"

// AuxKernels
#include "UniformLayerAuxKernel.h"

// Dirackernels
#include "FunctionPointForce.h"
#include "SeismicSource.h"

// Indicators
#include "LayerInterfaceIndicator.h"
#include "ShearWaveIndicator.h"

// Materials
#include "ComputeISoilStress.h"
#include "ComputeIsotropicElasticityTensorSoil.h"
#include "LinearSoilMaterial.h"

// Markers
#include "MinimumElementSizeMarker.h"

// VectorPostprocessors
#include "ResponseHistoryBuilder.h"
#include "ResponseSpectraCalculator.h"
#include "HousnerSpectrumIntensity.h"
#include "Fragility.h"

// UserObjects
#include "GroundMotionReader.h"
#include "HazardCurve.h"

// MultiApp
#include "HazardCurveMultiApp.h"

// Transfers
#include "HazardCurveTransfer.h"
#include "PiecewiseFunctionTransfer.h"

// Testing
#include "TestLayeredMaterialInterface.h"

template <>
InputParameters
validParams<MastodonApp>()
{
  InputParameters params = validParams<MooseApp>();
  params.set<bool>("error_unused") = true;
  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

MastodonApp::MastodonApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  MastodonApp::registerObjects(_factory);
  MastodonApp::registerObjectDepends(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  MastodonApp::associateSyntax(_syntax, _action_factory);
  MastodonApp::associateSyntaxDepends(_syntax, _action_factory);
}

MastodonApp::~MastodonApp() {}

// External entry point for dynamic application loading
extern "C" void
MastodonApp__registerApps()
{
  MastodonApp::registerApps();
}
void
MastodonApp::registerApps()
{
  registerApp(MastodonApp);
}

// External entry point for dynamic object registration
extern "C" void
MastodonApp__registerObjects(Factory & factory)
{
  MastodonApp::registerObjects(factory);
}
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

  // VectorPostprocessors
  registerVectorPostprocessor(ResponseHistoryBuilder);
  registerVectorPostprocessor(ResponseSpectraCalculator);
  registerVectorPostprocessor(HousnerSpectrumIntensity);
  registerVectorPostprocessor(Fragility);

  // UserObjects
  registerUserObject(GroundMotionReader);
  registerUserObject(HazardCurve);

  // MultiApps
  registerMultiApp(HazardCurveMultiApp);

  // Transfers
  registerTransfer(HazardCurveTransfer);
  registerTransfer(PiecewiseFunctionTransfer);

  // Testing
  registerMaterial(TestLayeredMaterialInterfaceDocString);
  registerKernel(TestLayeredMaterialInterfaceTypeError);
}

// External entry point for dynamic syntax association
extern "C" void
MastodonApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  MastodonApp::associateSyntax(syntax, action_factory);
}
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

  syntax.registerActionSyntax("MastodonModelAction", "Mastodon/Model");
  registerAction(MastodonModelAction, "meta_action");
  registerAction(MastodonModelAction, "add_variable");
  registerAction(MastodonModelAction, "add_aux_variable");
  registerAction(MastodonModelAction, "add_kernel");
  registerAction(MastodonModelAction, "add_aux_kernel");

  syntax.registerActionSyntax("MastodonOutputsAction", "Mastodon/Outputs");
  registerAction(MastodonOutputsAction, "add_aux_variable");
  registerAction(MastodonOutputsAction, "add_aux_kernel");
}

void
MastodonApp::registerObjectDepends(Factory & factory)
{
  SolidMechanicsApp::registerObjects(factory);
  TensorMechanicsApp::registerObjects(factory);
  ContactApp::registerObjects(factory);
  StochasticToolsApp::registerObjects(factory);
}

void
MastodonApp::associateSyntaxDepends(Syntax & syntax, ActionFactory & action_factory)
{
  SolidMechanicsApp::associateSyntax(syntax, action_factory);
  TensorMechanicsApp::associateSyntax(syntax, action_factory);
  ContactApp::associateSyntax(syntax, action_factory);
  StochasticToolsApp::associateSyntax(syntax, action_factory);
}
