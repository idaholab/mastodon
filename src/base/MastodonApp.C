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
#include "SeismicInputAction.h"

// BCs
#include "NonReflectingBC.h"
#include "SeismicForce.h"

// Dirackernels
#include "FunctionPointForce.h"

// Indicators
#include "ShearWaveIndicator.h"
#include "LayerInterfaceIndicator.h"

// Materials
#include "LinearSoilMaterial.h"
#include "ComputeIsotropicElasticityTensorSoil.h"

// Markers
#include "MinimumElementSizeMarker.h"

// Problems
#include "MastodonProblem.h"

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

  // DiracKernels
  registerDiracKernel(FunctionPointForce);

  // Indicators
  registerIndicator(ShearWaveIndicator);
  registerIndicator(LayerInterfaceIndicator);

  // Materials
  registerMaterial(LinearSoilMaterial);
  registerMaterial(ComputeIsotropicElasticityTensorSoil);

  // Markers
  registerMarker(MinimumElementSizeMarker);

  // Problems
  registerProblem(MastodonProblem);
}

// External entry point for dynamic syntax association
extern "C" void MastodonApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { MastodonApp::associateSyntax(syntax, action_factory); }
void
MastodonApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  syntax.registerActionSyntax("EmptyAction", "BCs/SeismicInput");
  syntax.registerActionSyntax("SeismicInputAction", "BCs/SeismicInput/*");

  syntax.registerActionSyntax("EmptyAction", "BCs/NonReflectingBC");
  syntax.registerActionSyntax("NonReflectingBCAction", "BCs/NonReflectingBC/*");

  registerAction(SeismicInputAction, "add_bc");
  registerAction(NonReflectingBCAction, "add_bc");
}
