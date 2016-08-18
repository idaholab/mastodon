#include "MastodonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

// Materials
#include "LinearSoilMaterial.h"

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
  ModulesApp::registerObjects(_factory);
  MastodonApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  MastodonApp::associateSyntax(_syntax, _action_factory);
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
  // Materials
  registerMaterial(LinearSoilMaterial);
}

// External entry point for dynamic syntax association
extern "C" void MastodonApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { MastodonApp::associateSyntax(syntax, action_factory); }
void
MastodonApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
