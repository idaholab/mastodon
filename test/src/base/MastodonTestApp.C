#include "MastodonTestApp.h"
#include "MastodonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<MastodonTestApp>()
{
  InputParameters params = validParams<MastodonApp>();
  return params;
}

MastodonTestApp::MastodonTestApp(InputParameters parameters) : MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  MastodonApp::registerObjectDepends(_factory);
  MastodonApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  MastodonApp::associateSyntaxDepends(_syntax, _action_factory);
  MastodonApp::associateSyntax(_syntax, _action_factory);

  Moose::registerExecFlags(_factory);
  ModulesApp::registerExecFlags(_factory);
  MastodonApp::registerExecFlags(_factory);

  bool use_test_objs = getParam<bool>("allow_test_objects");
  if (use_test_objs)
  {
    MastodonTestApp::registerObjects(_factory);
    MastodonTestApp::associateSyntax(_syntax, _action_factory);
    MastodonTestApp::registerExecFlags(_factory);
  }
}

void
MastodonTestApp::registerApps()
{
  registerApp(MastodonApp);
  registerApp(MastodonTestApp);
}

void
MastodonTestApp::registerObjects(Factory & factory)
{
  Registry::registerObjectsTo(factory, {"MastodonTestApp"});
}

void
MastodonTestApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & action_factory)
{
  Registry::registerActionsTo(action_factory, {"MastodonTestApp"});
}

void
MastodonTestApp::registerExecFlags(Factory & /*factory*/)
{
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/

// External entry point for dynamic application loading
extern "C" void
MastodonTestApp__registerApps()
{
  MastodonTestApp::registerApps();
}

// External entry point for dynamic syntax association
extern "C" void
MastodonTestApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
  MastodonTestApp::associateSyntax(syntax, action_factory);
}

// External entry point for dynamic object registration
extern "C" void
MastodonTestApp__registerObjects(Factory & factory)
{
  MastodonTestApp::registerObjects(factory);
}

// External entry point for dynamic execute flag registration
extern "C" void
MastodonTestApp__registerExecFlags(Factory & factory)
{
  MastodonTestApp::registerExecFlags(factory);
}
