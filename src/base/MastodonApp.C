#include "AppFactory.h"
#include "MastodonApp.h"
#include "Moose.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

// Optional dependence on BlackBear
#ifdef BLACKBEAR_ENABLED
#include "BlackBearApp.h"
#endif

InputParameters
MastodonApp::validParams()
{
  InputParameters params = MooseApp::validParams();
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
  MastodonApp::registerAll(_factory, _action_factory, _syntax);
}

void
MastodonApp::registerApps()
{
  registerApp(MastodonApp);
  ModulesApp::registerApps();
#ifdef BLACKBEAR_ENABLED
  BlackBearApp::registerApps();
#endif
}

void
MastodonApp::registerAll(Factory & factory, ActionFactory & action_factory, Syntax & syntax)
{

  Registry::registerObjectsTo(factory, {"MastodonApp"});
  Registry::registerActionsTo(action_factory, {"MastodonApp"});
  ModulesApp::registerAllObjects<MastodonApp>(factory, action_factory, syntax);
#ifdef BLACKBEAR_ENABLED
  BlackBearApp::registerAll(factory, action_factory, syntax);
#endif

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

  registerDataFilePath();
}

extern "C" void
MastodonApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  MastodonApp::registerAll(f, af, s);
}
