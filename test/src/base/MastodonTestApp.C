#include "MastodonTestApp.h"
#include "MastodonApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
MastodonTestApp::validParams()
{
  InputParameters params = MastodonApp::validParams();
  return params;
}

MastodonTestApp::MastodonTestApp(InputParameters parameters) : MooseApp(parameters)
{
  MastodonTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

void
MastodonTestApp::registerAll(Factory & factory,
                             ActionFactory & action_factory,
                             Syntax & syntax,
                             bool use_test_objs)
{
  MastodonApp::registerAll(factory, action_factory, syntax);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(factory, {"MastodonTestApp"});
    Registry::registerActionsTo(action_factory, {"MastodonTestApp"});
  }
}

void
MastodonTestApp::registerApps()
{
  registerApp(MastodonTestApp);
}
