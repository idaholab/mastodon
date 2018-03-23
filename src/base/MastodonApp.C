#include "AppFactory.h"
#include "MastodonApp.h"
#include "Moose.h"
#include "MooseSyntax.h"

// Modules
#include "ContactApp.h"
#include "SolidMechanicsApp.h"
#include "TensorMechanicsApp.h"
#include "StochasticToolsApp.h"

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

// When using the new Registry system, this line is required so that
// dependent apps know about the MastodonApp label.
registerKnownLabel("MastodonApp");

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
  Registry::registerObjectsTo(factory, {"MastodonApp"});
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
  Registry::registerActionsTo(action_factory, {"MastodonApp"});

  syntax.registerActionSyntax("EmptyAction", "BCs/SeismicForce");
  syntax.registerActionSyntax("SeismicForceAction", "BCs/SeismicForce/*");

  syntax.registerActionSyntax("EmptyAction", "BCs/SeismicDisplacement");
  syntax.registerActionSyntax("SeismicDisplacementAction", "BCs/SeismicDisplacement/*");

  syntax.registerActionSyntax("EmptyAction", "BCs/NonReflectingBC");
  syntax.registerActionSyntax("NonReflectingBCAction", "BCs/NonReflectingBC/*");

  syntax.registerActionSyntax("EmptyAction", "DiracKernels/SeismicSource");
  syntax.registerActionSyntax("SeismicSourceAction", "DiracKernels/SeismicSource/*");

  syntax.registerActionSyntax("EmptyAction", "Materials/I_Soil");
  syntax.registerActionSyntax("ISoilAction", "Materials/I_Soil/*");

  syntax.registerActionSyntax("MastodonModelAction", "Mastodon/Model");

  syntax.registerActionSyntax("MastodonOutputsAction", "Mastodon/Outputs");
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
